#include <security/pam_client.h>

struct pamc_handle_s {
    int dummy;
};

pamc_handle_t pamc_start(void) {
    return calloc(1, sizeof(struct pamc_handle_s));
}

int pamc_end(pamc_handle_t* pch) {
    if (pch) {
        free(*pch);
        *pch = NULL;
    }
    return 0;
}

int pamc_load(pamc_handle_t pch, const char* agent_id) {
    return pch ? 0 : -1;
}

int pamc_converse(pamc_handle_t pch, pamc_bp_t* prompt_p) {
    return (pch && prompt_p) ? 0 : -1;
}

int pamc_disable(pamc_handle_t pch, const char* agent_id) {
    return pch ? 0 : -1;
}

char** pamc_list_agents(pamc_handle_t pch) {
    return pch ? calloc(1, sizeof(char*)) : NULL;
}
