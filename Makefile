CC=gcc
SED=sed
PREFIX = /usr
SYSCONFDIR = /etc
BINDIR = $(PREFIX)/bin
SRCFILES := xwl.c
XMLFILES := aura-shell.xml viewporter.xml xdg-shell-unstable-v6.xml
AUXFILES := Makefile README LICENSE AUTHORS xwl@.service.in version.h.in xwlrc
ALLFILES := $(SRCFILES) $(XMLFILES) $(AUXFILES)
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
DIST_VERSION := $(shell git describe --abbrev=0 --tags)
DIST_VERSION_BITS := $(subst ., ,$(DIST_VERSION))
DIST_VERSION_MAJOR := $(word 1,$(DIST_VERSION_BITS))
DIST_VERSION_MINOR := $(word 2,$(DIST_VERSION_BITS))
DIST_VERSION_MINOR_NEXT := $(shell expr $(DIST_VERSION_MINOR) + 1)
CFLAGS=-g -Wall `pkg-config --cflags xcb xcb-composite wayland-server wayland-client libsystemd` -I. -DXWAYLAND_PATH=\"$(PREFIX)/bin\"
LDFLAGS=-lpthread -lm `pkg-config --libs xcb xcb-composite wayland-server wayland-client libsystemd`
DEPS = xdg-shell-unstable-v6-client-protocol.h aura-shell-client-protocol.h viewporter-client-protocol.h version.h
OBJECTS = xwl.o xdg-shell-unstable-v6-protocol.o aura-shell-protocol.o viewporter-protocol.o

all: xwl-run xwl@.service

xwl@.service: xwl@.service.in
	$(SED) \
		-e 's|@bindir[@]|$(BINDIR)|g' \
		-e 's|@sysconfdir[@]|$(SYSCONFDIR)|g' \
		$< > $@

xwl-run: $(OBJECTS)
	$(CC) $(OBJECTS) -o xwl-run $(LDFLAGS)

%-protocol.c: %.xml
	wayland-scanner code < $< > $@

%-client-protocol.h: %.xml
	wayland-scanner client-header < $< > $@

version.h: version.h.in
	$(SED) -e 's|@version[@]|$(GIT_VERSION)|g' $< > $@

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJECTS): $(DEPS)

.PHONY: all install uninstall update-version dist deb version-clean clean

install: all
	install -D xwl-run \
		$(DESTDIR)$(PREFIX)/bin/xwl-run
	install -D xwlrc \
		$(DESTDIR)$(SYSCONFDIR)/xwlrc
	install -m 644 -D xwl@.service \
		$(DESTDIR)$(SYSCONFDIR)/systemd/user/xwl@.service

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xwl-run
	rm -f $(DESTDIR)$(SYSCONFDIR)/xwlrc
	rm -f $(DESTDIR)$(SYSCONFDIR)/systemd/user/xwl@.service

update-version: version-clean
	dch -v $(DIST_VERSION_MAJOR).$(DIST_VERSION_MINOR_NEXT)-1
	git commit -m 'debian/changelog: bump to version $(DIST_VERSION_MAJOR).$(DIST_VERSION_MINOR_NEXT)' debian/changelog
	git tag $(DIST_VERSION_MAJOR).$(DIST_VERSION_MINOR_NEXT)

dist: version-clean $(DEPS)
	mkdir -p xwl-$(DIST_VERSION)
	cp -r $(ALLFILES) $(DEPS) debian xwl-$(DIST_VERSION)
	tar czf xwl-$(DIST_VERSION).tar.gz xwl-$(DIST_VERSION)
	rm -rf xwl-$(DIST_VERSION)

deb: dist
	ln -s xwl-$(DIST_VERSION).tar.gz xwl_$(DIST_VERSION).orig.tar.gz
	tar xzf xwl-$(DIST_VERSION).tar.gz
	cd xwl-$(DIST_VERSION) && debuild -i -us -uc -b
	rm -rf xwl-$(DIST_VERSION) xwl_$(DIST_VERSION).orig.tar.gz

version-clean:
	rm -f version.h

clean: version-clean
	rm -f *~ *-protocol.c *-client-protocol.h *.o xwl-run xwl@.service \
		xwl-*.tar.gz xwl*.deb xwl_*.build xwl_*.buildinfo xwl_*.changes
