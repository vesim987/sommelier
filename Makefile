CC=gcc
SED=sed
PREFIX = /usr
SYSCONFDIR = /etc
BINDIR = $(PREFIX)/bin
SRCFILES := xwl.c
XMLFILES := aura-shell.xml viewporter.xml xdg-shell-unstable-v6.xml
AUXFILES := Makefile README LICENSE AUTHORS xwl@.service.in xwlrc
ALLFILES := $(SRCFILES) $(XMLFILES) $(AUXFILES)
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
CFLAGS=-g -Wall `pkg-config --cflags xcb xcb-composite wayland-server wayland-client libsystemd` -I. -DVERSION=\"$(GIT_VERSION)\" -DXWAYLAND_PATH=\"$(PREFIX)/bin\"
LDFLAGS=-lpthread -lm `pkg-config --libs xcb xcb-composite wayland-server wayland-client libsystemd`
DEPS = xdg-shell-unstable-v6-client-protocol.h aura-shell-client-protocol.h viewporter-client-protocol.h
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

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJECTS): $(DEPS)

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

clean:
	rm -f *~ *-protocol.c *-client-protocol.h *.o xwl-run xwl@.service

dist:
	mkdir -p xwl-$(GIT_VERSION)
	cp $(ALLFILES) xwl-$(GIT_VERSION)
	tar czf xwl-$(GIT_VERSION).tar.gz xwl-$(GIT_VERSION)
	rm -rf xwl-$(GIT_VERSION)

.PHONY: install uninstall clean
