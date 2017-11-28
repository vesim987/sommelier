CC=gcc
SED=sed
CLANG_FORMAT=clang-format-3.9
CLANG_TIDY=clang-tidy-3.9
PREFIX = /usr
SYSCONFDIR = /etc
BINDIR = $(PREFIX)/bin
SRCFILES := xwl.c
XMLFILES := aura-shell.xml viewporter.xml xdg-shell-unstable-v6.xml
AUXFILES := Makefile README LICENSE AUTHORS xwl@.service.in version.h.in xwlrc xwl.sh
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
		-e 's|@version[@]|$(DIST_VERSION)|g' \
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

.PHONY: all install uninstall update-version dist deb version-clean clean style check-style tidy

install: all
	install -D xwl-run \
		$(DESTDIR)$(PREFIX)/bin/xwl-run
	install -D xwlrc $(DESTDIR)$(SYSCONFDIR)/xwlrc
	install -m 644 -D xwl@.service \
		$(DESTDIR)$(SYSCONFDIR)/systemd/user/xwl@.service
	install -m 644 -D xwl.sh $(DESTDIR)$(SYSCONFDIR)/profile.d/xwl.sh

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/xwl-run
	rm -f $(DESTDIR)$(SYSCONFDIR)/xwlrc
	rm -f $(DESTDIR)$(SYSCONFDIR)/systemd/user/xwl@.service
	rm -f $(DESTDIR)$(SYSCONFDIR)/profile.d/xwl.sh

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

style: $(DEPS)
	@for src in $(SRCFILES) ; do \
		echo "Formatting $$src..."; \
		$(CLANG_FORMAT) -i "$$src"; \
		$(CLANG_TIDY) -checks='-*,readability-identifier-naming' \
			-config="{CheckOptions: [ \
			{ key: readability-identifier-naming.StructCase, value: lower_case  }, \
			{ key: readability-identifier-naming.FunctionCase, value: lower_case }, \
			{ key: readability-identifier-naming.VariableCase, value: lower_case }, \
			{ key: readability-identifier-naming.GlobalConstantCase, value: lower_case }, \
			{ key: readability-identifier-naming.EnumConstantCase, value: UPPER_CASE } \
			]}" "$$src"; \
	done
	@echo "Done"

check-style:
	@for src in $(SRCFILES) ; do \
		var=`$(CLANG_FORMAT) "$$src" | diff "$$src" - | wc -l`; \
		if [ $$var -ne 0 ] ; then \
			echo "$$src does not respect the coding style (diff: $$var lines)"; \
			exit 1; \
		fi; \
	done
	@echo "Style check passed"

tidy: $(DEPS)
	@for src in $(SRCFILES); do \
		echo "Running tidy on $$src..."; \
		$(CLANG_TIDY) -checks="-*,modernize-use-auto,modernize-use-nullptr, \
			readability-else-after-return,readability-simplify-boolean-expr, \
			readability-redundant-member-init,modernize-use-default-member-init, \
			modernize-use-equals-default,modernize-use-equals-delete, \
			modernize-use-using,modernize-loop-convert, \
			cppcoreguidelines-no-malloc,misc-redundant-expression" \
			"$$src"; \
	done
	@echo "Done"
