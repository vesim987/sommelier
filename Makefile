CC=gcc
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)
CFLAGS=-g -Wall `pkg-config --cflags xcb xcb-composite wayland-server wayland-client` -I. -DVERSION=\"$(GIT_VERSION)\"
LDFLAGS=-lpthread -lm `pkg-config --libs xcb xcb-composite wayland-server wayland-client`
DEPS = xdg-shell-unstable-v6-client-protocol.h aura-shell-client-protocol.h viewporter-client-protocol.h
OBJECTS = xwl.o xdg-shell-unstable-v6-protocol.o aura-shell-protocol.o viewporter-protocol.o

xwl-launch: $(OBJECTS)
	$(CC) $(OBJECTS) -o xwl-run $(LDFLAGS)

%-protocol.c: %.xml
	wayland-scanner code < $< > $@

%-client-protocol.h: %.xml
	wayland-scanner client-header < $< > $@

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJECTS) : $(DEPS)

all: xwl-run

.PHONY: clean
clean:
	rm -f *~ *-protocol.c *-client-protocol.h *.o xwl-run
