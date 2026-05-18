/* groupmod.c - modify a group
 *
 * Copyright 2026 xlzs0 <xlzs0.raptor@gmail.com>
 *
 * See https://refspecs.linuxfoundation.org/LSB_4.1.0/LSB-Core-generic/LSB-Core-generic/groupmod.html

USE_GROUPMOD(NEWTOY(groupmod, "<1>1g#<0n:o", TOYFLAG_NEEDROOT|TOYFLAG_SBIN))

config GROUPMOD
  bool "groupmod"
  default n
  help
    usage: groupmod [-g gid [-o]] [-n group_name] group

    Modify a group.

    -g	New group ID (with -o, allow non-unique)
    -n	New group name
    -o	Allow non-unique GID (use with -g)
*/

#define FOR_groupmod
#include "toys.h"

GLOBALS(
  char *n;
  long g;
)

void groupmod_main(void)
{
  struct group *grp;
  char *name = *toys.optargs;

  if (!(grp = getgrnam(name))) error_exit("group '%s' not found", name);

  if (FLAG(o) && !FLAG(g)) error_exit("-o requires -g");

  if (FLAG(n)) {
    if (TT.n[strcspn(TT.n, ":/\n")] || strlen(TT.n) > 256)
      error_exit("bad group name '%s'", TT.n);
    if (getgrnam(TT.n)) error_exit("group '%s' already exists", TT.n);
  }

  if (FLAG(g)) {
    if (!FLAG(o) && getgrgid((gid_t)TT.g))
      error_exit("gid '%ld' in use", TT.g);
  }

  {
    char *members = NULL;
    int i;

    for (i = 0; grp->gr_mem && grp->gr_mem[i]; i++) {
      if (members) {
        char *tmp = xmprintf("%s,%s", members, grp->gr_mem[i]);
        free(members);
        members = tmp;
      } else members = xstrdup(grp->gr_mem[i]);
    }

    char *entry = xmprintf("%s:x:%ld:%s",
      FLAG(n) ? TT.n : grp->gr_name,
      FLAG(g) ? TT.g : (long)grp->gr_gid,
      members ? members : "");
    update_password("/etc/group", name, entry, 0);
    free(entry);

    entry = xmprintf("%s:!::%s",
      FLAG(n) ? TT.n : grp->gr_name,
      members ? members : "");
    update_password("/etc/gshadow", name, entry, 0);
    free(entry);
    free(members);
  }
}
