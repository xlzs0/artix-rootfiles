#define _GNU_SOURCE
#include <crypt.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <security/pam_appl.h>
#include <security/pam_ext.h>
#include <security/pam_modules.h>
#include <security/pam_modutil.h>
#include <shadow.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#define MAX_ITEM PAM_AUTHTOK_TYPE
#ifndef CHKPWD_HELPER
#define CHKPWD_HELPER "/usr/bin/unix_chkpwd"
#endif

struct data_item {
    char* name;
    void* data;
    void (*cleanup)(pam_handle_t*, void*, int);
    struct data_item* next;
};

struct pam_handle {
    void* items[MAX_ITEM + 1];
    char** env;
    struct data_item* data;
    unsigned int fail_delay;
};

static const char* errors[] = { "Success",
                                "dlopen failure",
                                "Symbol not found",
                                "Service error",
                                "System error",
                                "Memory buffer error",
                                "Permission denied",
                                "Authentication failure",
                                "Credentials insufficient",
                                "Authentication information unavailable",
                                "User unknown",
                                "Maximum tries exceeded",
                                "New authentication token required",
                                "Account expired",
                                "Session error",
                                "Credentials unavailable",
                                "Credentials expired",
                                "Credentials error",
                                "No module data",
                                "Conversation error",
                                "Authentication token error",
                                "Authentication token recovery error",
                                "Authentication token lock busy",
                                "Authentication token aging disabled",
                                "Try again",
                                "Ignore",
                                "Abort",
                                "Authentication token expired",
                                "Module unknown",
                                "Bad item",
                                "Conversation again",
                                "Incomplete" };

static int string_item(int item) {
    switch (item) {
    case PAM_SERVICE:
    case PAM_USER:
    case PAM_TTY:
    case PAM_RHOST:
    case PAM_AUTHTOK:
    case PAM_OLDAUTHTOK:
    case PAM_RUSER:
    case PAM_USER_PROMPT:
    case PAM_XDISPLAY:
    case PAM_AUTHTOK_TYPE:
        return 1;
    default:
        return 0;
    }
}

static void free_item(int item, void* value) {
    if (!value)
        return;
    if (string_item(item) || item == PAM_CONV)
        free(value);
    else if (item == PAM_XAUTHDATA) {
        struct pam_xauth_data* xauth = value;
        free(xauth->name);
        free(xauth->data);
        free(xauth);
    }
}

static int dup_item(int item, const void* in, void** out) {
    *out = NULL;
    if (!in)
        return PAM_SUCCESS;
    if (string_item(item)) {
        *out = strdup(in);
        return *out ? PAM_SUCCESS : PAM_BUF_ERR;
    }
    if (item == PAM_CONV) {
        *out = malloc(sizeof(struct pam_conv));
        if (!*out)
            return PAM_BUF_ERR;
        memcpy(*out, in, sizeof(struct pam_conv));
        return PAM_SUCCESS;
    }
    if (item == PAM_XAUTHDATA) {
        const struct pam_xauth_data* src = in;
        struct pam_xauth_data* dst = malloc(sizeof(*dst));
        if (!dst)
            return PAM_BUF_ERR;
        *dst = *src;
        dst->name = NULL;
        dst->data = NULL;
        if (src->name && src->namelen > 0) {
            dst->name = malloc((size_t)src->namelen + 1);
            if (!dst->name) {
                free(dst);
                return PAM_BUF_ERR;
            }
            memcpy(dst->name, src->name, (size_t)src->namelen);
            dst->name[src->namelen] = '\0';
        }
        if (src->data && src->datalen > 0) {
            dst->data = malloc((size_t)src->datalen);
            if (!dst->data) {
                free(dst->name);
                free(dst);
                return PAM_BUF_ERR;
            }
            memcpy(dst->data, src->data, (size_t)src->datalen);
        }
        *out = dst;
        return PAM_SUCCESS;
    }
    *out = (void*)in;
    return PAM_SUCCESS;
}

int pam_set_item(pam_handle_t* pamh, int item_type, const void* item) {
    void* copy;
    int ret;

    if (!pamh || item_type <= 0 || item_type > MAX_ITEM)
        return PAM_BAD_ITEM;
    ret = dup_item(item_type, item, &copy);
    if (ret != PAM_SUCCESS)
        return ret;
    free_item(item_type, pamh->items[item_type]);
    pamh->items[item_type] = copy;
    return PAM_SUCCESS;
}

int pam_get_item(const pam_handle_t* pamh, int item_type, const void** item) {
    if (!pamh || !item || item_type <= 0 || item_type > MAX_ITEM)
        return PAM_BAD_ITEM;
    *item = pamh->items[item_type];
    return PAM_SUCCESS;
}

int pam_start(const char* service_name, const char* user, const struct pam_conv* pam_conversation, pam_handle_t** pamh) {
    return pam_start_confdir(service_name, user, pam_conversation, NULL, pamh);
}

int pam_start_confdir(
    const char* service_name, const char* user, const struct pam_conv* pam_conversation, const char* confdir, pam_handle_t** pamh) {
    pam_handle_t* h;
    int ret;

    if (!service_name || !pam_conversation || !pamh)
        return PAM_SYSTEM_ERR;
    *pamh = NULL;
    h = calloc(1, sizeof(*h));
    if (!h)
        return PAM_BUF_ERR;
    if ((ret = pam_set_item(h, PAM_SERVICE, service_name)) != PAM_SUCCESS ||
        (user && (ret = pam_set_item(h, PAM_USER, user)) != PAM_SUCCESS) ||
        (ret = pam_set_item(h, PAM_CONV, pam_conversation)) != PAM_SUCCESS) {
        pam_end(h, ret);
        return ret;
    }
    *pamh = h;
    return PAM_SUCCESS;
}

int pam_end(pam_handle_t* pamh, int pam_status) {
    struct data_item* d;

    if (!pamh)
        return PAM_SYSTEM_ERR;
    for (int i = 1; i <= MAX_ITEM; i++)
        free_item(i, pamh->items[i]);
    for (char** e = pamh->env; e && *e; e++)
        free(*e);
    free(pamh->env);
    d = pamh->data;
    while (d) {
        struct data_item* next = d->next;
        if (d->cleanup)
            d->cleanup(pamh, d->data, pam_status);
        free(d->name);
        free(d);
        d = next;
    }
    free(pamh);
    return PAM_SUCCESS;
}

static int ask(pam_handle_t* pamh, int style, const char* prompt, char** answer) {
    const struct pam_conv* conv;
    const struct pam_message msg = { style, prompt };
    const struct pam_message* msgp = &msg;
    struct pam_response* resp = NULL;
    int ret;

    if (!pamh || !answer)
        return PAM_CONV_ERR;
    conv = pamh->items[PAM_CONV];
    if (!conv || !conv->conv)
        return PAM_CONV_ERR;
    ret = conv->conv(1, &msgp, &resp, conv->appdata_ptr);
    if (ret != PAM_SUCCESS || !resp)
        return ret == PAM_SUCCESS ? PAM_CONV_ERR : ret;
    *answer = resp[0].resp;
    free(resp);
    return PAM_SUCCESS;
}

int pam_get_user(pam_handle_t* pamh, const char** user, const char* prompt) {
    const void* item = NULL;
    char* answer = NULL;
    int ret;

    if (!pamh || !user)
        return PAM_SYSTEM_ERR;
    pam_get_item(pamh, PAM_USER, &item);
    if (!item) {
        ret = ask(pamh, PAM_PROMPT_ECHO_ON, prompt ? prompt : "login: ", &answer);
        if (ret != PAM_SUCCESS)
            return ret;
        ret = pam_set_item(pamh, PAM_USER, answer);
        free(answer);
        if (ret != PAM_SUCCESS)
            return ret;
        pam_get_item(pamh, PAM_USER, &item);
    }
    *user = item;
    return PAM_SUCCESS;
}

int pam_get_authtok(pam_handle_t* pamh, int item, const char** authtok, const char* prompt) {
    const void* tok = NULL;
    char* answer = NULL;
    int ret;

    if (!pamh || !authtok || (item != PAM_AUTHTOK && item != PAM_OLDAUTHTOK))
        return PAM_SYSTEM_ERR;
    pam_get_item(pamh, item, &tok);
    if (!tok) {
        const char* p = prompt ? prompt : (item == PAM_OLDAUTHTOK ? "Current password: " : "Password: ");
        ret = ask(pamh, PAM_PROMPT_ECHO_OFF, p, &answer);
        if (ret != PAM_SUCCESS)
            return ret;
        ret = pam_set_item(pamh, item, answer);
        free(answer);
        if (ret != PAM_SUCCESS)
            return ret;
        pam_get_item(pamh, item, &tok);
    }
    *authtok = tok;
    return PAM_SUCCESS;
}

int pam_get_authtok_noverify(pam_handle_t* pamh, const char** authtok, const char* prompt) {
    return pam_get_authtok(pamh, PAM_AUTHTOK, authtok, prompt);
}

int pam_get_authtok_verify(pam_handle_t* pamh, const char** authtok, const char* prompt) {
    return pam_get_authtok(pamh, PAM_AUTHTOK, authtok, prompt);
}

static int locked_hash(const char* hash) {
    return !hash || !*hash || hash[0] == '!' || hash[0] == '*';
}

static int run_unix_chkpwd(pam_handle_t* pamh, const char* user, const char* pass, int flags) {
    int fds[2];
    pid_t child;
    int retval = PAM_AUTH_ERR;
    int wstatus;

    if (!user || !*user)
        return PAM_USER_UNKNOWN;
    if (pipe(fds) != 0)
        return PAM_AUTHINFO_UNAVAIL;

    child = fork();
    if (child == 0) {
        static char* envp[] = { NULL };
        const char* args[] = { CHKPWD_HELPER, user, (flags & PAM_DISALLOW_NULL_AUTHTOK) ? "nonull" : "nullok", NULL };

        if (dup2(fds[0], STDIN_FILENO) != STDIN_FILENO)
            _exit(PAM_AUTHINFO_UNAVAIL);
        close(fds[0]);
        close(fds[1]);

        if (setuid(0) == -1 && geteuid() == 0)
            _exit(PAM_AUTHINFO_UNAVAIL);

        execve(CHKPWD_HELPER, (char* const*)args, envp);
        _exit(PAM_AUTHINFO_UNAVAIL);
    }

    if (child < 0) {
        close(fds[0]);
        close(fds[1]);
        return PAM_AUTHINFO_UNAVAIL;
    }

    close(fds[0]);
    if (pass) {
        size_t len = strlen(pass);
        if (len > PAM_MAX_RESP_SIZE)
            len = PAM_MAX_RESP_SIZE;
        if (write(fds[1], pass, len) < 0 || write(fds[1], "", 1) < 0)
            retval = PAM_AUTH_ERR;
        else
            retval = PAM_SUCCESS;
    } else {
        if (write(fds[1], "", 1) < 0)
            retval = PAM_AUTH_ERR;
        else
            retval = PAM_SUCCESS;
    }
    close(fds[1]);

    while (waitpid(child, &wstatus, 0) < 0) {
        if (errno != EINTR) {
            pam_syslog(pamh, LOG_ERR, "unix_chkpwd waitpid failed: %m");
            return PAM_AUTH_ERR;
        }
    }
    if (!WIFEXITED(wstatus)) {
        pam_syslog(pamh, LOG_ERR, "unix_chkpwd abnormal exit");
        return PAM_AUTH_ERR;
    }
    if (retval != PAM_AUTH_ERR)
        retval = WEXITSTATUS(wstatus);
    return retval;
}

int pam_authenticate(pam_handle_t* pamh, int flags) {
    const char* user = NULL;
    const char* pass = NULL;
    struct spwd* sp;
    char* calc;
    int ret;
    int sp_errno = 0;

    if ((ret = pam_get_user(pamh, &user, NULL)) != PAM_SUCCESS)
        return ret;
    if (!user || !*user)
        goto fail_user_unknown;
    errno = 0;
    sp = getspnam(user);
    sp_errno = errno;
    if ((ret = pam_get_authtok(pamh, PAM_AUTHTOK, &pass, NULL)) != PAM_SUCCESS)
        return ret;
    if ((!pass || !*pass) && (flags & PAM_DISALLOW_NULL_AUTHTOK))
        goto fail_auth_err;
    if (!sp) {
        ret = sp_errno == EACCES ? run_unix_chkpwd(pamh, user, pass, flags) : PAM_USER_UNKNOWN;
        goto fail;
    }
    if (locked_hash(sp->sp_pwdp))
        goto fail_auth_err;
    calc = crypt(pass ? pass : "", sp->sp_pwdp);
    if (!calc)
        goto fail_authinfo_unavail;
    if (strcmp(calc, sp->sp_pwdp) == 0)
        return PAM_SUCCESS;
    goto fail_auth_err;

fail_user_unknown:
    ret = PAM_USER_UNKNOWN;
    goto fail;
fail_authinfo_unavail:
    ret = PAM_AUTHINFO_UNAVAIL;
    goto fail;
fail_auth_err:
    ret = PAM_AUTH_ERR;
fail:
    pam_set_item(pamh, PAM_AUTHTOK, NULL);
    return ret;
}

static int verify_password_hash(const char* password, const char* hash) {
    char* calc;

    if (locked_hash(hash))
        return PAM_AUTH_ERR;
    calc = crypt(password ? password : "", hash);
    if (!calc)
        return PAM_AUTHINFO_UNAVAIL;
    return strcmp(calc, hash) == 0 ? PAM_SUCCESS : PAM_AUTH_ERR;
}

static char* search_key(const char* file_name, const char* key) {
    return pam_modutil_search_key(NULL, file_name, key);
}

static long search_long_key(const char* file_name, const char* key, long fallback) {
    char* value = search_key(file_name, key);
    char* end = NULL;
    long ret;

    if (!value)
        return fallback;
    errno = 0;
    ret = strtol(value, &end, 10);
    if (errno || end == value || *end != '\0')
        ret = fallback;
    free(value);
    return ret;
}

static const char* password_hash_prefix(void) {
    static char prefix[8];
    char* method = search_key("/etc/login.defs", "ENCRYPT_METHOD");

    if (method) {
        if (strcasecmp(method, "YESCRYPT") == 0)
            strcpy(prefix, "$y$");
        else if (strcasecmp(method, "SHA512") == 0)
            strcpy(prefix, "$6$");
        else if (strcasecmp(method, "SHA256") == 0)
            strcpy(prefix, "$5$");
        else if (strcasecmp(method, "MD5") == 0)
            strcpy(prefix, "$1$");
        else if (strcasecmp(method, "BCRYPT") == 0)
            strcpy(prefix, "$2b$");
        free(method);
    }
    if (!prefix[0])
        strcpy(prefix, "$y$");
    return prefix;
}

static unsigned long password_hash_rounds(const char* prefix) {
    long rounds = 0;

    if (strcmp(prefix, "$y$") == 0) {
        rounds = search_long_key("/etc/login.defs", "YESCRYPT_COST_FACTOR", 5);
        if (rounds < 3)
            rounds = 3;
        if (rounds > 11)
            rounds = 11;
    } else if (strcmp(prefix, "$5$") == 0 || strcmp(prefix, "$6$") == 0) {
        rounds = search_long_key("/etc/login.defs", "SHA_CRYPT_MAX_ROUNDS", 0);
        if (rounds < 1000 || rounds >= INT_MAX)
            rounds = 0;
        else if (rounds > 9999999)
            rounds = 9999999;
    } else if (strcmp(prefix, "$2b$") == 0) {
        rounds = search_long_key("/etc/login.defs", "BCRYPT_MIN_ROUNDS", 5);
        if (rounds < 4)
            rounds = 4;
        if (rounds > 31)
            rounds = 31;
    }
    return (unsigned long)rounds;
}

static char* hash_password(const char* password) {
    const char* prefix = password_hash_prefix();
    unsigned long rounds = password_hash_rounds(prefix);
    char salt[128];
    char* hash;

    static const char b64[] = "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    unsigned char rnd[16];
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0 || read(fd, rnd, sizeof(rnd)) != (ssize_t)sizeof(rnd)) {
        if (fd >= 0)
            close(fd);
        return NULL;
    }
    close(fd);

    int n;
    if (strcmp(prefix, "$6$") == 0 || strcmp(prefix, "$5$") == 0) {
        char b64salt[17];
        for (int i = 0; i < 16; i++)
            b64salt[i] = b64[rnd[i] & 63];
        b64salt[16] = '\0';
        if (rounds)
            n = snprintf(salt, sizeof(salt), "%srounds=%lu$%s$", prefix, rounds, b64salt);
        else
            n = snprintf(salt, sizeof(salt), "%s%s$", prefix, b64salt);
    } else if (strcmp(prefix, "$2b$") == 0) {
        char b64salt[23];
        for (int i = 0; i < 22; i++)
            b64salt[i] = b64[rnd[i % 16] & 63];
        b64salt[22] = '\0';
        unsigned long cost = rounds ? rounds : 12;
        n = snprintf(salt, sizeof(salt), "$2b$%02lu$%s", cost, b64salt);
    } else {
        char b64salt[17];
        for (int i = 0; i < 16; i++)
            b64salt[i] = b64[rnd[i] & 63];
        b64salt[16] = '\0';
        n = snprintf(salt, sizeof(salt), "%s%s$", prefix, b64salt);
    }
    if (n <= 0 || (size_t)n >= sizeof(salt))
        return NULL;

    hash = crypt(password ? password : "", salt);
    return hash ? strdup(hash) : NULL;
}

static int lock_shadow(void) {
    for (int i = 0; i < 100; i++) {
        if (lckpwdf() == 0)
            return PAM_SUCCESS;
        usleep(1000);
    }
    return PAM_AUTHTOK_LOCK_BUSY;
}

static int update_shadow_password(pam_handle_t* pamh, const char* user, char* new_hash) {
    char tmp[] = "/etc/shadow.XXXXXX";
    struct spwd newent;
    struct spwd* ent;
    struct stat st;
    FILE *in = NULL, *out = NULL;
    int fd = -1;
    int err = 1, wrote = 0;

    if (!user || !*user || !new_hash)
        return PAM_AUTHTOK_ERR;
    if (lock_shadow() != PAM_SUCCESS)
        return PAM_AUTHTOK_LOCK_BUSY;

    fd = mkstemp(tmp);
    if (fd < 0)
        goto out;
    out = fdopen(fd, "w");
    if (!out)
        goto out;
    fd = -1;

    in = fopen("/etc/shadow", "r");
    if (!in)
        goto out;
    if (fstat(fileno(in), &st) == 0) {
        (void)fchown(fileno(out), st.st_uid, st.st_gid);
        (void)fchmod(fileno(out), st.st_mode);
    }

    while ((ent = fgetspent(in)) != NULL) {
        if (strcmp(ent->sp_namp, user) == 0) {
            long today = (long)(time(NULL) / (60 * 60 * 24));
            ent->sp_pwdp = new_hash;
            ent->sp_lstchg = today ? today : -1;
            wrote = 1;
        }
        if (putspent(ent, out) != 0)
            goto out;
    }
    if (!wrote) {
        memset(&newent, 0, sizeof(newent));
        newent.sp_namp = (char*)user;
        newent.sp_pwdp = new_hash;
        newent.sp_lstchg = (long)(time(NULL) / (60 * 60 * 24));
        if (newent.sp_lstchg == 0)
            newent.sp_lstchg = -1;
        newent.sp_min = newent.sp_max = newent.sp_warn = -1;
        newent.sp_inact = newent.sp_expire = -1;
        newent.sp_flag = (unsigned long)-1;
        if (putspent(&newent, out) != 0)
            goto out;
    }
    if (fflush(out) != 0 || fsync(fileno(out)) != 0 || fclose(out) != 0) {
        out = NULL;
        goto out;
    }
    out = NULL;
    if (rename(tmp, "/etc/shadow") != 0)
        goto out;
    pam_syslog(pamh, LOG_NOTICE, "password changed for %s", user);
    err = 0;

out:
    if (in)
        fclose(in);
    if (out)
        fclose(out);
    if (fd >= 0)
        close(fd);
    if (err)
        unlink(tmp);
    ulckpwdf();
    return err ? PAM_AUTHTOK_ERR : PAM_SUCCESS;
}

int pam_chauthtok(pam_handle_t* pamh, int flags) {
    const char* user = NULL;
    const char* oldpass = NULL;
    const char* newpass = NULL;
    const void* item = NULL;
    struct spwd* sp;
    char* hash = NULL;
    int ret;

    if (!pamh)
        return PAM_SYSTEM_ERR;
    ret = pam_get_user(pamh, &user, NULL);
    if (ret != PAM_SUCCESS)
        return ret;
    if (!user || !*user)
        return PAM_USER_UNKNOWN;

    sp = getspnam(user);
    if (getuid() != 0) {
        if (!sp)
            return PAM_USER_UNKNOWN;
        ret = pam_get_authtok(pamh, PAM_OLDAUTHTOK, &oldpass, "Current password: ");
        if (ret != PAM_SUCCESS)
            return ret;
        ret = verify_password_hash(oldpass, sp->sp_pwdp);
        if (ret != PAM_SUCCESS)
            return ret;
    }

    ret = pam_get_authtok(pamh, PAM_AUTHTOK, &newpass, "New password: ");
    if (ret != PAM_SUCCESS)
        return ret;
    pam_get_item(pamh, PAM_AUTHTOK, &item);
    if (item == newpass) {
        char* again = NULL;
        ret = ask(pamh, PAM_PROMPT_ECHO_OFF, "Retype new password: ", &again);
        if (ret != PAM_SUCCESS)
            return ret;
        if (strcmp(newpass ? newpass : "", again ? again : "") != 0) {
            free(again);
            pam_set_item(pamh, PAM_AUTHTOK, NULL);
            return PAM_AUTHTOK_ERR;
        }
        free(again);
    }
    if (!newpass || !*newpass)
        return PAM_AUTHTOK_ERR;
    hash = hash_password(newpass);
    if (!hash)
        return PAM_AUTHTOK_ERR;
    ret = update_shadow_password(pamh, user, hash);
    free(hash);
    return ret;
}

int pam_setcred(pam_handle_t* pamh, int flags) {
    return pamh ? PAM_SUCCESS : PAM_SYSTEM_ERR;
}
int pam_acct_mgmt(pam_handle_t* pamh, int flags) {
    return pamh ? PAM_SUCCESS : PAM_SYSTEM_ERR;
}
int pam_open_session(pam_handle_t* pamh, int flags) {
    return pamh ? PAM_SUCCESS : PAM_SYSTEM_ERR;
}
int pam_close_session(pam_handle_t* pamh, int flags) {
    return pamh ? PAM_SUCCESS : PAM_SYSTEM_ERR;
}

const char* pam_strerror(pam_handle_t* pamh, int errnum) {
    if (errnum >= 0 && (size_t)errnum < sizeof(errors) / sizeof(errors[0]))
        return errors[errnum];
    return "Unknown PAM error";
}

int pam_fail_delay(pam_handle_t* pamh, unsigned int musec_delay) {
    if (!pamh)
        return PAM_SYSTEM_ERR;
    if (musec_delay > pamh->fail_delay)
        pamh->fail_delay = musec_delay;
    return PAM_SUCCESS;
}

static int env_name_len(const char* s) {
    const char* eq = s ? strchr(s, '=') : NULL;
    return eq ? (int)(eq - s) : -1;
}

int pam_putenv(pam_handle_t* pamh, const char* name_value) {
    int namelen;
    char* copy;
    size_t count = 0;

    if (!pamh || !name_value || !*name_value)
        return PAM_SYSTEM_ERR;
    namelen = env_name_len(name_value);
    if (namelen < 0) {
        size_t nlen = strlen(name_value);
        for (char** e = pamh->env; e && *e; e++, count++) {
            if (strncmp(*e, name_value, nlen) == 0 && (*e)[nlen] == '=') {
                free(*e);
                size_t tail = 0;
                for (char** t = e + 1; *t; t++)
                    tail++;
                memmove(e, e + 1, (tail + 1) * sizeof(char*));
                return PAM_SUCCESS;
            }
        }
        return PAM_BAD_ITEM;
    }
    if (namelen == 0)
        return PAM_SYSTEM_ERR;
    for (char** e = pamh->env; e && *e; e++, count++) {
        if (strncmp(*e, name_value, (size_t)namelen) == 0 && (*e)[namelen] == '=') {
            copy = strdup(name_value);
            if (!copy)
                return PAM_BUF_ERR;
            free(*e);
            *e = copy;
            return PAM_SUCCESS;
        }
    }
    char** newenv = realloc(pamh->env, (count + 2) * sizeof(char*));
    if (!newenv)
        return PAM_BUF_ERR;
    pamh->env = newenv;
    pamh->env[count] = strdup(name_value);
    if (!pamh->env[count])
        return PAM_BUF_ERR;
    pamh->env[count + 1] = NULL;
    return PAM_SUCCESS;
}

const char* pam_getenv(pam_handle_t* pamh, const char* name) {
    size_t len;
    if (!pamh || !name)
        return NULL;
    len = strlen(name);
    for (char** e = pamh->env; e && *e; e++)
        if (strncmp(*e, name, len) == 0 && (*e)[len] == '=')
            return *e + len + 1;
    return NULL;
}

char** pam_getenvlist(pam_handle_t* pamh) {
    size_t count = 0;
    char** out;
    if (!pamh)
        return NULL;
    while (pamh->env && pamh->env[count])
        count++;
    out = calloc(count + 1, sizeof(char*));
    if (!out)
        return NULL;
    for (size_t i = 0; i < count; i++) {
        out[i] = strdup(pamh->env[i]);
        if (!out[i]) {
            for (size_t j = 0; j < i; j++)
                free(out[j]);
            free(out);
            return NULL;
        }
    }
    return out;
}

int pam_set_data(pam_handle_t* pamh,
                 const char* module_data_name,
                 void* data,
                 void (*cleanup)(pam_handle_t* pamh, void* data, int error_status)) {
    struct data_item* d;
    if (!pamh || !module_data_name)
        return PAM_SYSTEM_ERR;
    for (d = pamh->data; d; d = d->next) {
        if (strcmp(d->name, module_data_name) == 0) {
            if (d->cleanup)
                d->cleanup(pamh, d->data, PAM_DATA_REPLACE);
            d->data = data;
            d->cleanup = cleanup;
            return PAM_SUCCESS;
        }
    }
    d = calloc(1, sizeof(*d));
    if (!d)
        return PAM_BUF_ERR;
    d->name = strdup(module_data_name);
    if (!d->name) {
        free(d);
        return PAM_BUF_ERR;
    }
    d->data = data;
    d->cleanup = cleanup;
    d->next = pamh->data;
    pamh->data = d;
    return PAM_SUCCESS;
}

int pam_get_data(const pam_handle_t* pamh, const char* module_data_name, const void** data) {
    if (!pamh || !module_data_name || !data)
        return PAM_SYSTEM_ERR;
    for (struct data_item* d = pamh->data; d; d = d->next) {
        if (strcmp(d->name, module_data_name) == 0) {
            *data = d->data;
            return PAM_SUCCESS;
        }
    }
    return PAM_NO_MODULE_DATA;
}

int pam_vprompt(pam_handle_t* pamh, int style, char** response, const char* fmt, va_list args) {
    char* prompt = NULL;
    char* answer = NULL;
    int ret;

    if (!fmt || vasprintf(&prompt, fmt, args) < 0)
        return PAM_BUF_ERR;
    ret = ask(pamh, style, prompt, &answer);
    free(prompt);
    if (response)
        *response = answer;
    else
        free(answer);
    return ret;
}

int pam_prompt(pam_handle_t* pamh, int style, char** response, const char* fmt, ...) {
    va_list args;
    int ret;
    va_start(args, fmt);
    ret = pam_vprompt(pamh, style, response, fmt, args);
    va_end(args);
    return ret;
}

void pam_vsyslog(const pam_handle_t* pamh, int priority, const char* fmt, va_list args) {
    vsyslog(priority, fmt, args);
}

void pam_syslog(const pam_handle_t* pamh, int priority, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    pam_vsyslog(pamh, priority, fmt, args);
    va_end(args);
}

struct passwd* pam_modutil_getpwnam(pam_handle_t* pamh, const char* user) {
    return getpwnam(user);
}
struct passwd* pam_modutil_getpwuid(pam_handle_t* pamh, uid_t uid) {
    return getpwuid(uid);
}
struct group* pam_modutil_getgrnam(pam_handle_t* pamh, const char* group) {
    return getgrnam(group);
}
struct group* pam_modutil_getgrgid(pam_handle_t* pamh, gid_t gid) {
    return getgrgid(gid);
}
struct spwd* pam_modutil_getspnam(pam_handle_t* pamh, const char* user) {
    return getspnam(user);
}
const char* pam_modutil_getlogin(pam_handle_t* pamh) {
    return getlogin();
}
int pam_modutil_read(int fd, char* buffer, int count) {
    int block, offset = 0;
    while (count > 0) {
        block = (int)read(fd, &buffer[offset], (size_t)count);
        if (block < 0) {
            if (errno == EINTR)
                continue;
            return block;
        }
        if (block == 0)
            return offset;
        offset += block;
        count -= block;
    }
    return offset;
}

int pam_modutil_write(int fd, const char* buffer, int count) {
    int block, offset = 0;
    while (count > 0) {
        block = (int)write(fd, &buffer[offset], (size_t)count);
        if (block < 0) {
            if (errno == EINTR)
                continue;
            return block;
        }
        if (block == 0)
            return offset;
        offset += block;
        count -= block;
    }
    return offset;
}
int pam_modutil_audit_write(pam_handle_t* pamh, int type, const char* message, int retval) {
    return PAM_SUCCESS;
}
int pam_modutil_drop_priv(pam_handle_t* pamh, struct pam_modutil_privs* p, const struct passwd* pw) {
    if (p)
        p->is_dropped = 1;
    return PAM_SUCCESS;
}
int pam_modutil_regain_priv(pam_handle_t* pamh, struct pam_modutil_privs* p) {
    if (p)
        p->is_dropped = 0;
    return PAM_SUCCESS;
}
int pam_modutil_sanitize_helper_fds(pam_handle_t* pamh,
                                    enum pam_modutil_redirect_fd a,
                                    enum pam_modutil_redirect_fd b,
                                    enum pam_modutil_redirect_fd c) {
    return PAM_SUCCESS;
}

int pam_modutil_user_in_group_nam_nam(pam_handle_t* pamh, const char* user, const char* group) {
    struct group* gr = getgrnam(group);
    struct passwd* pw;
    if (!gr)
        return 0;
    pw = getpwnam(user);
    if (pw && pw->pw_gid == gr->gr_gid)
        return 1;
    for (char** m = gr->gr_mem; m && *m; m++)
        if (strcmp(*m, user) == 0)
            return 1;
    return 0;
}

int pam_modutil_user_in_group_nam_gid(pam_handle_t* pamh, const char* user, gid_t group) {
    struct group* gr = getgrgid(group);
    return gr ? pam_modutil_user_in_group_nam_nam(pamh, user, gr->gr_name) : 0;
}

int pam_modutil_user_in_group_uid_nam(pam_handle_t* pamh, uid_t user, const char* group) {
    struct passwd* pw = getpwuid(user);
    return pw ? pam_modutil_user_in_group_nam_nam(pamh, pw->pw_name, group) : 0;
}

int pam_modutil_user_in_group_uid_gid(pam_handle_t* pamh, uid_t user, gid_t group) {
    struct passwd* pw = getpwuid(user);
    return pw ? pam_modutil_user_in_group_nam_gid(pamh, pw->pw_name, group) : 0;
}

int pam_modutil_check_user_in_passwd(pam_handle_t* pamh, const char* user_name, const char* file_name) {
    FILE* f;
    char* line = NULL;
    size_t cap = 0;
    size_t ulen;
    int found = 0;

    if (!user_name)
        return PAM_SERVICE_ERR;
    if (strchr(user_name, ':'))
        return PAM_PERM_DENIED;
    if (!file_name)
        file_name = "/etc/passwd";
    f = fopen(file_name, "r");
    if (!f)
        return PAM_SERVICE_ERR;
    ulen = strlen(user_name);
    while (getline(&line, &cap, f) >= 0) {
        char* colon = strchr(line, ':');
        if (colon && (size_t)(colon - line) == ulen && strncmp(line, user_name, ulen) == 0)
            found = 1;
    }
    free(line);
    fclose(f);
    return found ? PAM_SUCCESS : PAM_PERM_DENIED;
}

char* pam_modutil_search_key(pam_handle_t* pamh, const char* file_name, const char* key) {
    FILE* f;
    char* line = NULL;
    size_t cap = 0;
    char* ret = NULL;

    if (!file_name || !key || !(f = fopen(file_name, "r")))
        return NULL;
    while (getline(&line, &cap, f) >= 0) {
        char* p = line;
        char *comment, *tok, *end;

        p[strcspn(p, "\r\n")] = '\0';

        while (*p == ' ' || *p == '\t')
            p++;

        comment = strchr(p, '#');
        if (comment)
            *comment = '\0';

        if (!*p)
            continue;

        tok = p;
        end = tok;
        while (*end && *end != ' ' && *end != '\t' && *end != '=')
            end++;
        if (*end) {
            *end = '\0';
            p = end + 1;
        } else {
            p = end;
        }
        if (strcasecmp(tok, key) != 0)
            continue;

        while (*p == ' ' || *p == '\t' || *p == '=')
            p++;

        end = p + strlen(p);
        while (end > p && (end[-1] == ' ' || end[-1] == '\t'))
            end--;
        *end = '\0';
        ret = strdup(p);
        break;
    }
    free(line);
    fclose(f);
    return ret;
}
