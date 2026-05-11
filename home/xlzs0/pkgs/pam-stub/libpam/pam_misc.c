#define _GNU_SOURCE
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

time_t pam_misc_conv_warn_time;
time_t pam_misc_conv_die_time;
const char* pam_misc_conv_warn_line;
const char* pam_misc_conv_die_line;
int pam_misc_conv_died;
int (*pam_binary_handler_fn)(void* appdata, pamc_bp_t* prompt_p);
void (*pam_binary_handler_free)(void* appdata, pamc_bp_t* prompt_p);

static char* read_response(const char* prompt, int echo) {
    struct termios oldt, newt;
    char* line = NULL;
    size_t cap = 0;
    int have_term = 0;

    if (prompt)
        fputs(prompt, stderr);
    if (!echo && isatty(STDIN_FILENO) && tcgetattr(STDIN_FILENO, &oldt) == 0) {
        newt = oldt;
        newt.c_lflag &= (tcflag_t)~ECHO;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
        have_term = 1;
    }
    if (getline(&line, &cap, stdin) < 0) {
        free(line);
        line = NULL;
    }
    if (have_term) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
        fputc('\n', stderr);
    }
    if (line)
        line[strcspn(line, "\n")] = '\0';
    return line;
}

int misc_conv(int num_msg, const struct pam_message** msgm, struct pam_response** response, void* appdata_ptr) {
    struct pam_response* resp;

    if (num_msg <= 0 || !msgm || !response)
        return PAM_CONV_ERR;
    resp = calloc((size_t)num_msg, sizeof(*resp));
    if (!resp)
        return PAM_BUF_ERR;
    for (int i = 0; i < num_msg; i++) {
        const struct pam_message* msg = msgm[i];
        if (!msg)
            goto fail;
        switch (msg->msg_style) {
        case PAM_PROMPT_ECHO_OFF:
            resp[i].resp = read_response(msg->msg, 0);
            break;
        case PAM_PROMPT_ECHO_ON:
            resp[i].resp = read_response(msg->msg, 1);
            break;
        case PAM_ERROR_MSG:
        case PAM_TEXT_INFO:
            if (msg->msg)
                fprintf(stderr, "%s\n", msg->msg);
            break;
        default:
            goto fail;
        }
    }
    *response = resp;
    return PAM_SUCCESS;
fail:
    for (int i = 0; i < num_msg; i++)
        free(resp[i].resp);
    free(resp);
    return PAM_CONV_ERR;
}

int pam_misc_paste_env(pam_handle_t* pamh, const char* const* user_env) {
    int ret = PAM_SUCCESS;
    if (!user_env)
        return PAM_SUCCESS;
    for (; *user_env && ret == PAM_SUCCESS; user_env++)
        ret = pam_putenv(pamh, *user_env);
    return ret;
}

char** pam_misc_drop_env(char** env) {
    if (env) {
        for (char** p = env; *p; p++)
            free(*p);
        free(env);
    }
    return NULL;
}

int pam_misc_setenv(pam_handle_t* pamh, const char* name, const char* value, int readonly) {
    char* buf;
    int ret;

    if (!name || !value)
        return PAM_SYSTEM_ERR;
    if (asprintf(&buf, "%s=%s", name, value) < 0)
        return PAM_BUF_ERR;
    ret = pam_putenv(pamh, buf);
    free(buf);
    return ret;
}
