PREFIX ?= /usr
LIBDIR ?= $(PREFIX)/lib
SBINDIR ?= $(PREFIX)/bin
INCLUDEDIR ?= $(PREFIX)/include
PKGCONFIGDIR ?= $(LIBDIR)/pkgconfig

CC ?= cc
CFLAGS ?= -O2 -pipe
CPPFLAGS ?=
LDFLAGS ?=

COMMON_CFLAGS = $(CPPFLAGS) $(CFLAGS) -fPIC -Iinclude -Wall -Wextra -Wno-unused-parameter

.PHONY: all clean install

all: libpam.so libpamc.so libpam_misc.so unix_chkpwd

libpam.so.0.85.1: libpam/pam.c libpam/libpam.map
	$(CC) $(COMMON_CFLAGS) -DCHKPWD_HELPER=\"$(SBINDIR)/unix_chkpwd\" $(LDFLAGS) -shared -Wl,-soname,libpam.so.0 -Wl,--version-script=libpam/libpam.map -o $@ libpam/pam.c -lcrypt

libpam.so.0: libpam.so.0.85.1
	ln -sfn libpam.so.0.85.1 $@

libpam.so: libpam.so.0
	ln -sfn libpam.so.0 $@

libpamc.so.0.82.1: libpam/pamc.c libpam/libpamc.map
	$(CC) $(COMMON_CFLAGS) $(LDFLAGS) -shared -Wl,-soname,libpamc.so.0 -Wl,--version-script=libpam/libpamc.map -o $@ libpam/pamc.c

libpamc.so.0: libpamc.so.0.82.1
	ln -sfn libpamc.so.0.82.1 $@

libpamc.so: libpamc.so.0
	ln -sfn libpamc.so.0 $@

libpam_misc.so.0.82.1: libpam/pam_misc.c libpam/libpam_misc.map libpam.so
	$(CC) $(COMMON_CFLAGS) $(LDFLAGS) -shared -Wl,-soname,libpam_misc.so.0 -Wl,--version-script=libpam/libpam_misc.map -o $@ libpam/pam_misc.c -L. -lpam

libpam_misc.so.0: libpam_misc.so.0.82.1
	ln -sfn libpam_misc.so.0.82.1 $@

libpam_misc.so: libpam_misc.so.0
	ln -sfn libpam_misc.so.0 $@

unix_chkpwd: libpam/unix_chkpwd.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -Iinclude -Wall -Wextra -o $@ libpam/unix_chkpwd.c -lcrypt

install: all
	install -d "$(DESTDIR)$(LIBDIR)" "$(DESTDIR)$(SBINDIR)" "$(DESTDIR)$(INCLUDEDIR)/security" "$(DESTDIR)$(PKGCONFIGDIR)"
	install -m755 libpam.so.0.85.1 "$(DESTDIR)$(LIBDIR)/"
	install -m755 libpamc.so.0.82.1 "$(DESTDIR)$(LIBDIR)/"
	install -m755 libpam_misc.so.0.82.1 "$(DESTDIR)$(LIBDIR)/"
	install -m4755 unix_chkpwd "$(DESTDIR)$(SBINDIR)/"
	ln -sfn libpam.so.0.85.1 "$(DESTDIR)$(LIBDIR)/libpam.so.0"
	ln -sfn libpam.so.0 "$(DESTDIR)$(LIBDIR)/libpam.so"
	ln -sfn libpamc.so.0.82.1 "$(DESTDIR)$(LIBDIR)/libpamc.so.0"
	ln -sfn libpamc.so.0 "$(DESTDIR)$(LIBDIR)/libpamc.so"
	ln -sfn libpam_misc.so.0.82.1 "$(DESTDIR)$(LIBDIR)/libpam_misc.so.0"
	ln -sfn libpam_misc.so.0 "$(DESTDIR)$(LIBDIR)/libpam_misc.so"
	install -m644 include/security/*.h "$(DESTDIR)$(INCLUDEDIR)/security/"
	install -m644 pkgconfig/*.pc "$(DESTDIR)$(PKGCONFIGDIR)/"

clean:
	rm -f libpam.so libpam.so.0 libpam.so.0.85.1
	rm -f libpamc.so libpamc.so.0 libpamc.so.0.82.1
	rm -f libpam_misc.so libpam_misc.so.0 libpam_misc.so.0.82.1
	rm -f unix_chkpwd
