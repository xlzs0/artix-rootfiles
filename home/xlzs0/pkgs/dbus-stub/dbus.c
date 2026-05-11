#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint32_t dbus_bool_t;
typedef uint32_t dbus_uint32_t;
typedef int32_t dbus_int32_t;
typedef uint32_t dbus_unichar_t;
typedef unsigned long dbus_uint64_t;

typedef struct DBusConnection DBusConnection;
typedef struct DBusPendingCall DBusPendingCall;

typedef struct DBusMessage {
    int refcount;
    int type;
    dbus_uint32_t serial;
    dbus_uint32_t reply_serial;
    char* path;
    char* interface;
    char* member;
    char* error_name;
    char* destination;
    char* sender;
    char* signature;
    dbus_bool_t no_reply;
    dbus_bool_t auto_start;
} DBusMessage;
typedef struct DBusServer DBusServer;
typedef struct DBusWatch DBusWatch;
typedef struct DBusTimeout DBusTimeout;
typedef struct DBusPreallocatedSend DBusPreallocatedSend;
typedef struct DBusAddressEntry DBusAddressEntry;
typedef struct DBusSignatureIter DBusSignatureIter;

typedef struct DBusError {
    const char* name;
    const char* message;
    unsigned int dummy1 : 1;
    unsigned int dummy2 : 1;
    unsigned int dummy3 : 1;
    unsigned int dummy4 : 1;
    unsigned int dummy5 : 1;
    void* dummy6;
} DBusError;

typedef struct DBusMessageIter {
    void* dummy1;
    void* dummy2;
    dbus_uint32_t dummy3;
    int dummy4;
    int dummy5;
    int dummy6;
    int dummy7;
    int dummy8;
    int dummy9;
    int dummy10;
    int dummy11;
    int dummy12;
    void* dummy13;
    void* dummy14;
} DBusMessageIter;

void* dbus_malloc(size_t n) {
    return malloc(n);
}
void* dbus_malloc0(size_t n) {
    return calloc(1, n);
}
void* dbus_realloc(void* p, size_t n) {
    return realloc(p, n);
}
void dbus_free(void* p) {
    free(p);
}

void dbus_free_string_array(char** a) {
    if (!a)
        return;
    for (char** p = a; *p; p++)
        free(*p);
    free(a);
}

void dbus_error_init(DBusError* e) {
    if (e)
        memset(e, 0, sizeof(DBusError));
}

void dbus_error_free(DBusError* e) {
    (void)e;
}

void dbus_set_error(DBusError* e, const char* n, const char* fmt, ...) {
    (void)e;
    (void)n;
    (void)fmt;
}

void dbus_set_error_const(DBusError* e, const char* n, const char* m) {
    (void)e;
    (void)n;
    (void)m;
}

void dbus_move_error(DBusError* src, DBusError* dst) {
    (void)src;
    (void)dst;
}

dbus_bool_t dbus_error_has_name(const DBusError* e, const char* n) {
    (void)e;
    (void)n;
    return 0;
}

dbus_bool_t dbus_error_is_set(const DBusError* e) {
    (void)e;
    return 0;
}

DBusConnection* dbus_connection_open(const char* a, DBusError* e) {
    (void)a;
    (void)e;
    return NULL;
}

DBusConnection* dbus_connection_open_private(const char* a, DBusError* e) {
    (void)a;
    (void)e;
    return NULL;
}

DBusConnection* dbus_connection_ref(DBusConnection* c) {
    (void)c;
    return NULL;
}

void dbus_connection_unref(DBusConnection* c) {
    (void)c;
}

void dbus_connection_close(DBusConnection* c) {
    (void)c;
}

dbus_bool_t dbus_connection_get_is_connected(DBusConnection* c) {
    (void)c;
    return 0;
}

dbus_bool_t dbus_connection_get_is_authenticated(DBusConnection* c) {
    (void)c;
    return 0;
}

dbus_bool_t dbus_connection_get_is_anonymous(DBusConnection* c) {
    (void)c;
    return 0;
}

char* dbus_connection_get_server_id(DBusConnection* c) {
    (void)c;
    return NULL;
}

dbus_bool_t dbus_connection_can_send_type(DBusConnection* c, int t) {
    (void)c;
    (void)t;
    return 0;
}

void dbus_connection_set_exit_on_disconnect(DBusConnection* c, dbus_bool_t v) {
    (void)c;
    (void)v;
}

void dbus_connection_flush(DBusConnection* c) {
    (void)c;
}

dbus_bool_t dbus_connection_read_write_dispatch(DBusConnection* c, int t) {
    (void)c;
    (void)t;
    return 0;
}

dbus_bool_t dbus_connection_read_write(DBusConnection* c, int t) {
    (void)c;
    (void)t;
    return 0;
}

DBusMessage* dbus_connection_borrow_message(DBusConnection* c) {
    (void)c;
    return NULL;
}

void dbus_connection_return_message(DBusConnection* c, DBusMessage* m) {
    (void)c;
    (void)m;
}

void dbus_connection_steal_borrowed_message(DBusConnection* c, DBusMessage* m) {
    (void)c;
    (void)m;
}

DBusMessage* dbus_connection_pop_message(DBusConnection* c) {
    (void)c;
    return NULL;
}

int dbus_connection_get_dispatch_status(DBusConnection* c) {
    (void)c;
    return 0;
}

int dbus_connection_dispatch(DBusConnection* c) {
    (void)c;
    return 0;
}

dbus_bool_t dbus_connection_has_messages_to_send(DBusConnection* c) {
    (void)c;
    return 0;
}

dbus_bool_t dbus_connection_send(DBusConnection* c, DBusMessage* m, dbus_uint32_t* serial) {
    (void)c;
    (void)m;
    (void)serial;
    return 0;
}

dbus_bool_t dbus_connection_send_with_reply(DBusConnection* c, DBusMessage* m, DBusPendingCall** p, int t) {
    (void)c;
    (void)m;
    (void)p;
    (void)t;
    return 0;
}

DBusMessage* dbus_connection_send_with_reply_and_block(DBusConnection* c, DBusMessage* m, int t, DBusError* e) {
    (void)c;
    (void)m;
    (void)t;
    (void)e;
    return NULL;
}

dbus_bool_t dbus_connection_set_watch_functions(DBusConnection* c, void* a, void* b, void* t, void* f, void* d) {
    (void)c;
    (void)a;
    (void)b;
    (void)t;
    (void)f;
    (void)d;
    return 0;
}

dbus_bool_t dbus_connection_set_timeout_functions(DBusConnection* c, void* a, void* b, void* t, void* f, void* d) {
    (void)c;
    (void)a;
    (void)b;
    (void)t;
    (void)f;
    (void)d;
    return 0;
}

void dbus_connection_set_wakeup_main_function(DBusConnection* c, void* f, void* d, void* fr) {
    (void)c;
    (void)f;
    (void)d;
    (void)fr;
}

void dbus_connection_set_dispatch_status_function(DBusConnection* c, void* f, void* d, void* fr) {
    (void)c;
    (void)f;
    (void)d;
    (void)fr;
}

dbus_bool_t dbus_connection_get_unix_user(DBusConnection* c, unsigned long* uid) {
    (void)c;
    (void)uid;
    return 0;
}

dbus_bool_t dbus_connection_get_unix_process_id(DBusConnection* c, unsigned long* pid) {
    (void)c;
    (void)pid;
    return 0;
}

dbus_bool_t dbus_connection_get_adt_audit_session_data(DBusConnection* c, void** data, dbus_int32_t* len) {
    (void)c;
    (void)data;
    (void)len;
    return 0;
}

void dbus_connection_set_unix_user_function(DBusConnection* c, void* f, void* d, void* fr) {
    (void)c;
    (void)f;
    (void)d;
    (void)fr;
}

dbus_bool_t dbus_connection_get_windows_user(DBusConnection* c, char** sid) {
    (void)c;
    (void)sid;
    return 0;
}

void dbus_connection_set_windows_user_function(DBusConnection* c, void* f, void* d, void* fr) {
    (void)c;
    (void)f;
    (void)d;
    (void)fr;
}

void dbus_connection_set_allow_anonymous(DBusConnection* c, dbus_bool_t v) {
    (void)c;
    (void)v;
}

void dbus_connection_set_builtin_filters_enabled(DBusConnection* c, dbus_bool_t v) {
    (void)c;
    (void)v;
}

void dbus_connection_set_route_peer_messages(DBusConnection* c, dbus_bool_t v) {
    (void)c;
    (void)v;
}

dbus_bool_t dbus_connection_add_filter(DBusConnection* c, void* f, void* d, void* fr) {
    (void)c;
    (void)f;
    (void)d;
    (void)fr;
    return 0;
}

void dbus_connection_remove_filter(DBusConnection* c, void* f, void* d) {
    (void)c;
    (void)f;
    (void)d;
}

dbus_bool_t dbus_connection_allocate_data_slot(dbus_int32_t* slot) {
    (void)slot;
    return 0;
}

void dbus_connection_free_data_slot(dbus_int32_t* slot) {
    (void)slot;
}

dbus_bool_t dbus_connection_set_data(DBusConnection* c, dbus_int32_t slot, void* d, void* fr) {
    (void)c;
    (void)slot;
    (void)d;
    (void)fr;
    return 0;
}

void* dbus_connection_get_data(DBusConnection* c, dbus_int32_t slot) {
    (void)c;
    (void)slot;
    return NULL;
}

void dbus_connection_set_change_sigpipe(dbus_bool_t v) {
    (void)v;
}

void dbus_connection_set_max_message_size(DBusConnection* c, long s) {
    (void)c;
    (void)s;
}

long dbus_connection_get_max_message_size(DBusConnection* c) {
    (void)c;
    return 0;
}

void dbus_connection_set_max_received_size(DBusConnection* c, long s) {
    (void)c;
    (void)s;
}

long dbus_connection_get_max_received_size(DBusConnection* c) {
    (void)c;
    return 0;
}

void dbus_connection_set_max_message_unix_fds(DBusConnection* c, long n) {
    (void)c;
    (void)n;
}

long dbus_connection_get_max_message_unix_fds(DBusConnection* c) {
    (void)c;
    return 0;
}

void dbus_connection_set_max_received_unix_fds(DBusConnection* c, long n) {
    (void)c;
    (void)n;
}

long dbus_connection_get_max_received_unix_fds(DBusConnection* c) {
    (void)c;
    return 0;
}

long dbus_connection_get_outgoing_size(DBusConnection* c) {
    (void)c;
    return 0;
}

long dbus_connection_get_outgoing_unix_fds(DBusConnection* c) {
    (void)c;
    return 0;
}

DBusPreallocatedSend* dbus_connection_preallocate_send(DBusConnection* c) {
    (void)c;
    return NULL;
}

void dbus_connection_free_preallocated_send(DBusConnection* c, DBusPreallocatedSend* p) {
    (void)c;
    (void)p;
}

void dbus_connection_send_preallocated(DBusConnection* c, DBusPreallocatedSend* p, DBusMessage* m, dbus_uint32_t* s) {
    (void)c;
    (void)p;
    (void)m;
    (void)s;
}

dbus_bool_t dbus_connection_try_register_object_path(DBusConnection* c, const char* path, const void* vtable, void* d, DBusError* e) {
    (void)c;
    (void)path;
    (void)vtable;
    (void)d;
    (void)e;
    return 0;
}

dbus_bool_t dbus_connection_register_object_path(DBusConnection* c, const char* path, const void* vtable, void* d) {
    (void)c;
    (void)path;
    (void)vtable;
    (void)d;
    return 0;
}

dbus_bool_t dbus_connection_try_register_fallback(DBusConnection* c, const char* path, const void* vtable, void* d, DBusError* e) {
    (void)c;
    (void)path;
    (void)vtable;
    (void)d;
    (void)e;
    return 0;
}

dbus_bool_t dbus_connection_register_fallback(DBusConnection* c, const char* path, const void* vtable, void* d) {
    (void)c;
    (void)path;
    (void)vtable;
    (void)d;
    return 0;
}

dbus_bool_t dbus_connection_unregister_object_path(DBusConnection* c, const char* path) {
    (void)c;
    (void)path;
    return 0;
}

dbus_bool_t dbus_connection_get_object_path_data(DBusConnection* c, const char* path, void** data) {
    (void)c;
    (void)path;
    (void)data;
    return 0;
}

dbus_bool_t dbus_connection_list_registered(DBusConnection* c, const char* path, char*** child) {
    (void)c;
    (void)path;
    (void)child;
    return 0;
}

dbus_bool_t dbus_connection_get_unix_fd(DBusConnection* c, int* fd) {
    (void)c;
    (void)fd;
    return 0;
}

dbus_bool_t dbus_connection_get_socket(DBusConnection* c, int* fd) {
    (void)c;
    (void)fd;
    return 0;
}

int dbus_watch_get_fd(DBusWatch* w) {
    (void)w;
    return -1;
}

int dbus_watch_get_unix_fd(DBusWatch* w) {
    (void)w;
    return -1;
}

int dbus_watch_get_socket(DBusWatch* w) {
    (void)w;
    return -1;
}

unsigned int dbus_watch_get_flags(DBusWatch* w) {
    (void)w;
    return 0;
}

void* dbus_watch_get_data(DBusWatch* w) {
    (void)w;
    return NULL;
}

void dbus_watch_set_data(DBusWatch* w, void* d, void* fr) {
    (void)w;
    (void)d;
    (void)fr;
}

dbus_bool_t dbus_watch_handle(DBusWatch* w, unsigned int f) {
    (void)w;
    (void)f;
    return 0;
}

dbus_bool_t dbus_watch_get_enabled(DBusWatch* w) {
    (void)w;
    return 0;
}

int dbus_timeout_get_interval(DBusTimeout* t) {
    (void)t;
    return 0;
}

void* dbus_timeout_get_data(DBusTimeout* t) {
    (void)t;
    return NULL;
}

void dbus_timeout_set_data(DBusTimeout* t, void* d, void* fr) {
    (void)t;
    (void)d;
    (void)fr;
}

dbus_bool_t dbus_timeout_handle(DBusTimeout* t) {
    (void)t;
    return 0;
}

dbus_bool_t dbus_timeout_get_enabled(DBusTimeout* t) {
    (void)t;
    return 0;
}

static char* xstrdup(const char* s) {
    return s ? strdup(s) : NULL;
}

static DBusMessage* msg_alloc(int type) {
    DBusMessage* m = calloc(1, sizeof(*m));
    if (m) {
        m->refcount = 1;
        m->type = type;
    }
    return m;
}

static void msg_free(DBusMessage* m) {
    free(m->path);
    free(m->interface);
    free(m->member);
    free(m->error_name);
    free(m->destination);
    free(m->sender);
    free(m->signature);
    free(m);
}

DBusMessage* dbus_message_new(int type) {
    return msg_alloc(type);
}

DBusMessage* dbus_message_new_method_call(const char* dest, const char* path, const char* iface, const char* method) {
    DBusMessage* m = msg_alloc(1);
    if (!m)
        return NULL;
    m->destination = xstrdup(dest);
    m->path = xstrdup(path);
    m->interface = xstrdup(iface);
    m->member = xstrdup(method);
    return m;
}

DBusMessage* dbus_message_new_method_return(DBusMessage* call) {
    DBusMessage* m = msg_alloc(2);
    if (m && call)
        m->reply_serial = call->serial;
    return m;
}

DBusMessage* dbus_message_new_signal(const char* path, const char* iface, const char* name) {
    DBusMessage* m = msg_alloc(4);
    if (!m)
        return NULL;
    m->path = xstrdup(path);
    m->interface = xstrdup(iface);
    m->member = xstrdup(name);
    return m;
}

DBusMessage* dbus_message_new_error(DBusMessage* reply_to, const char* name, const char* msg) {
    DBusMessage* m = msg_alloc(3);
    if (!m)
        return NULL;
    m->error_name = xstrdup(name);
    if (reply_to)
        m->reply_serial = reply_to->serial;
    (void)msg;
    return m;
}

DBusMessage* dbus_message_new_error_printf(DBusMessage* reply_to, const char* name, const char* fmt, ...) {
    (void)fmt;
    return dbus_message_new_error(reply_to, name, NULL);
}

DBusMessage* dbus_message_copy(const DBusMessage* m) {
    if (!m)
        return NULL;
    DBusMessage* c = msg_alloc(m->type);
    if (!c)
        return NULL;
    c->serial = m->serial;
    c->reply_serial = m->reply_serial;
    c->no_reply = m->no_reply;
    c->auto_start = m->auto_start;
    c->path = xstrdup(m->path);
    c->interface = xstrdup(m->interface);
    c->member = xstrdup(m->member);
    c->error_name = xstrdup(m->error_name);
    c->destination = xstrdup(m->destination);
    c->sender = xstrdup(m->sender);
    c->signature = xstrdup(m->signature);
    return c;
}

DBusMessage* dbus_message_ref(DBusMessage* m) {
    if (m)
        m->refcount++;
    return m;
}

void dbus_message_unref(DBusMessage* m) {
    if (m && --m->refcount <= 0)
        msg_free(m);
}

int dbus_message_get_type(DBusMessage* m) {
    return m ? m->type : 0;
}

static dbus_bool_t msg_set_str(char** dst, const char* src) {
    free(*dst);
    *dst = xstrdup(src);
    return 1;
}

dbus_bool_t dbus_message_set_path(DBusMessage* m, const char* p) {
    return m ? msg_set_str(&m->path, p) : 0;
}

const char* dbus_message_get_path(DBusMessage* m) {
    return m ? m->path : NULL;
}

dbus_bool_t dbus_message_has_path(DBusMessage* m, const char* p) {
    return m && m->path && p && strcmp(m->path, p) == 0;
}

dbus_bool_t dbus_message_set_interface(DBusMessage* m, const char* p) {
    return m ? msg_set_str(&m->interface, p) : 0;
}

const char* dbus_message_get_interface(DBusMessage* m) {
    return m ? m->interface : NULL;
}

dbus_bool_t dbus_message_has_interface(DBusMessage* m, const char* i) {
    return m && m->interface && i && strcmp(m->interface, i) == 0;
}

dbus_bool_t dbus_message_set_member(DBusMessage* m, const char* p) {
    return m ? msg_set_str(&m->member, p) : 0;
}

const char* dbus_message_get_member(DBusMessage* m) {
    return m ? m->member : NULL;
}

dbus_bool_t dbus_message_has_member(DBusMessage* m, const char* mb) {
    return m && m->member && mb && strcmp(m->member, mb) == 0;
}

dbus_bool_t dbus_message_set_error_name(DBusMessage* m, const char* p) {
    return m ? msg_set_str(&m->error_name, p) : 0;
}

const char* dbus_message_get_error_name(DBusMessage* m) {
    return m ? m->error_name : NULL;
}

dbus_bool_t dbus_message_set_destination(DBusMessage* m, const char* p) {
    return m ? msg_set_str(&m->destination, p) : 0;
}

const char* dbus_message_get_destination(DBusMessage* m) {
    return m ? m->destination : NULL;
}

dbus_bool_t dbus_message_set_sender(DBusMessage* m, const char* p) {
    return m ? msg_set_str(&m->sender, p) : 0;
}

const char* dbus_message_get_sender(DBusMessage* m) {
    return m ? m->sender : NULL;
}

const char* dbus_message_get_signature(DBusMessage* m) {
    return m ? m->signature : "";
}

void dbus_message_set_no_reply(DBusMessage* m, dbus_bool_t v) {
    if (m)
        m->no_reply = v;
}

dbus_bool_t dbus_message_get_no_reply(DBusMessage* m) {
    return m ? m->no_reply : 0;
}

dbus_bool_t dbus_message_is_method_call(DBusMessage* m, const char* i, const char* mb) {
    return m && m->type == 1 && dbus_message_has_interface(m, i) && dbus_message_has_member(m, mb);
}

dbus_bool_t dbus_message_is_signal(DBusMessage* m, const char* i, const char* mb) {
    return m && m->type == 4 && dbus_message_has_interface(m, i) && dbus_message_has_member(m, mb);
}

dbus_bool_t dbus_message_is_error(DBusMessage* m, const char* n) {
    return m && m->type == 3 && m->error_name && n && strcmp(m->error_name, n) == 0;
}

dbus_bool_t dbus_message_has_destination(DBusMessage* m, const char* d) {
    return m && m->destination && d && strcmp(m->destination, d) == 0;
}

dbus_bool_t dbus_message_has_sender(DBusMessage* m, const char* s) {
    return m && m->sender && s && strcmp(m->sender, s) == 0;
}

dbus_bool_t dbus_message_has_signature(DBusMessage* m, const char* s) {
    const char* sig = dbus_message_get_signature(m);
    return s && strcmp(sig, s) == 0;
}

dbus_uint32_t dbus_message_get_serial(DBusMessage* m) {
    return m ? m->serial : 0;
}

void dbus_message_set_serial(DBusMessage* m, dbus_uint32_t s) {
    if (m)
        m->serial = s;
}

dbus_bool_t dbus_message_set_reply_serial(DBusMessage* m, dbus_uint32_t s) {
    if (!m)
        return 0;
    m->reply_serial = s;
    return 1;
}

dbus_uint32_t dbus_message_get_reply_serial(DBusMessage* m) {
    return m ? m->reply_serial : 0;
}

void dbus_message_set_auto_start(DBusMessage* m, dbus_bool_t v) {
    if (m)
        m->auto_start = v;
}

dbus_bool_t dbus_message_get_auto_start(DBusMessage* m) {
    return m ? m->auto_start : 0;
}

dbus_bool_t dbus_message_get_path_decomposed(DBusMessage* m, char*** p) {
    (void)m;
    (void)p;
    return 0;
}

const char* dbus_message_get_container_instance(DBusMessage* m) {
    (void)m;
    return NULL;
}

dbus_bool_t dbus_message_set_container_instance(DBusMessage* m, const char* p) {
    (void)m;
    (void)p;
    return 0;
}

dbus_bool_t dbus_message_append_args(DBusMessage* m, int first, ...) {
    (void)m;
    (void)first;
    return 0;
}

dbus_bool_t dbus_message_append_args_valist(DBusMessage* m, int first, va_list args) {
    (void)m;
    (void)first;
    (void)args;
    return 0;
}

dbus_bool_t dbus_message_get_args(DBusMessage* m, DBusError* e, int first, ...) {
    (void)m;
    (void)e;
    (void)first;
    return 0;
}

dbus_bool_t dbus_message_get_args_valist(DBusMessage* m, DBusError* e, int first, va_list args) {
    (void)m;
    (void)e;
    (void)first;
    (void)args;
    return 0;
}

dbus_bool_t dbus_message_contains_unix_fds(DBusMessage* m) {
    (void)m;
    return 0;
}

void dbus_message_iter_init_closed(DBusMessageIter* iter) {
    if (iter)
        memset(iter, 0, sizeof(DBusMessageIter));
}

dbus_bool_t dbus_message_iter_init(DBusMessage* m, DBusMessageIter* iter) {
    (void)m;
    if (iter)
        memset(iter, 0, sizeof(DBusMessageIter));
    return 0;
}

dbus_bool_t dbus_message_iter_has_next(void* iter) {
    (void)iter;
    return 0;
}

dbus_bool_t dbus_message_iter_next(void* iter) {
    (void)iter;
    return 0;
}

char* dbus_message_iter_get_signature(void* iter) {
    (void)iter;
    return NULL;
}

int dbus_message_iter_get_arg_type(void* iter) {
    (void)iter;
    return 0;
}

int dbus_message_iter_get_element_type(void* iter) {
    (void)iter;
    return 0;
}

void dbus_message_iter_recurse(void* iter, void* sub) {
    (void)iter;
    (void)sub;
}

void dbus_message_iter_get_basic(void* iter, void* val) {
    (void)iter;
    (void)val;
}

int dbus_message_iter_get_element_count(void* iter) {
    (void)iter;
    return 0;
}

int dbus_message_iter_get_array_len(void* iter) {
    (void)iter;
    return 0;
}

void dbus_message_iter_get_fixed_array(void* iter, void* val, int* n) {
    (void)iter;
    (void)val;
    (void)n;
}

void dbus_message_iter_init_append(DBusMessage* m, DBusMessageIter* iter) {
    (void)m;
    if (iter)
        memset(iter, 0, sizeof(DBusMessageIter));
}

dbus_bool_t dbus_message_iter_append_basic(void* iter, int type, const void* val) {
    (void)iter;
    (void)type;
    (void)val;
    return 1;
}

dbus_bool_t dbus_message_iter_append_fixed_array(void* iter, int etype, const void* val, int n) {
    (void)iter;
    (void)etype;
    (void)val;
    (void)n;
    return 1;
}

dbus_bool_t dbus_message_iter_open_container(DBusMessageIter* iter, int type, const char* sig, DBusMessageIter* sub) {
    (void)iter;
    (void)type;
    (void)sig;
    if (sub)
        memset(sub, 0, sizeof(DBusMessageIter));
    return 1;
}

dbus_bool_t dbus_message_iter_close_container(void* iter, void* sub) {
    (void)iter;
    (void)sub;
    return 1;
}

void dbus_message_iter_abandon_container(void* iter, void* sub) {
    (void)iter;
    (void)sub;
}

void dbus_message_iter_abandon_container_if_open(void* iter, void* sub) {
    (void)iter;
    (void)sub;
}

void dbus_message_lock(DBusMessage* m) {
    (void)m;
}

dbus_bool_t dbus_set_error_from_message(DBusError* e, DBusMessage* m) {
    (void)e;
    (void)m;
    return 0;
}

dbus_bool_t dbus_message_allocate_data_slot(dbus_int32_t* slot) {
    (void)slot;
    return 0;
}

void dbus_message_free_data_slot(dbus_int32_t* slot) {
    (void)slot;
}

dbus_bool_t dbus_message_set_data(DBusMessage* m, dbus_int32_t slot, void* d, void* fr) {
    (void)m;
    (void)slot;
    (void)d;
    (void)fr;
    return 0;
}

void* dbus_message_get_data(DBusMessage* m, dbus_int32_t slot) {
    (void)m;
    (void)slot;
    return NULL;
}

int dbus_message_type_from_string(const char* s) {
    (void)s;
    return 0;
}

const char* dbus_message_type_to_string(int type) {
    (void)type;
    return "invalid";
}

dbus_bool_t dbus_message_marshal(DBusMessage* m, char** data, int* len) {
    (void)m;
    (void)data;
    (void)len;
    return 0;
}

DBusMessage* dbus_message_demarshal(const char* s, int len, DBusError* e) {
    (void)s;
    (void)len;
    (void)e;
    return NULL;
}

int dbus_message_demarshal_bytes_needed(const char* s, int len) {
    (void)s;
    (void)len;
    return 0;
}

void dbus_message_set_allow_interactive_authorization(DBusMessage* m, dbus_bool_t v) {
    (void)m;
    (void)v;
}

dbus_bool_t dbus_message_get_allow_interactive_authorization(DBusMessage* m) {
    (void)m;
    return 0;
}

DBusConnection* dbus_bus_get(int type, DBusError* e) {
    (void)type;
    (void)e;
    return NULL;
}

DBusConnection* dbus_bus_get_private(int type, DBusError* e) {
    (void)type;
    (void)e;
    return NULL;
}

dbus_bool_t dbus_bus_register(DBusConnection* c, DBusError* e) {
    (void)c;
    (void)e;
    return 0;
}

dbus_bool_t dbus_bus_set_unique_name(DBusConnection* c, const char* n) {
    (void)c;
    (void)n;
    return 0;
}

const char* dbus_bus_get_unique_name(DBusConnection* c) {
    (void)c;
    return NULL;
}

unsigned long dbus_bus_get_unix_user(DBusConnection* c, const char* n, DBusError* e) {
    (void)c;
    (void)n;
    (void)e;
    return (unsigned long)-1;
}

char* dbus_bus_get_id(DBusConnection* c, DBusError* e) {
    (void)c;
    (void)e;
    return NULL;
}

int dbus_bus_request_name(DBusConnection* c, const char* n, unsigned int f, DBusError* e) {
    (void)c;
    (void)n;
    (void)f;
    (void)e;
    return -1;
}

int dbus_bus_release_name(DBusConnection* c, const char* n, DBusError* e) {
    (void)c;
    (void)n;
    (void)e;
    return -1;
}

dbus_bool_t dbus_bus_name_has_owner(DBusConnection* c, const char* n, DBusError* e) {
    (void)c;
    (void)n;
    (void)e;
    return 0;
}

dbus_bool_t dbus_bus_start_service_by_name(DBusConnection* c, const char* n, dbus_uint32_t f, dbus_uint32_t* r, DBusError* e) {
    (void)c;
    (void)n;
    (void)f;
    (void)r;
    (void)e;
    return 0;
}

void dbus_bus_add_match(DBusConnection* c, const char* rule, DBusError* e) {
    (void)c;
    (void)rule;
    (void)e;
}

void dbus_bus_remove_match(DBusConnection* c, const char* rule, DBusError* e) {
    (void)c;
    (void)rule;
    (void)e;
}

char* dbus_get_local_machine_id(void) {
    return NULL;
}

char* dbus_try_get_local_machine_id(DBusError* e) {
    (void)e;
    return NULL;
}
void dbus_get_version(int* maj, int* min, int* mic) {
    if (maj)
        *maj = 1;
    if (min)
        *min = 16;
    if (mic)
        *mic = 2;
}

dbus_bool_t dbus_setenv(const char* var, const char* val) {
    (void)var;
    (void)val;
    return 0;
}

void dbus_shutdown(void) {}

DBusPendingCall* dbus_pending_call_ref(DBusPendingCall* p) {
    (void)p;
    return NULL;
}

void dbus_pending_call_unref(DBusPendingCall* p) {
    (void)p;
}

dbus_bool_t dbus_pending_call_set_notify(DBusPendingCall* p, void* f, void* d, void* fr) {
    (void)p;
    (void)f;
    (void)d;
    (void)fr;
    return 0;
}

void dbus_pending_call_cancel(DBusPendingCall* p) {
    (void)p;
}

dbus_bool_t dbus_pending_call_get_completed(DBusPendingCall* p) {
    (void)p;
    return 0;
}

DBusMessage* dbus_pending_call_steal_reply(DBusPendingCall* p) {
    (void)p;
    return NULL;
}

void dbus_pending_call_block(DBusPendingCall* p) {
    (void)p;
}

dbus_bool_t dbus_pending_call_allocate_data_slot(dbus_int32_t* slot) {
    (void)slot;
    return 0;
}

void dbus_pending_call_free_data_slot(dbus_int32_t* slot) {
    (void)slot;
}

dbus_bool_t dbus_pending_call_set_data(DBusPendingCall* p, dbus_int32_t slot, void* d, void* fr) {
    (void)p;
    (void)slot;
    (void)d;
    (void)fr;
    return 0;
}

void* dbus_pending_call_get_data(DBusPendingCall* p, dbus_int32_t slot) {
    (void)p;
    (void)slot;
    return NULL;
}

DBusServer* dbus_server_listen(const char* addr, DBusError* e) {
    (void)addr;
    (void)e;
    return NULL;
}

DBusServer* dbus_server_ref(DBusServer* s) {
    (void)s;
    return NULL;
}

void dbus_server_unref(DBusServer* s) {
    (void)s;
}

void dbus_server_disconnect(DBusServer* s) {
    (void)s;
}

dbus_bool_t dbus_server_get_is_connected(DBusServer* s) {
    (void)s;
    return 0;
}

char* dbus_server_get_address(DBusServer* s) {
    (void)s;
    return NULL;
}

char* dbus_server_get_id(DBusServer* s) {
    (void)s;
    return NULL;
}

void dbus_server_set_new_connection_function(DBusServer* s, void* f, void* d, void* fr) {
    (void)s;
    (void)f;
    (void)d;
    (void)fr;
}

dbus_bool_t dbus_server_set_watch_functions(DBusServer* s, void* a, void* b, void* t, void* d, void* fr) {
    (void)s;
    (void)a;
    (void)b;
    (void)t;
    (void)d;
    (void)fr;
    return 0;
}

dbus_bool_t dbus_server_set_timeout_functions(DBusServer* s, void* a, void* b, void* t, void* d, void* fr) {
    (void)s;
    (void)a;
    (void)b;
    (void)t;
    (void)d;
    (void)fr;
    return 0;
}

dbus_bool_t dbus_server_set_auth_mechanisms(DBusServer* s, const char** mechs) {
    (void)s;
    (void)mechs;
    return 0;
}

dbus_bool_t dbus_server_allocate_data_slot(dbus_int32_t* slot) {
    (void)slot;
    return 0;
}

void dbus_server_free_data_slot(dbus_int32_t* slot) {
    (void)slot;
}

dbus_bool_t dbus_server_set_data(DBusServer* s, int slot, void* d, void* fr) {
    (void)s;
    (void)slot;
    (void)d;
    (void)fr;
    return 0;
}

void* dbus_server_get_data(DBusServer* s, int slot) {
    (void)s;
    (void)slot;
    return NULL;
}

void dbus_signature_iter_init(DBusSignatureIter* iter, const char* sig) {
    (void)iter;
    (void)sig;
}

int dbus_signature_iter_get_current_type(const DBusSignatureIter* iter) {
    (void)iter;
    return 0;
}

char* dbus_signature_iter_get_signature(const DBusSignatureIter* iter) {
    (void)iter;
    return NULL;
}

int dbus_signature_iter_get_element_type(const DBusSignatureIter* iter) {
    (void)iter;
    return 0;
}

dbus_bool_t dbus_signature_iter_next(DBusSignatureIter* iter) {
    (void)iter;
    return 0;
}

void dbus_signature_iter_recurse(const DBusSignatureIter* iter, DBusSignatureIter* sub) {
    (void)iter;
    (void)sub;
}

dbus_bool_t dbus_signature_validate(const char* sig, DBusError* e) {
    (void)sig;
    (void)e;
    return 0;
}

dbus_bool_t dbus_signature_validate_single(const char* sig, DBusError* e) {
    (void)sig;
    (void)e;
    return 0;
}

dbus_bool_t dbus_type_is_valid(int t) {
    (void)t;
    return 0;
}

dbus_bool_t dbus_type_is_basic(int t) {
    (void)t;
    return 0;
}

dbus_bool_t dbus_type_is_container(int t) {
    (void)t;
    return 0;
}

dbus_bool_t dbus_type_is_fixed(int t) {
    (void)t;
    return 0;
}

dbus_bool_t dbus_validate_bus_name(const char* n, DBusError* e) {
    (void)n;
    (void)e;
    return 1;
}

dbus_bool_t dbus_validate_error_name(const char* n, DBusError* e) {
    (void)n;
    (void)e;
    return 1;
}

dbus_bool_t dbus_validate_interface(const char* n, DBusError* e) {
    (void)n;
    (void)e;
    return 1;
}

dbus_bool_t dbus_validate_member(const char* n, DBusError* e) {
    (void)n;
    (void)e;
    return 1;
}

dbus_bool_t dbus_validate_path(const char* n, DBusError* e) {
    (void)n;
    (void)e;
    return 1;
}

dbus_bool_t dbus_validate_utf8(const char* s, DBusError* e) {
    (void)s;
    (void)e;
    return 1;
}

dbus_bool_t dbus_threads_init(const void* funcs) {
    (void)funcs;
    return 1;
}

dbus_bool_t dbus_threads_init_default(void) {
    return 1;
}

dbus_bool_t dbus_parse_address(const char* addr, void*** entries, int* n, DBusError* e) {
    (void)addr;
    (void)entries;
    (void)n;
    (void)e;
    return 0;
}

const char* dbus_address_entry_get_value(DBusAddressEntry* entry, const char* key) {
    (void)entry;
    (void)key;
    return NULL;
}

const char* dbus_address_entry_get_method(DBusAddressEntry* entry) {
    (void)entry;
    return NULL;
}

void dbus_address_entries_free(DBusAddressEntry** entries) {
    (void)entries;
}

char* dbus_address_escape_value(const char* val) {
    (void)val;
    return NULL;
}

char* dbus_address_unescape_value(const char* val, DBusError* e) {
    (void)val;
    (void)e;
    return NULL;
}
