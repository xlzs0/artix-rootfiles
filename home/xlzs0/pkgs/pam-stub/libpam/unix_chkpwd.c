#define _GNU_SOURCE
#include <crypt.h>
#include <errno.h>
#include <pwd.h>
#include <security/_pam_types.h>
#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

static int locked_hash(const char* hash) {
    return !hash || !*hash || hash[0] == '!' || hash[0] == '*';
}

static int read_password(char* buf, size_t size) {
    size_t off = 0;

    if (!buf || size == 0)
        return -1;
    while (off + 1 < size) {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n == 0)
            break;
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (c == '\0')
            break;
        buf[off++] = c;
    }
    buf[off] = '\0';
    return 0;
}

static int caller_may_check_user(const char* user) {
    uid_t ruid = getuid();
    struct passwd* pwd;

    if (ruid == 0)
        return 1;
    pwd = getpwuid(ruid);
    return pwd && strcmp(pwd->pw_name, user) == 0;
}

int main(int argc, char** argv) {
    char pass[PAM_MAX_RESP_SIZE + 1];
    const char* user;
    const char* option;
    struct spwd* sp;
    char* calc;
    int nullok;

    openlog("unix_chkpwd", LOG_PID, LOG_AUTHPRIV);

    if (isatty(STDIN_FILENO) || argc != 3) {
        syslog(LOG_NOTICE, "inappropriate use of Unix helper binary [UID=%d]", (int)getuid());
        return PAM_SYSTEM_ERR;
    }

    user = argv[1];
    option = argv[2];
    if (strcmp(option, "nullok") == 0)
        nullok = 1;
    else if (strcmp(option, "nonull") == 0)
        nullok = 0;
    else
        return PAM_SYSTEM_ERR;

    if (!caller_may_check_user(user))
        return PAM_AUTH_ERR;
    if (read_password(pass, sizeof(pass)) != 0)
        pass[0] = '\0';
    if (!nullok && pass[0] == '\0')
        return PAM_AUTH_ERR;

    errno = 0;
    sp = getspnam(user);
    if (!sp)
        return errno == EACCES ? PAM_AUTHINFO_UNAVAIL : PAM_USER_UNKNOWN;
    if (locked_hash(sp->sp_pwdp))
        return PAM_AUTH_ERR;

    calc = crypt(pass, sp->sp_pwdp);
    memset(pass, 0, sizeof(pass));
    if (!calc)
        return PAM_AUTHINFO_UNAVAIL;
    if (strcmp(calc, sp->sp_pwdp) != 0) {
        syslog(LOG_NOTICE, "password check failed for user (%s)", user);
        return PAM_AUTH_ERR;
    }
    return PAM_SUCCESS;
}
