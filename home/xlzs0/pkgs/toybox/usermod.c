/* usermod.c - modify a user account
 *
 * Copyright 2026 xlzs0 <xlzs0.raptor@gmail.com>
 *
 * See https://refspecs.linuxfoundation.org/LSB_4.1.0/LSB-Core-generic/LSB-Core-generic/usermod.html

USE_USERMOD(NEWTOY(usermod, "<1>1c:d:g:G:l:mop:s:u#", TOYFLAG_NEEDROOT|TOYFLAG_SBIN))

config USERMOD
  bool "usermod"
  default n
  help
    usage: usermod [-c comment] [-d home_dir [-m]] [-g initial_group] [-G group[,...]]
    usage:         [-l login_name] [-o] [-p passwd] [-s shell] [-u uid] login

    Modify a user account.

    -c	New value of the password file comment field
    -d	New home directory (with -m, move current home directory contents)
    -g	New initial login group name or number
    -G	Supplementary groups (comma-separated, replaces current)
    -l	New login name
    -m	Move home directory contents to new location (use with -d)
    -o	Allow non-unique UID (use with -u)
    -p	Encrypted password (as returned by crypt(3))
    -s	New login shell (blank for system default)
    -u	New numerical user ID
*/

#define FOR_usermod
#include "toys.h"

GLOBALS(
  char *c, *d, *g, *G, *l, *p, *s;
  long u;
)

static char *update_group_members(char **cur_mem, char *user, int add)
{
  int i, found = 0, len = 1;
  char *out, *p;

  for (i = 0; cur_mem[i]; i++) {
    if (!strcmp(cur_mem[i], user)) found = 1;
    else len += strlen(cur_mem[i]) + 1;
  }

  if ((add && found) || (!add && !found)) return NULL;

  if (add) len += strlen(user) + 1;

  out = p = xzalloc(len);
  for (i = 0; cur_mem[i]; i++) {
    if (!strcmp(cur_mem[i], user)) continue;
    if (p != out) *p++ = ',';
    p = stpcpy(p, cur_mem[i]);
  }
  if (add) {
    if (p != out) *p++ = ',';
    strcpy(p, user);
  }

  return out;
}

void usermod_main(void)
{
  struct passwd *pwd;
  struct group *grp;
  char *login = *toys.optargs, *cur_name, *new_name, *new_home;
  uid_t new_uid;
  gid_t new_gid;

  if (!(pwd = getpwnam(login))) error_exit("user '%s' not found", login);

  cur_name = xstrdup(pwd->pw_name);
  new_name = FLAG(l) ? TT.l : pwd->pw_name;
  new_home = FLAG(d) ? TT.d : pwd->pw_dir;
  new_uid  = FLAG(u) ? (uid_t)TT.u : pwd->pw_uid;
  new_gid  = pwd->pw_gid;

  if (FLAG(l)) {
    if (!*TT.l || TT.l[strcspn(TT.l, ":/\n")] || strlen(TT.l) >= LOGIN_NAME_MAX)
      error_exit("bad login name '%s'", TT.l);
    if (getpwnam(TT.l)) error_exit("login '%s' already in use", TT.l);
  }

  if (FLAG(u)) {
    if (TT.u < 0) error_exit("invalid uid '%ld'", TT.u);
    if (!FLAG(o) && getpwuid((uid_t)TT.u))
      error_exit("uid '%ld' in use", TT.u);
  }

  if (FLAG(g)) {
    char *end;
    long gid = strtol(TT.g, &end, 10);

    if (*end || TT.g == end) {
      if (!(grp = getgrnam(TT.g))) error_exit("group '%s' not found", TT.g);
      new_gid = grp->gr_gid;
    } else {
      if (!(grp = getgrgid((gid_t)gid))) error_exit("gid '%ld' not found", gid);
      new_gid = (gid_t)gid;
    }
  }

  if (FLAG(G) && *TT.G) {
    char *p = xstrdup(TT.G), *tok, *tmp = p;

    while ((tok = strsep(&tmp, ",")))
      if (*tok && !getgrnam(tok)) error_exit("group '%s' not found", tok);
    free(p);
  }

  {
    char *entry = xmprintf("%s:x:%ld:%ld:%s:%s:%s",
      new_name, (long)new_uid, (long)new_gid,
      FLAG(c) ? TT.c : pwd->pw_gecos,
      new_home,
      FLAG(s) ? TT.s : pwd->pw_shell);
    update_password("/etc/passwd", cur_name, entry, 0);
    free(entry);
  }

  if (FLAG(l)) update_password("/etc/shadow", cur_name, new_name, 0);

  if (FLAG(p)) update_password("/etc/shadow", new_name, TT.p, 1);

  if (FLAG(G)) {
    char **new_grps = NULL;
    int n_new = 0;

    if (*TT.G) {
      char *p = xstrdup(TT.G), *tok, *tmp = p;

      while ((tok = strsep(&tmp, ","))) if (*tok) n_new++;
      free(p);

      new_grps = xzalloc((n_new + 1) * sizeof(char *));
      p = xstrdup(TT.G);
      tmp = p;
      n_new = 0;
      while ((tok = strsep(&tmp, ",")))
        if (*tok) new_grps[n_new++] = xstrdup(tok);
      free(p);
    }

    setgrent();
    while ((grp = getgrent())) {
      int i, in_new = 0;
      char *mem_str;

      if (grp->gr_gid == pwd->pw_gid && !FLAG(g)) continue;
      if (FLAG(g) && grp->gr_gid == new_gid) continue;

      for (i = 0; i < n_new; i++)
        if (!strcmp(grp->gr_name, new_grps[i])) { in_new = 1; break; }

      int in_cur = 0;
      for (i = 0; grp->gr_mem[i]; i++)
        if (!strcmp(grp->gr_mem[i], cur_name)) { in_cur = 1; break; }

      if (in_cur == in_new) continue;

      mem_str = update_group_members(grp->gr_mem, cur_name, in_new);
      if (mem_str) {
        update_password("/etc/group", grp->gr_name, mem_str, 3);
        update_password("/etc/gshadow", grp->gr_name, mem_str, 3);
        free(mem_str);
      }
    }
    endgrent();

    for (int i = 0; i < n_new; i++) free(new_grps[i]);
    free(new_grps);
  }

  if (FLAG(l)) {
    setgrent();
    while ((grp = getgrent())) {
      int i, in_grp = 0;

      for (i = 0; grp->gr_mem[i]; i++)
        if (!strcmp(grp->gr_mem[i], cur_name)) { in_grp = 1; break; }

      if (!in_grp) continue;

      int len = strlen(new_name) + 2;
      for (i = 0; grp->gr_mem[i]; i++)
        if (strcmp(grp->gr_mem[i], cur_name)) len += strlen(grp->gr_mem[i]) + 1;
      char *mem_str = xzalloc(len), *p = mem_str;
      for (i = 0; grp->gr_mem[i]; i++) {
        char *name = !strcmp(grp->gr_mem[i], cur_name) ? new_name : grp->gr_mem[i];
        if (p != mem_str) *p++ = ',';
        p = stpcpy(p, name);
      }
      update_password("/etc/group", grp->gr_name, mem_str, 3);
      update_password("/etc/gshadow", grp->gr_name, mem_str, 3);
      free(mem_str);
    }
    endgrent();
  }

  if (FLAG(m) && FLAG(d) && strcmp(pwd->pw_dir, TT.d)) {
    char *ug;

    if (!access(TT.d, F_OK))
      error_exit("home dir '%s' already exists", TT.d);
    if (xrun((char *[]){"mv", pwd->pw_dir, TT.d, NULL}))
      error_exit("failed to move home directory");
    ug = xmprintf("%ld:%ld", (long)new_uid, (long)new_gid);
    if (xrun((char *[]){"chown", "-R", ug, TT.d, NULL}))
      perror_msg("failed to chown new home directory");
    free(ug);
  }

  if (FLAG(u) && !access(new_home, F_OK)) {
    char *old_uid = xmprintf("%ld", (long)pwd->pw_uid);
    char *new_uid_s = xmprintf("%ld", (long)new_uid);

    if (xrun((char *[]){"find", new_home, "-user", old_uid, "-exec",
        "chown", new_uid_s, "{}", ";", NULL}))
      perror_msg("failed to update file ownership");
    free(old_uid);
    free(new_uid_s);
  }

  free(cur_name);
}
