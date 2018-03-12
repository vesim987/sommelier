// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Needed to include both wayland-client.h and wayland-server.h.
#ifndef WL_HIDE_DEPRECATED
#define WL_HIDE_DEPRECATED
#endif

// Needed for pipe2.
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <systemd/sd-daemon.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-util.h>
#include <xcb/composite.h>
#include <xcb/xcb.h>
#include <xcb/xfixes.h>

#include "aura-shell-client-protocol.h"
#include "drm-server-protocol.h"
#include "linux-dmabuf-unstable-v1-client-protocol.h"
#include "version.h"
#include "viewporter-client-protocol.h"
#include "xdg-shell-unstable-v6-client-protocol.h"

#ifndef XWAYLAND_PATH
#define XWAYLAND_PATH "/usr/bin"
#endif

struct xwl;

struct xwl_host_callback {
  struct wl_resource *resource;
  struct wl_callback *proxy;
};

struct xwl_compositor {
  struct xwl *xwl;
  uint32_t id;
  uint32_t version;
  struct wl_global *host_global;
  struct wl_compositor *internal;
};

struct xwl_host_surface {
  struct xwl *xwl;
  struct wl_resource *resource;
  struct wl_surface *proxy;
  struct wp_viewport *viewport;
  uint32_t contents_width;
  uint32_t contents_height;
  int is_cursor;
  uint32_t last_event_serial;
};

struct xwl_host_compositor {
  struct xwl_compositor *compositor;
  struct wl_resource *resource;
  struct wl_compositor *proxy;
};

struct xwl_host_buffer {
  struct wl_resource *resource;
  struct wl_buffer *proxy;
  uint32_t width;
  uint32_t height;
};

struct xwl_host_shm_pool {
  struct wl_resource *resource;
  struct wl_shm_pool *proxy;
};

struct xwl_host_shm {
  struct xwl_shm *shm;
  struct wl_resource *resource;
  struct wl_shm *proxy;
};

struct xwl_shm {
  struct xwl *xwl;
  uint32_t id;
  struct wl_global *host_global;
};

struct xwl_host_shell {
  struct xwl_shell *shell;
  struct wl_resource *resource;
  struct wl_shell *proxy;
};

struct xwl_shell {
  struct xwl *xwl;
  uint32_t id;
  struct wl_global *host_global;
};

struct xwl_host_output {
  struct xwl_output *output;
  struct wl_resource *resource;
  struct wl_output *proxy;
  struct zaura_output *aura_output;
  uint32_t flags;
  int width;
  int height;
  int refresh;
  double scale;
  double max_scale;
};

struct xwl_output {
  struct xwl *xwl;
  uint32_t id;
  uint32_t version;
  struct wl_global *host_global;
  struct wl_list link;
};

struct xwl_seat {
  struct xwl *xwl;
  uint32_t id;
  uint32_t version;
  struct wl_global *host_global;
  uint32_t last_serial;
  struct wl_list link;
};

struct xwl_host_pointer {
  struct xwl_seat *seat;
  struct wl_resource *resource;
  struct wl_pointer *proxy;
  struct wl_resource *focus_resource;
  struct wl_listener focus_resource_listener;
  uint32_t focus_serial;
};

struct xwl_host_keyboard {
  struct xwl_seat *seat;
  struct wl_resource *resource;
  struct wl_keyboard *proxy;
  struct wl_resource *focus_resource;
  struct wl_listener focus_resource_listener;
  uint32_t focus_serial;
};

struct xwl_host_touch {
  struct xwl_seat *seat;
  struct wl_resource *resource;
  struct wl_touch *proxy;
  struct wl_resource *focus_resource;
  struct wl_listener focus_resource_listener;
};

struct xwl_host_seat {
  struct xwl_seat *seat;
  struct wl_resource *resource;
  struct wl_seat *proxy;
};

struct xwl_data_device_manager {
  struct xwl *xwl;
  uint32_t id;
  uint32_t version;
  struct wl_data_device_manager *internal;
};

struct xwl_data_offer {
  struct xwl *xwl;
  struct wl_data_offer *internal;
  int utf8_text;
};

struct xwl_data_source {
  struct xwl *xwl;
  struct wl_data_source *internal;
};

struct xwl_xdg_shell {
  struct xwl *xwl;
  uint32_t id;
  struct zxdg_shell_v6 *internal;
};

struct xwl_aura_shell {
  struct xwl *xwl;
  uint32_t id;
  uint32_t version;
  struct zaura_shell *internal;
};

struct xwl_viewporter {
  struct xwl *xwl;
  uint32_t id;
  struct wp_viewporter *internal;
};

struct xwl_linux_dmabuf {
  struct xwl *xwl;
  uint32_t id;
  uint32_t version;
  struct wl_global *host_drm_global;
  struct zwp_linux_dmabuf_v1 *internal;
};

struct xwl_host_drm {
  struct xwl_linux_dmabuf *linux_dmabuf;
  uint32_t version;
  struct wl_resource *resource;
};

struct xwl_config {
  uint32_t serial;
  uint32_t mask;
  uint32_t values[5];
  uint32_t states_length;
  uint32_t states[3];
};

struct xwl_window {
  struct xwl *xwl;
  xcb_window_t id;
  xcb_window_t frame_id;
  uint32_t host_surface_id;
  int unpaired;
  int x;
  int y;
  int width;
  int height;
  int border_width;
  int depth;
  int managed;
  int realized;
  int activated;
  xcb_window_t transient_for;
  int decorated;
  char *name;
  char *clazz;
  uint32_t size_flags;
  struct xwl_config next_config;
  struct xwl_config pending_config;
  struct zxdg_surface_v6 *xdg_surface;
  struct zxdg_toplevel_v6 *xdg_toplevel;
  struct zxdg_popup_v6 *xdg_popup;
  struct zaura_surface *aura_surface;
  struct wl_list link;
};

enum {
  ATOM_WM_S0,
  ATOM_WM_PROTOCOLS,
  ATOM_WM_STATE,
  ATOM_WM_DELETE_WINDOW,
  ATOM_WM_TAKE_FOCUS,
  ATOM_WL_SURFACE_ID,
  ATOM_UTF8_STRING,
  ATOM_MOTIF_WM_HINTS,
  ATOM_NET_FRAME_EXTENTS,
  ATOM_NET_SUPPORTING_WM_CHECK,
  ATOM_NET_WM_NAME,
  ATOM_NET_WM_MOVERESIZE,
  ATOM_NET_WM_STATE,
  ATOM_NET_WM_STATE_FULLSCREEN,
  ATOM_NET_WM_STATE_MAXIMIZED_VERT,
  ATOM_NET_WM_STATE_MAXIMIZED_HORZ,
  ATOM_CLIPBOARD,
  ATOM_CLIPBOARD_MANAGER,
  ATOM_TARGETS,
  ATOM_TIMESTAMP,
  ATOM_TEXT,
  ATOM_INCR,
  ATOM_WL_SELECTION,
  ATOM_LAST = ATOM_WL_SELECTION,
};

struct xwl {
  char **runprog;
  struct wl_display *display;
  struct wl_display *host_display;
  struct wl_client *client;
  struct xwl_compositor *compositor;
  struct xwl_shm *shm;
  struct xwl_shell *shell;
  struct xwl_data_device_manager *data_device_manager;
  struct xwl_xdg_shell *xdg_shell;
  struct xwl_aura_shell *aura_shell;
  struct xwl_viewporter *viewporter;
  struct xwl_linux_dmabuf *linux_dmabuf;
  struct wl_list outputs;
  struct wl_list seats;
  struct wl_event_source *display_event_source;
  struct wl_event_source *sigchld_event_source;
  int wm_fd;
  const char *drm_device;
  pid_t xwayland_pid;
  pid_t child_pid;
  xcb_connection_t *connection;
  const xcb_query_extension_reply_t *xfixes_extension;
  xcb_screen_t *screen;
  xcb_window_t window;
  struct wl_list windows, unpaired_windows;
  struct xwl_window *host_focus_window;
  int needs_set_input_focus;
  double scale;
  const char *app_id;
  int exit_with_child;
  int clipboard_manager;
  uint32_t frame_color;
  int has_frame_color;
  int show_window_title;
  struct xwl_host_seat *default_seat;
  xcb_window_t selection_window;
  xcb_window_t selection_owner;
  int selection_incremental_transfer;
  xcb_selection_request_event_t selection_request;
  xcb_timestamp_t selection_timestamp;
  struct wl_data_device *selection_data_device;
  struct xwl_data_offer *selection_data_offer;
  struct xwl_data_source *selection_data_source;
  int selection_data_source_send_fd;
  struct wl_event_source *selection_send_event_source;
  xcb_get_property_reply_t *selection_property_reply;
  int selection_property_offset;
  struct wl_event_source *selection_event_source;
  struct wl_array selection_data;
  int selection_data_offer_receive_fd;
  int selection_data_ack_pending;
  union {
    const char *name;
    xcb_intern_atom_cookie_t cookie;
    xcb_atom_t value;
  } atoms[ATOM_LAST + 1];
  xcb_visualid_t visual_ids[256];
  xcb_colormap_t colormaps[256];
};

enum {
  PROPERTY_WM_NAME,
  PROPERTY_WM_CLASS,
  PROPERTY_WM_TRANSIENT_FOR,
  PROPERTY_WM_NORMAL_HINTS,
  PROPERTY_MOTIF_WM_HINTS,
};

#define US_POSITION (1L << 0)
#define US_SIZE (1L << 1)
#define P_POSITION (1L << 2)
#define P_SIZE (1L << 3)
#define P_MIN_SIZE (1L << 4)
#define P_MAX_SIZE (1L << 5)
#define P_RESIZE_INC (1L << 6)
#define P_ASPECT (1L << 7)
#define P_BASE_SIZE (1L << 8)
#define P_WIN_GRAVITY (1L << 9)

#define MWM_HINTS_FUNCTIONS (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_HINTS_INPUT_MODE (1L << 2)
#define MWM_HINTS_STATUS (1L << 3)

#define MWM_DECOR_ALL (1L << 0)
#define MWM_DECOR_BORDER (1L << 1)
#define MWM_DECOR_RESIZEH (1L << 2)
#define MWM_DECOR_TITLE (1L << 3)
#define MWM_DECOR_MENU (1L << 4)
#define MWM_DECOR_MINIMIZE (1L << 5)
#define MWM_DECOR_MAXIMIZE (1L << 6)

#define NET_WM_MOVERESIZE_SIZE_TOPLEFT 0
#define NET_WM_MOVERESIZE_SIZE_TOP 1
#define NET_WM_MOVERESIZE_SIZE_TOPRIGHT 2
#define NET_WM_MOVERESIZE_SIZE_RIGHT 3
#define NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT 4
#define NET_WM_MOVERESIZE_SIZE_BOTTOM 5
#define NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT 6
#define NET_WM_MOVERESIZE_SIZE_LEFT 7
#define NET_WM_MOVERESIZE_MOVE 8

#define NET_WM_STATE_REMOVE 0
#define NET_WM_STATE_ADD 1
#define NET_WM_STATE_TOGGLE 2

#define WM_STATE_WITHDRAWN 0
#define WM_STATE_NORMAL 1
#define WM_STATE_ICONIC 3

#define SEND_EVENT_MASK 0x80

#define CAPTION_HEIGHT 32

#define MIN_SCALE 0.1
#define MAX_SCALE 10.0

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static void xwl_xdg_shell_ping(void *data, struct zxdg_shell_v6 *xdg_shell,
                               uint32_t serial) {
  zxdg_shell_v6_pong(xdg_shell, serial);
}

static const struct zxdg_shell_v6_listener xwl_xdg_shell_listener = {
    xwl_xdg_shell_ping};

static void xwl_send_configure_notify(struct xwl_window *window) {
  xcb_configure_notify_event_t event = {
      .response_type = XCB_CONFIGURE_NOTIFY,
      .event = window->id,
      .window = window->id,
      .above_sibling = XCB_WINDOW_NONE,
      .x = window->x,
      .y = window->y,
      .width = window->width,
      .height = window->height,
      .border_width = window->border_width,
      .override_redirect = 0,
      .pad0 = 0,
      .pad1 = 0,
  };

  xcb_send_event(window->xwl->connection, 0, window->id,
                 XCB_EVENT_MASK_STRUCTURE_NOTIFY, (char *)&event);
}

static void xwl_adjust_window_size_for_screen_size(struct xwl_window *window) {
  struct xwl *xwl = window->xwl;

  // Clamp size to screen.
  window->width = MIN(window->width, xwl->screen->width_in_pixels);
  window->height = MIN(window->height, xwl->screen->height_in_pixels);
}

static void
xwl_adjust_window_position_for_screen_size(struct xwl_window *window) {
  struct xwl *xwl = window->xwl;

  // Center horizontally/vertically.
  window->x = xwl->screen->width_in_pixels / 2 - window->width / 2;
  window->y = xwl->screen->height_in_pixels / 2 - window->height / 2;
}

static void xwl_configure_window(struct xwl_window *window) {
  assert(!window->pending_config.serial);

  if (window->next_config.mask) {
    int x = window->x;
    int y = window->y;
    int i = 0;

    xcb_configure_window(window->xwl->connection, window->frame_id,
                         window->next_config.mask, window->next_config.values);

    if (window->next_config.mask & XCB_CONFIG_WINDOW_X)
      x = window->next_config.values[i++];
    if (window->next_config.mask & XCB_CONFIG_WINDOW_Y)
      y = window->next_config.values[i++];

    assert(window->managed);
    xcb_configure_window(window->xwl->connection, window->id,
                         window->next_config.mask &
                             ~(XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y),
                         &window->next_config.values[i]);

    if (window->next_config.mask & XCB_CONFIG_WINDOW_WIDTH)
      window->width = window->next_config.values[i++];
    if (window->next_config.mask & XCB_CONFIG_WINDOW_HEIGHT)
      window->height = window->next_config.values[i++];
    if (window->next_config.mask & XCB_CONFIG_WINDOW_BORDER_WIDTH)
      window->border_width = window->next_config.values[i++];

    if (x != window->x || y != window->y) {
      window->x = x;
      window->y = y;
      xwl_send_configure_notify(window);
    }
  }

  if (window->managed) {
    xcb_change_property(window->xwl->connection, XCB_PROP_MODE_REPLACE,
                        window->id, window->xwl->atoms[ATOM_NET_WM_STATE].value,
                        XCB_ATOM_ATOM, 32, window->next_config.states_length,
                        window->next_config.states);
  }

  window->pending_config = window->next_config;
  window->next_config.serial = 0;
  window->next_config.mask = 0;
  window->next_config.states_length = 0;
}

static void xwl_set_input_focus(struct xwl *xwl, struct xwl_window *window) {
  if (window) {
    xcb_client_message_event_t event = {
        .response_type = XCB_CLIENT_MESSAGE,
        .format = 32,
        .window = window->id,
        .type = xwl->atoms[ATOM_WM_PROTOCOLS].value,
        .data.data32 =
            {
                xwl->atoms[ATOM_WM_TAKE_FOCUS].value, XCB_CURRENT_TIME,
            },
    };

    if (!window->managed)
      return;

    xcb_send_event(xwl->connection, 0, window->id,
                   XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT, (char *)&event);

    xcb_set_input_focus(xwl->connection, XCB_INPUT_FOCUS_NONE, window->id,
                        XCB_CURRENT_TIME);
  } else {
    xcb_set_input_focus(xwl->connection, XCB_INPUT_FOCUS_NONE, XCB_NONE,
                        XCB_CURRENT_TIME);
  }
}

static void xwl_restack_windows(struct xwl *xwl, uint32_t focus_resource_id) {
  struct xwl_window *sibling;
  uint32_t values[1];

  wl_list_for_each(sibling, &xwl->windows, link) {
    if (!sibling->managed)
      continue;

    // Move focus window to the top and all other windows to the bottom.
    values[0] = sibling->host_surface_id == focus_resource_id
                    ? XCB_STACK_MODE_ABOVE
                    : XCB_STACK_MODE_BELOW;
    xcb_configure_window(xwl->connection, sibling->frame_id,
                         XCB_CONFIG_WINDOW_STACK_MODE, values);
  }
}

static void xwl_roundtrip(struct xwl *xwl) {
  free(xcb_get_input_focus_reply(xwl->connection,
                                 xcb_get_input_focus(xwl->connection), NULL));
}

static int
xwl_process_pending_configure_acks(struct xwl_window *window,
                                   struct xwl_host_surface *host_surface) {
  if (!window->pending_config.serial)
    return 0;

  if (window->managed && host_surface) {
    int width = window->width + window->border_width * 2;
    int height = window->height + window->border_width * 2;
    // Early out if we expect contents to match window size at some point in
    // the future.
    if (width != host_surface->contents_width ||
        height != host_surface->contents_height) {
      return 0;
    }
  }

  if (window->xdg_surface) {
    zxdg_surface_v6_ack_configure(window->xdg_surface,
                                  window->pending_config.serial);
  }
  window->pending_config.serial = 0;

  if (window->next_config.serial)
    xwl_configure_window(window);

  return 1;
}

static void xwl_xdg_surface_configure(void *data,
                                      struct zxdg_surface_v6 *xdg_surface,
                                      uint32_t serial) {
  struct xwl_window *window = zxdg_surface_v6_get_user_data(xdg_surface);

  window->next_config.serial = serial;
  if (!window->pending_config.serial) {
    struct wl_resource *host_resource;
    struct xwl_host_surface *host_surface = NULL;

    host_resource =
        wl_client_get_object(window->xwl->client, window->host_surface_id);
    if (host_resource)
      host_surface = wl_resource_get_user_data(host_resource);

    xwl_configure_window(window);

    if (xwl_process_pending_configure_acks(window, host_surface)) {
      if (host_surface)
        wl_surface_commit(host_surface->proxy);
    }
  }
}

static const struct zxdg_surface_v6_listener xwl_xdg_surface_listener = {
    xwl_xdg_surface_configure};

static void xwl_xdg_toplevel_configure(void *data,
                                       struct zxdg_toplevel_v6 *xdg_toplevel,
                                       int32_t width, int32_t height,
                                       struct wl_array *states) {
  struct xwl_window *window = zxdg_toplevel_v6_get_user_data(xdg_toplevel);
  int activated = 0;
  uint32_t *state;
  int i = 0;

  if (!window->managed)
    return;

  if (width && height) {
    int32_t width_in_pixels = width * window->xwl->scale;
    int32_t height_in_pixels = height * window->xwl->scale;
    int i = 0;

    window->next_config.mask = XCB_CONFIG_WINDOW_WIDTH |
                               XCB_CONFIG_WINDOW_HEIGHT |
                               XCB_CONFIG_WINDOW_BORDER_WIDTH;
    if (!(window->size_flags & (US_POSITION | P_POSITION))) {
      window->next_config.mask |= XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
      window->next_config.values[i++] =
          window->xwl->screen->width_in_pixels / 2 - width_in_pixels / 2;
      window->next_config.values[i++] =
          window->xwl->screen->height_in_pixels / 2 - height_in_pixels / 2;
    }
    window->next_config.values[i++] = width_in_pixels;
    window->next_config.values[i++] = height_in_pixels;
    window->next_config.values[i++] = 0;
  }

  wl_array_for_each(state, states) {
    if (*state == ZXDG_TOPLEVEL_V6_STATE_FULLSCREEN) {
      window->next_config.states[i++] =
          window->xwl->atoms[ATOM_NET_WM_STATE_FULLSCREEN].value;
    }
    if (*state == ZXDG_TOPLEVEL_V6_STATE_MAXIMIZED) {
      window->next_config.states[i++] =
          window->xwl->atoms[ATOM_NET_WM_STATE_MAXIMIZED_VERT].value;
      window->next_config.states[i++] =
          window->xwl->atoms[ATOM_NET_WM_STATE_MAXIMIZED_HORZ].value;
    }
    if (*state == ZXDG_TOPLEVEL_V6_STATE_ACTIVATED)
      activated = 1;
  }

  if (activated != window->activated) {
    if (activated != (window->xwl->host_focus_window == window)) {
      window->xwl->host_focus_window = activated ? window : NULL;
      window->xwl->needs_set_input_focus = 1;
    }
    window->activated = activated;
  }

  window->next_config.states_length = i;
}

static void xwl_xdg_toplevel_close(void *data,
                                   struct zxdg_toplevel_v6 *xdg_toplevel) {
  struct xwl_window *window = zxdg_toplevel_v6_get_user_data(xdg_toplevel);
  xcb_client_message_event_t event = {
      .response_type = XCB_CLIENT_MESSAGE,
      .format = 32,
      .window = window->id,
      .type = window->xwl->atoms[ATOM_WM_PROTOCOLS].value,
      .data.data32 =
          {
              window->xwl->atoms[ATOM_WM_DELETE_WINDOW].value, XCB_CURRENT_TIME,
          },
  };

  xcb_send_event(window->xwl->connection, 0, window->id,
                 XCB_EVENT_MASK_NO_EVENT, (const char *)&event);
}

static const struct zxdg_toplevel_v6_listener xwl_xdg_toplevel_listener = {
    xwl_xdg_toplevel_configure, xwl_xdg_toplevel_close};

static void xwl_xdg_popup_configure(void *data, struct zxdg_popup_v6 *xdg_popup,
                                    int32_t x, int32_t y, int32_t width,
                                    int32_t height) {}

static void xwl_xdg_popup_done(void *data,
                               struct zxdg_popup_v6 *zxdg_popup_v6) {}

static const struct zxdg_popup_v6_listener xwl_xdg_popup_listener = {
    xwl_xdg_popup_configure, xwl_xdg_popup_done};

static void xwl_window_set_wm_state(struct xwl_window *window, int state) {
  struct xwl *xwl = window->xwl;
  uint32_t values[2];

  values[0] = state;
  values[1] = XCB_WINDOW_NONE;

  xcb_change_property(xwl->connection, XCB_PROP_MODE_REPLACE, window->id,
                      xwl->atoms[ATOM_WM_STATE].value,
                      xwl->atoms[ATOM_WM_STATE].value, 32, 2, values);
}

static void xwl_window_update(struct xwl_window *window) {
  struct wl_resource *host_resource = NULL;
  struct xwl_host_surface *host_surface;
  struct xwl *xwl = window->xwl;
  struct xwl_window *parent = NULL;
  const char *app_id = NULL;

  if (window->host_surface_id) {
    host_resource = wl_client_get_object(xwl->client, window->host_surface_id);
    if (host_resource && window->unpaired) {
      wl_list_remove(&window->link);
      wl_list_insert(&xwl->windows, &window->link);
      window->unpaired = 0;
    }
  } else if (!window->unpaired) {
    wl_list_remove(&window->link);
    wl_list_insert(&xwl->unpaired_windows, &window->link);
    window->unpaired = 1;
  }

  if (!host_resource) {
    if (window->aura_surface) {
      zaura_surface_destroy(window->aura_surface);
      window->aura_surface = NULL;
    }
    if (window->xdg_toplevel) {
      zxdg_toplevel_v6_destroy(window->xdg_toplevel);
      window->xdg_toplevel = NULL;
    }
    if (window->xdg_popup) {
      zxdg_popup_v6_destroy(window->xdg_popup);
      window->xdg_popup = NULL;
    }
    if (window->xdg_surface) {
      zxdg_surface_v6_destroy(window->xdg_surface);
      window->xdg_surface = NULL;
    }
    return;
  }

  host_surface = wl_resource_get_user_data(host_resource);
  assert(host_surface);
  assert(!host_surface->is_cursor);

  assert(xwl->xdg_shell);
  assert(xwl->xdg_shell->internal);

  if (window->managed) {
    app_id = xwl->app_id ? xwl->app_id : window->clazz;

    if (window->transient_for != XCB_WINDOW_NONE) {
      struct xwl_window *sibling;

      wl_list_for_each(sibling, &xwl->windows, link) {
        if (sibling->id == window->transient_for) {
          if (sibling->xdg_toplevel)
            parent = sibling;
          break;
        }
      }
    }
  } else {
    struct xwl_window *sibling;
    uint32_t parent_last_event_serial = 0;

    wl_list_for_each(sibling, &xwl->windows, link) {
      struct wl_resource *sibling_host_resource;
      struct xwl_host_surface *sibling_host_surface;

      if (!sibling->realized)
        continue;

      sibling_host_resource =
          wl_client_get_object(xwl->client, sibling->host_surface_id);
      if (!sibling_host_resource)
        continue;

      // Any parent will do but prefer last event window.
      sibling_host_surface = wl_resource_get_user_data(sibling_host_resource);
      if (parent_last_event_serial > sibling_host_surface->last_event_serial)
        continue;

      parent = sibling;
      parent_last_event_serial = sibling_host_surface->last_event_serial;
    }
  }

  if (!window->depth) {
    xcb_get_geometry_reply_t *geometry_reply = xcb_get_geometry_reply(
        xwl->connection, xcb_get_geometry(xwl->connection, window->id), NULL);
    if (geometry_reply) {
      window->depth = geometry_reply->depth;
      free(geometry_reply);
    }
  }

  if (!window->xdg_surface) {
    window->xdg_surface = zxdg_shell_v6_get_xdg_surface(
        xwl->xdg_shell->internal, host_surface->proxy);
    zxdg_surface_v6_set_user_data(window->xdg_surface, window);
    zxdg_surface_v6_add_listener(window->xdg_surface, &xwl_xdg_surface_listener,
                                 window);
  }

  if (xwl->aura_shell) {
    if (!window->aura_surface) {
      window->aura_surface = zaura_shell_get_aura_surface(
          xwl->aura_shell->internal, host_surface->proxy);
    }
    zaura_surface_set_frame(window->aura_surface,
                            window->decorated
                                ? ZAURA_SURFACE_FRAME_TYPE_NORMAL
                                : window->depth == 32
                                      ? ZAURA_SURFACE_FRAME_TYPE_NONE
                                      : ZAURA_SURFACE_FRAME_TYPE_SHADOW);

    if (xwl->has_frame_color &&
        xwl->aura_shell->version >=
            ZAURA_SURFACE_SET_FRAME_COLORS_SINCE_VERSION) {
      zaura_surface_set_frame_colors(window->aura_surface, xwl->frame_color,
                                     xwl->frame_color);
    }
  }

  if (window->managed || !parent) {
    if (!window->xdg_toplevel) {
      window->xdg_toplevel = zxdg_surface_v6_get_toplevel(window->xdg_surface);
      zxdg_toplevel_v6_set_user_data(window->xdg_toplevel, window);
      zxdg_toplevel_v6_add_listener(window->xdg_toplevel,
                                    &xwl_xdg_toplevel_listener, window);
    }
    if (parent)
      zxdg_toplevel_v6_set_parent(window->xdg_toplevel, parent->xdg_toplevel);
    if (window->name && xwl->show_window_title)
      zxdg_toplevel_v6_set_title(window->xdg_toplevel, window->name);
    if (app_id)
      zxdg_toplevel_v6_set_app_id(window->xdg_toplevel, app_id);
  } else if (!window->xdg_popup) {
    struct zxdg_positioner_v6 *positioner;

    positioner = zxdg_shell_v6_create_positioner(xwl->xdg_shell->internal);
    assert(positioner);
    zxdg_positioner_v6_set_anchor(positioner,
                                  ZXDG_POSITIONER_V6_ANCHOR_TOP |
                                      ZXDG_POSITIONER_V6_ANCHOR_LEFT);
    zxdg_positioner_v6_set_gravity(positioner,
                                   ZXDG_POSITIONER_V6_GRAVITY_BOTTOM |
                                       ZXDG_POSITIONER_V6_GRAVITY_RIGHT);
    zxdg_positioner_v6_set_anchor_rect(
        positioner, (window->x - parent->x) / xwl->scale,
        (window->y - parent->y) / xwl->scale, 1, 1);

    window->xdg_popup = zxdg_surface_v6_get_popup(
        window->xdg_surface, parent->xdg_surface, positioner);
    zxdg_popup_v6_set_user_data(window->xdg_popup, window);
    zxdg_popup_v6_add_listener(window->xdg_popup, &xwl_xdg_popup_listener,
                               window);

    zxdg_positioner_v6_destroy(positioner);
  }

  if ((window->size_flags & (US_POSITION | P_POSITION)) && parent &&
      xwl->aura_shell &&
      xwl->aura_shell->version >= ZAURA_SURFACE_SET_PARENT_SINCE_VERSION) {
    zaura_surface_set_parent(window->aura_surface, parent->aura_surface,
                             (window->x - parent->x) / xwl->scale,
                             (window->y - parent->y) / xwl->scale);
  }

  wl_surface_commit(host_surface->proxy);
  if (host_surface->contents_width && host_surface->contents_height)
    window->realized = 1;
}

static void xwl_host_surface_destroy(struct wl_client *client,
                                     struct wl_resource *resource) {
  wl_resource_destroy(resource);
}

static void xwl_host_surface_attach(struct wl_client *client,
                                    struct wl_resource *resource,
                                    struct wl_resource *buffer_resource,
                                    int32_t x, int32_t y) {
  struct xwl_host_surface *host = wl_resource_get_user_data(resource);
  struct xwl_host_buffer *host_buffer =
      wl_resource_get_user_data(buffer_resource);
  struct wl_buffer *buffer_proxy = NULL;
  struct xwl_window *window;
  double scale = host->xwl->scale;

  if (host_buffer) {
    host->contents_width = host_buffer->width;
    host->contents_height = host_buffer->height;
    buffer_proxy = host_buffer->proxy;
  }

  wl_surface_attach(host->proxy, buffer_proxy, x / scale, y / scale);
  if (host->viewport) {
    if (host_buffer) {
      wp_viewport_set_destination(host->viewport,
                                  ceil(host->contents_width / scale),
                                  ceil(host->contents_height / scale));
    }
  } else {
    wl_surface_set_buffer_scale(host->proxy, scale);
  }

  wl_list_for_each(window, &host->xwl->windows, link) {
    if (window->host_surface_id == wl_resource_get_id(resource)) {
      while (xwl_process_pending_configure_acks(window, host))
        continue;

      break;
    }
  }
}

static void xwl_host_surface_damage(struct wl_client *client,
                                    struct wl_resource *resource, int32_t x,
                                    int32_t y, int32_t width, int32_t height) {
  struct xwl_host_surface *host = wl_resource_get_user_data(resource);
  double scale = host->xwl->scale;
  int32_t x1, y1, x2, y2;

  // Enclosing rect after scaling and outset by one pixel to account for
  // potential filtering.
  x1 = (x - 1) / scale;
  y1 = (y - 1) / scale;
  x2 = ceil((x + width + 1) / scale);
  y2 = ceil((y + height + 1) / scale);

  wl_surface_damage(host->proxy, x1, y1, x2 - x1, y2 - y1);
}

static void xwl_frame_callback_done(void *data, struct wl_callback *callback,
                                    uint32_t time) {
  struct xwl_host_callback *host = wl_callback_get_user_data(callback);

  wl_callback_send_done(host->resource, time);
}

static const struct wl_callback_listener xwl_frame_callback_listener = {
    xwl_frame_callback_done};

static void xwl_host_callback_destroy(struct wl_resource *resource) {
  struct xwl_host_callback *host = wl_resource_get_user_data(resource);

  wl_callback_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_host_surface_frame(struct wl_client *client,
                                   struct wl_resource *resource,
                                   uint32_t callback) {
  struct xwl_host_surface *host = wl_resource_get_user_data(resource);
  struct xwl_host_callback *host_callback;

  host_callback = malloc(sizeof(*host_callback));
  assert(host_callback);

  host_callback->resource =
      wl_resource_create(client, &wl_callback_interface, 1, callback);
  wl_resource_set_implementation(host_callback->resource, NULL, host_callback,
                                 xwl_host_callback_destroy);
  host_callback->proxy = wl_surface_frame(host->proxy);
  wl_callback_set_user_data(host_callback->proxy, host_callback);
  wl_callback_add_listener(host_callback->proxy, &xwl_frame_callback_listener,
                           host_callback);
}

static void
xwl_host_surface_set_opaque_region(struct wl_client *client,
                                   struct wl_resource *resource,
                                   struct wl_resource *region_resource) {
  // Not implemented
  wl_resource_post_no_memory(resource);
}

static void
xwl_host_surface_set_input_region(struct wl_client *client,
                                  struct wl_resource *resource,
                                  struct wl_resource *region_resource) {
  // Not implemented
  wl_resource_post_no_memory(resource);
}

static void xwl_host_surface_commit(struct wl_client *client,
                                    struct wl_resource *resource) {
  struct xwl_host_surface *host = wl_resource_get_user_data(resource);
  struct xwl_window *window;

  // No need to defer cursor commits.
  if (host->is_cursor) {
    wl_surface_commit(host->proxy);
    return;
  }

  // Commit if surface is associated with a window. Otherwise, defer
  // commit until window is created.
  wl_list_for_each(window, &host->xwl->windows, link) {
    if (window->host_surface_id == wl_resource_get_id(resource)) {
      if (window->xdg_surface) {
        wl_surface_commit(host->proxy);
        if (host->contents_width && host->contents_height)
          window->realized = 1;
      }
      break;
    }
  }
}

static void xwl_host_surface_set_buffer_transform(struct wl_client *client,
                                                  struct wl_resource *resource,
                                                  int32_t transform) {
  // Not implemented.
  wl_resource_post_no_memory(resource);
}

static void xwl_host_surface_set_buffer_scale(struct wl_client *client,
                                              struct wl_resource *resource,
                                              int32_t scale) {
  // Not implemented.
  wl_resource_post_no_memory(resource);
}

static void xwl_host_surface_damage_buffer(struct wl_client *client,
                                           struct wl_resource *resource,
                                           int32_t x, int32_t y, int32_t width,
                                           int32_t height) {
  // Not implemented.
  wl_resource_post_no_memory(resource);
}

static const struct wl_surface_interface xwl_surface_implementation = {
    xwl_host_surface_destroy,
    xwl_host_surface_attach,
    xwl_host_surface_damage,
    xwl_host_surface_frame,
    xwl_host_surface_set_opaque_region,
    xwl_host_surface_set_input_region,
    xwl_host_surface_commit,
    xwl_host_surface_set_buffer_transform,
    xwl_host_surface_set_buffer_scale,
    xwl_host_surface_damage_buffer};

static void xwl_destroy_host_surface(struct wl_resource *resource) {
  struct xwl_host_surface *host = wl_resource_get_user_data(resource);
  struct xwl_window *window, *surface_window = NULL;

  wl_list_for_each(window, &host->xwl->windows, link) {
    if (window->host_surface_id == wl_resource_get_id(resource)) {
      surface_window = window;
      break;
    }
  }

  if (surface_window) {
    surface_window->host_surface_id = 0;
    xwl_window_update(surface_window);
  }

  if (host->viewport)
    wp_viewport_destroy(host->viewport);
  wl_surface_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_compositor_create_host_surface(struct wl_client *client,
                                               struct wl_resource *resource,
                                               uint32_t id) {
  struct xwl_host_compositor *host = wl_resource_get_user_data(resource);
  struct xwl_host_surface *host_surface;
  struct xwl_window *window, *unpaired_window = NULL;

  host_surface = malloc(sizeof(*host_surface));
  assert(host_surface);

  host_surface->xwl = host->compositor->xwl;
  host_surface->contents_width = 0;
  host_surface->contents_height = 0;
  host_surface->is_cursor = 0;
  host_surface->last_event_serial = 0;
  host_surface->resource = wl_resource_create(
      client, &wl_surface_interface, wl_resource_get_version(resource), id);
  wl_resource_set_implementation(host_surface->resource,
                                 &xwl_surface_implementation, host_surface,
                                 xwl_destroy_host_surface);
  host_surface->proxy = wl_compositor_create_surface(host->proxy);
  wl_surface_set_user_data(host_surface->proxy, host_surface);
  if (host_surface->xwl->viewporter) {
    host_surface->viewport = wp_viewporter_get_viewport(
        host_surface->xwl->viewporter->internal, host_surface->proxy);
  }

  wl_list_for_each(window, &host->compositor->xwl->unpaired_windows, link) {
    if (window->host_surface_id == id) {
      unpaired_window = window;
      break;
    }
  }

  if (unpaired_window)
    xwl_window_update(window);
}

static void xwl_compositor_create_host_region(struct wl_client *client,
                                              struct wl_resource *resource,
                                              uint32_t id) {
  // Not implemented.
  wl_resource_post_no_memory(resource);
}

static const struct wl_compositor_interface xwl_compositor_implementation = {
    xwl_compositor_create_host_surface, xwl_compositor_create_host_region};

static void xwl_destroy_host_compositor(struct wl_resource *resource) {
  struct xwl_host_compositor *host = wl_resource_get_user_data(resource);

  wl_compositor_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_bind_host_compositor(struct wl_client *client, void *data,
                                     uint32_t version, uint32_t id) {
  struct xwl_compositor *compositor = (struct xwl_compositor *)data;
  struct xwl_host_compositor *host;

  host = malloc(sizeof(*host));
  assert(host);
  host->compositor = compositor;
  host->resource = wl_resource_create(client, &wl_compositor_interface,
                                      MIN(version, compositor->version), id);
  wl_resource_set_implementation(host->resource, &xwl_compositor_implementation,
                                 host, xwl_destroy_host_compositor);
  host->proxy = wl_registry_bind(
      wl_display_get_registry(compositor->xwl->display), compositor->id,
      &wl_compositor_interface, compositor->version);
  wl_compositor_set_user_data(host->proxy, host);
}

static void xwl_host_buffer_destroy(struct wl_client *client,
                                    struct wl_resource *resource) {
  wl_resource_destroy(resource);
}

static const struct wl_buffer_interface xwl_buffer_implementation = {
    xwl_host_buffer_destroy};

static void xwl_buffer_release(void *data, struct wl_buffer *buffer) {
  struct xwl_host_buffer *host = wl_buffer_get_user_data(buffer);

  wl_buffer_send_release(host->resource);
}

static const struct wl_buffer_listener xwl_buffer_listener = {
    xwl_buffer_release};

static void xwl_destroy_host_buffer(struct wl_resource *resource) {
  struct xwl_host_buffer *host = wl_resource_get_user_data(resource);

  wl_buffer_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_host_shm_pool_create_host_buffer(struct wl_client *client,
                                                 struct wl_resource *resource,
                                                 uint32_t id, int32_t offset,
                                                 int32_t width, int32_t height,
                                                 int32_t stride,
                                                 uint32_t format) {
  struct xwl_host_shm_pool *host = wl_resource_get_user_data(resource);
  struct xwl_host_buffer *host_buffer;

  host_buffer = malloc(sizeof(*host_buffer));
  assert(host_buffer);

  host_buffer->width = width;
  host_buffer->height = height;
  host_buffer->resource =
      wl_resource_create(client, &wl_buffer_interface, 1, id);
  wl_resource_set_implementation(host_buffer->resource,
                                 &xwl_buffer_implementation, host_buffer,
                                 xwl_destroy_host_buffer);
  host_buffer->proxy = wl_shm_pool_create_buffer(host->proxy, offset, width,
                                                 height, stride, format);
  wl_buffer_set_user_data(host_buffer->proxy, host_buffer);
  wl_buffer_add_listener(host_buffer->proxy, &xwl_buffer_listener, host_buffer);
}

static void xwl_host_shm_pool_destroy(struct wl_client *client,
                                      struct wl_resource *resource) {
  wl_resource_destroy(resource);
}

static void xwl_host_shm_pool_resize(struct wl_client *client,
                                     struct wl_resource *resource,
                                     int32_t size) {
  struct xwl_host_shm_pool *host = wl_resource_get_user_data(resource);

  wl_shm_pool_resize(host->proxy, size);
}

static const struct wl_shm_pool_interface xwl_shm_pool_implementation = {
    xwl_host_shm_pool_create_host_buffer, xwl_host_shm_pool_destroy,
    xwl_host_shm_pool_resize};

static void xwl_destroy_host_shm_pool(struct wl_resource *resource) {
  struct xwl_host_shm_pool *host = wl_resource_get_user_data(resource);

  wl_shm_pool_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_shm_create_host_pool(struct wl_client *client,
                                     struct wl_resource *resource, uint32_t id,
                                     int fd, int32_t size) {
  struct xwl_host_shm *host = wl_resource_get_user_data(resource);
  struct xwl_host_shm_pool *host_shm_pool;

  host_shm_pool = malloc(sizeof(*host_shm_pool));
  assert(host_shm_pool);

  host_shm_pool->resource =
      wl_resource_create(client, &wl_shm_pool_interface, 1, id);
  wl_resource_set_implementation(host_shm_pool->resource,
                                 &xwl_shm_pool_implementation, host_shm_pool,
                                 xwl_destroy_host_shm_pool);
  host_shm_pool->proxy = wl_shm_create_pool(host->proxy, fd, size);
  wl_shm_pool_set_user_data(host_shm_pool->proxy, host_shm_pool);

  close(fd);
}

static const struct wl_shm_interface xwl_shm_implementation = {
    xwl_shm_create_host_pool};

static void xwl_destroy_host_shm(struct wl_resource *resource) {
  struct xwl_host_shm *host = wl_resource_get_user_data(resource);

  wl_shm_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_bind_host_shm(struct wl_client *client, void *data,
                              uint32_t version, uint32_t id) {
  struct xwl_shm *shm = (struct xwl_shm *)data;
  struct xwl_host_shm *host;

  host = malloc(sizeof(*host));
  assert(host);
  host->shm = shm;
  host->resource = wl_resource_create(client, &wl_shm_interface, 1, id);
  wl_resource_set_implementation(host->resource, &xwl_shm_implementation, host,
                                 xwl_destroy_host_shm);
  host->proxy = wl_registry_bind(wl_display_get_registry(shm->xwl->display),
                                 shm->id, &wl_shm_interface,
                                 wl_resource_get_version(host->resource));
  wl_shm_set_user_data(host->proxy, host);
}

static void
xwl_host_shell_get_host_shell_surface(struct wl_client *client,
                                      struct wl_resource *resource, uint32_t id,
                                      struct wl_resource *surface_resource) {
  // Not implemented.
  wl_resource_post_no_memory(resource);
}

static const struct wl_shell_interface xwl_shell_implementation = {
    xwl_host_shell_get_host_shell_surface};

static void xwl_destroy_host_shell(struct wl_resource *resource) {
  struct xwl_host_shell *host = wl_resource_get_user_data(resource);

  wl_shell_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_bind_host_shell(struct wl_client *client, void *data,
                                uint32_t version, uint32_t id) {
  struct xwl_shell *shell = (struct xwl_shell *)data;
  struct xwl_host_shell *host;

  host = malloc(sizeof(*host));
  assert(host);
  host->shell = shell;
  host->resource = wl_resource_create(client, &wl_shell_interface, 1, id);
  wl_resource_set_implementation(host->resource, &xwl_shell_implementation,
                                 host, xwl_destroy_host_shell);
  host->proxy = wl_registry_bind(wl_display_get_registry(shell->xwl->display),
                                 shell->id, &wl_shell_interface,
                                 wl_resource_get_version(host->resource));
  wl_shell_set_user_data(host->proxy, host);
}

static void xwl_output_geometry(void *data, struct wl_output *output, int x,
                                int y, int physical_width, int physical_height,
                                int subpixel, const char *make,
                                const char *model, int transform) {
  struct xwl_host_output *host = wl_output_get_user_data(output);

  wl_output_send_geometry(host->resource, x, y, physical_width, physical_height,
                          subpixel, make, model, transform);
}

static void xwl_output_mode(void *data, struct wl_output *output,
                            uint32_t flags, int width, int height,
                            int refresh) {
  struct xwl_host_output *host = wl_output_get_user_data(output);

  // Wait until scale is known before sending mode.
  host->flags = flags;
  host->width = width;
  host->height = height;
  host->refresh = refresh;
}

static void xwl_output_done(void *data, struct wl_output *output) {
  struct xwl_host_output *host = wl_output_get_user_data(output);
  double scale = host->output->xwl->scale;

  // Send mode now that scale is known.
  wl_output_send_mode(host->resource, host->flags,
                      (scale * host->scale * host->width) / host->max_scale,
                      (scale * host->scale * host->height) / host->max_scale,
                      host->refresh);
  wl_output_send_scale(host->resource, 1);
  wl_output_send_done(host->resource);

  host->max_scale = 1.0;
}

static void xwl_output_scale(void *data, struct wl_output *output,
                             int32_t scale) {}

static const struct wl_output_listener xwl_output_listener = {
    xwl_output_geometry, xwl_output_mode, xwl_output_done, xwl_output_scale};

static void xwl_aura_output_scale(void *data, struct zaura_output *output,
                                  uint32_t flags, uint32_t scale) {
  struct xwl_host_output *host = zaura_output_get_user_data(output);

  switch (scale) {
  case ZAURA_OUTPUT_SCALE_FACTOR_0500:
  case ZAURA_OUTPUT_SCALE_FACTOR_0600:
  case ZAURA_OUTPUT_SCALE_FACTOR_0625:
  case ZAURA_OUTPUT_SCALE_FACTOR_0750:
  case ZAURA_OUTPUT_SCALE_FACTOR_0800:
  case ZAURA_OUTPUT_SCALE_FACTOR_1000:
  case ZAURA_OUTPUT_SCALE_FACTOR_1125:
  case ZAURA_OUTPUT_SCALE_FACTOR_1200:
  case ZAURA_OUTPUT_SCALE_FACTOR_1250:
  case ZAURA_OUTPUT_SCALE_FACTOR_1500:
  case ZAURA_OUTPUT_SCALE_FACTOR_1600:
  case ZAURA_OUTPUT_SCALE_FACTOR_2000:
    break;
  default:
    fprintf(stderr, "Warning: Unknown scale factor: %d\n", scale);
    break;
  }

  host->max_scale = MAX(host->max_scale, scale / 1000.0);

  if (flags & ZAURA_OUTPUT_SCALE_PROPERTY_CURRENT)
    host->scale = scale / 1000.0;
}

static const struct zaura_output_listener xwl_aura_output_listener = {
    xwl_aura_output_scale};

static void xwl_destroy_host_output(struct wl_resource *resource) {
  struct xwl_host_output *host = wl_resource_get_user_data(resource);

  if (host->aura_output)
    zaura_output_destroy(host->aura_output);
  wl_output_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_bind_host_output(struct wl_client *client, void *data,
                                 uint32_t version, uint32_t id) {
  struct xwl_output *output = (struct xwl_output *)data;
  struct xwl *xwl = output->xwl;
  struct xwl_host_output *host;

  host = malloc(sizeof(*host));
  assert(host);
  host->output = output;
  host->resource = wl_resource_create(client, &wl_output_interface,
                                      MIN(version, output->version), id);
  wl_resource_set_implementation(host->resource, NULL, host,
                                 xwl_destroy_host_output);
  host->proxy = wl_registry_bind(wl_display_get_registry(xwl->display),
                                 output->id, &wl_output_interface,
                                 wl_resource_get_version(host->resource));
  wl_output_set_user_data(host->proxy, host);
  wl_output_add_listener(host->proxy, &xwl_output_listener, host);
  host->aura_output = NULL;
  host->flags = 0;
  host->width = 1024;
  host->height = 768;
  host->refresh = 60000;
  host->scale = 1.0;
  host->max_scale = 1.0;
  if (xwl->aura_shell &&
      (xwl->aura_shell->version >= ZAURA_SHELL_GET_AURA_OUTPUT_SINCE_VERSION)) {
    host->aura_output =
        zaura_shell_get_aura_output(xwl->aura_shell->internal, host->proxy);
    zaura_output_set_user_data(host->aura_output, host);
    zaura_output_add_listener(host->aura_output, &xwl_aura_output_listener,
                              host);
  }
}

static void xwl_data_offer_destroy(struct xwl_data_offer *host) {
  wl_data_offer_destroy(host->internal);
  free(host);
}

static void xwl_set_selection(struct xwl *xwl,
                              struct xwl_data_offer *data_offer) {

  if (xwl->selection_data_offer) {
    xwl_data_offer_destroy(xwl->selection_data_offer);
    xwl->selection_data_offer = NULL;
  }

  if (xwl->clipboard_manager) {
    if (!data_offer) {
      if (xwl->selection_owner == xwl->selection_window)
        xcb_set_selection_owner(xwl->connection, XCB_ATOM_NONE,
                                xwl->atoms[ATOM_CLIPBOARD].value,
                                xwl->selection_timestamp);
      return;
    }

    xcb_set_selection_owner(xwl->connection, xwl->selection_window,
                            xwl->atoms[ATOM_CLIPBOARD].value, XCB_CURRENT_TIME);
  }

  xwl->selection_data_offer = data_offer;
}

static const char *xwl_utf8_mime_type = "text/plain;charset=utf-8";

static void xwl_data_offer_offer(void *data, struct wl_data_offer *data_offer,
                                 const char *type) {
  struct xwl_data_offer *host = data;

  if (strcmp(type, xwl_utf8_mime_type) == 0)
    host->utf8_text = 1;
}

static void xwl_data_offer_source_actions(void *data,
                                          struct wl_data_offer *data_offer,
                                          uint32_t source_actions) {}

static void xwl_data_offer_action(void *data, struct wl_data_offer *data_offer,
                                  uint32_t dnd_action) {}

static const struct wl_data_offer_listener xwl_data_offer_listener = {
    xwl_data_offer_offer, xwl_data_offer_source_actions, xwl_data_offer_action};

static void xwl_data_device_data_offer(void *data,
                                       struct wl_data_device *data_device,
                                       struct wl_data_offer *data_offer) {
  struct xwl *xwl = (struct xwl *)data;
  struct xwl_data_offer *host_data_offer;

  host_data_offer = malloc(sizeof(*host_data_offer));
  assert(host_data_offer);

  host_data_offer->xwl = xwl;
  host_data_offer->internal = data_offer;
  host_data_offer->utf8_text = 0;

  wl_data_offer_add_listener(host_data_offer->internal,
                             &xwl_data_offer_listener, host_data_offer);
}

static void xwl_data_device_enter(void *data,
                                  struct wl_data_device *data_device,
                                  uint32_t serial, struct wl_surface *surface,
                                  wl_fixed_t x, wl_fixed_t y,
                                  struct wl_data_offer *data_offer) {}

static void xwl_data_device_leave(void *data,
                                  struct wl_data_device *data_device) {}

static void xwl_data_device_motion(void *data,
                                   struct wl_data_device *data_device,
                                   uint32_t time, wl_fixed_t x, wl_fixed_t y) {}

static void xwl_data_device_drop(void *data,
                                 struct wl_data_device *data_device) {}

static void xwl_data_device_selection(void *data,
                                      struct wl_data_device *data_device,
                                      struct wl_data_offer *data_offer) {
  struct xwl *xwl = (struct xwl *)data;
  struct xwl_data_offer *host_data_offer =
      data_offer ? wl_data_offer_get_user_data(data_offer) : NULL;

  xwl_set_selection(xwl, host_data_offer);
}

static const struct wl_data_device_listener xwl_data_device_listener = {
    xwl_data_device_data_offer, xwl_data_device_enter,
    xwl_data_device_leave,      xwl_data_device_motion,
    xwl_data_device_drop,       xwl_data_device_selection};

static void xwl_host_pointer_set_cursor(struct wl_client *client,
                                        struct wl_resource *resource,
                                        uint32_t serial,
                                        struct wl_resource *surface_resource,
                                        int32_t hotspot_x, int32_t hotspot_y) {
  struct xwl_host_pointer *host = wl_resource_get_user_data(resource);
  struct xwl_host_surface *host_surface = NULL;
  double scale = host->seat->xwl->scale;

  if (surface_resource) {
    host_surface = wl_resource_get_user_data(surface_resource);
    host_surface->is_cursor = 1;
    if (host_surface->contents_width && host_surface->contents_height)
      wl_surface_commit(host_surface->proxy);
  }

  wl_pointer_set_cursor(host->proxy, serial,
                        host_surface ? host_surface->proxy : NULL,
                        hotspot_x / scale, hotspot_y / scale);
}

static void xwl_host_pointer_release(struct wl_client *client,
                                     struct wl_resource *resource) {
  wl_resource_destroy(resource);
}

static const struct wl_pointer_interface xwl_pointer_implementation = {
    xwl_host_pointer_set_cursor, xwl_host_pointer_release};

static void xwl_set_last_event_serial(struct xwl_host_surface *host_surface) {
  host_surface->last_event_serial =
      wl_display_next_serial(host_surface->xwl->display);
}

static void xwl_pointer_set_focus(struct xwl_host_pointer *host,
                                  uint32_t serial,
                                  struct xwl_host_surface *host_surface,
                                  wl_fixed_t x, wl_fixed_t y) {
  struct wl_resource *surface_resource =
      host_surface ? host_surface->resource : NULL;

  if (surface_resource == host->focus_resource)
    return;

  if (host->focus_resource)
    wl_pointer_send_leave(host->resource, serial, host->focus_resource);

  wl_list_remove(&host->focus_resource_listener.link);
  wl_list_init(&host->focus_resource_listener.link);
  host->focus_resource = surface_resource;
  host->focus_serial = serial;

  if (surface_resource) {
    double scale = host->seat->xwl->scale;

    // Make sure focus surface is on top before sending enter event.
    xwl_restack_windows(host->seat->xwl, wl_resource_get_id(surface_resource));
    xwl_roundtrip(host->seat->xwl);

    wl_resource_add_destroy_listener(surface_resource,
                                     &host->focus_resource_listener);

    wl_pointer_send_enter(host->resource, serial, surface_resource, x * scale,
                          y * scale);
  }
}

static void xwl_pointer_enter(void *data, struct wl_pointer *pointer,
                              uint32_t serial, struct wl_surface *surface,
                              wl_fixed_t x, wl_fixed_t y) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);
  struct xwl_host_surface *host_surface =
      surface ? wl_surface_get_user_data(surface) : NULL;

  if (!host_surface)
    return;

  xwl_pointer_set_focus(host, serial, host_surface, x, y);

  host->seat->last_serial = serial;
}

static void xwl_pointer_leave(void *data, struct wl_pointer *pointer,
                              uint32_t serial, struct wl_surface *surface) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

  xwl_pointer_set_focus(host, serial, NULL, 0, 0);
}

static void xwl_pointer_motion(void *data, struct wl_pointer *pointer,
                               uint32_t time, wl_fixed_t x, wl_fixed_t y) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);
  double scale = host->seat->xwl->scale;

  wl_pointer_send_motion(host->resource, time, x * scale, y * scale);
}

static void xwl_pointer_button(void *data, struct wl_pointer *pointer,
                               uint32_t serial, uint32_t time, uint32_t button,
                               uint32_t state) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

  wl_pointer_send_button(host->resource, serial, time, button, state);

  host->seat->last_serial = serial;
}

static void xwl_pointer_axis(void *data, struct wl_pointer *pointer,
                             uint32_t time, uint32_t axis, wl_fixed_t value) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);
  double scale = host->seat->xwl->scale;

  wl_pointer_send_axis(host->resource, time, axis, value * scale);
}

static void xwl_pointer_frame(void *data, struct wl_pointer *pointer) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

  if (host->focus_resource)
    xwl_set_last_event_serial(wl_resource_get_user_data(host->focus_resource));

  wl_pointer_send_frame(host->resource);
}

void xwl_pointer_axis_source(void *data, struct wl_pointer *pointer,
                             uint32_t axis_source) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

  wl_pointer_send_axis_source(host->resource, axis_source);
}

static void xwl_pointer_axis_stop(void *data, struct wl_pointer *pointer,
                                  uint32_t time, uint32_t axis) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

  wl_pointer_send_axis_stop(host->resource, time, axis);
}

static void xwl_pointer_axis_discrete(void *data, struct wl_pointer *pointer,
                                      uint32_t axis, int32_t discrete) {
  struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

  wl_pointer_send_axis_discrete(host->resource, axis, discrete);
}

static const struct wl_pointer_listener xwl_pointer_listener = {
    xwl_pointer_enter,       xwl_pointer_leave,     xwl_pointer_motion,
    xwl_pointer_button,      xwl_pointer_axis,      xwl_pointer_frame,
    xwl_pointer_axis_source, xwl_pointer_axis_stop, xwl_pointer_axis_discrete};

static void xwl_host_keyboard_release(struct wl_client *client,
                                      struct wl_resource *resource) {
  wl_resource_destroy(resource);
}

static const struct wl_keyboard_interface xwl_keyboard_implementation = {
    xwl_host_keyboard_release};

static void xwl_keyboard_keymap(void *data, struct wl_keyboard *keyboard,
                                uint32_t format, int32_t fd, uint32_t size) {
  struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

  wl_keyboard_send_keymap(host->resource, format, fd, size);

  close(fd);
}

static void xwl_keyboard_set_focus(struct xwl_host_keyboard *host,
                                   uint32_t serial,
                                   struct xwl_host_surface *host_surface,
                                   struct wl_array *keys) {
  struct wl_resource *surface_resource =
      host_surface ? host_surface->resource : NULL;

  if (surface_resource == host->focus_resource)
    return;

  if (host->focus_resource)
    wl_keyboard_send_leave(host->resource, serial, host->focus_resource);

  wl_list_remove(&host->focus_resource_listener.link);
  wl_list_init(&host->focus_resource_listener.link);
  host->focus_resource = surface_resource;
  host->focus_serial = serial;

  if (surface_resource) {
    wl_resource_add_destroy_listener(surface_resource,
                                     &host->focus_resource_listener);
    wl_keyboard_send_enter(host->resource, serial, surface_resource, keys);
  }

  host->seat->last_serial = serial;
}

static void xwl_keyboard_enter(void *data, struct wl_keyboard *keyboard,
                               uint32_t serial, struct wl_surface *surface,
                               struct wl_array *keys) {
  struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);
  struct xwl_host_surface *host_surface =
      surface ? wl_surface_get_user_data(surface) : NULL;

  if (!host_surface)
    return;

  xwl_keyboard_set_focus(host, serial, host_surface, keys);

  host->seat->last_serial = serial;
}

static void xwl_keyboard_leave(void *data, struct wl_keyboard *keyboard,
                               uint32_t serial, struct wl_surface *surface) {
  struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

  xwl_keyboard_set_focus(host, serial, NULL, NULL);
}

static void xwl_keyboard_key(void *data, struct wl_keyboard *keyboard,
                             uint32_t serial, uint32_t time, uint32_t key,
                             uint32_t state) {
  struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

  if (host->focus_resource)
    xwl_set_last_event_serial(wl_resource_get_user_data(host->focus_resource));

  wl_keyboard_send_key(host->resource, serial, time, key, state);

  host->seat->last_serial = serial;
}

static void xwl_keyboard_modifiers(void *data, struct wl_keyboard *keyboard,
                                   uint32_t serial, uint32_t mods_depressed,
                                   uint32_t mods_latched, uint32_t mods_locked,
                                   uint32_t group) {
  struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

  wl_keyboard_send_modifiers(host->resource, serial, mods_depressed,
                             mods_latched, mods_locked, group);

  host->seat->last_serial = serial;
}

static void xwl_keyboard_repeat_info(void *data, struct wl_keyboard *keyboard,
                                     int32_t rate, int32_t delay) {
  struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

  wl_keyboard_send_repeat_info(host->resource, rate, delay);
}

static const struct wl_keyboard_listener xwl_keyboard_listener = {
    xwl_keyboard_keymap, xwl_keyboard_enter,     xwl_keyboard_leave,
    xwl_keyboard_key,    xwl_keyboard_modifiers, xwl_keyboard_repeat_info};

static void xwl_host_touch_release(struct wl_client *client,
                                   struct wl_resource *resource) {
  wl_resource_destroy(resource);
}

static const struct wl_touch_interface xwl_touch_implementation = {
    xwl_host_touch_release};

static void xwl_host_touch_down(void *data, struct wl_touch *touch,
                                uint32_t serial, uint32_t time,
                                struct wl_surface *surface, int32_t id,
                                wl_fixed_t x, wl_fixed_t y) {
  struct xwl_host_touch *host = wl_touch_get_user_data(touch);
  struct xwl_host_surface *host_surface =
      surface ? wl_surface_get_user_data(surface) : NULL;
  double scale = host->seat->xwl->scale;

  if (!host_surface)
    return;

  if (host_surface->resource != host->focus_resource) {
    wl_list_remove(&host->focus_resource_listener.link);
    wl_list_init(&host->focus_resource_listener.link);
    host->focus_resource = host_surface->resource;
    wl_resource_add_destroy_listener(host_surface->resource,
                                     &host->focus_resource_listener);
  }

  // Make sure focus surface is on top before sending down event.
  xwl_restack_windows(host->seat->xwl,
                      wl_resource_get_id(host_surface->resource));
  xwl_roundtrip(host->seat->xwl);

  wl_touch_send_down(host->resource, serial, time, host_surface->resource, id,
                     x * scale, y * scale);

  host->seat->last_serial = serial;
}

static void xwl_host_touch_up(void *data, struct wl_touch *touch,
                              uint32_t serial, uint32_t time, int32_t id) {
  struct xwl_host_touch *host = wl_touch_get_user_data(touch);

  wl_list_remove(&host->focus_resource_listener.link);
  wl_list_init(&host->focus_resource_listener.link);
  host->focus_resource = NULL;

  wl_touch_send_up(host->resource, serial, time, id);

  host->seat->last_serial = serial;
}

static void xwl_host_touch_motion(void *data, struct wl_touch *touch,
                                  uint32_t time, int32_t id, wl_fixed_t x,
                                  wl_fixed_t y) {
  struct xwl_host_touch *host = wl_touch_get_user_data(touch);
  double scale = host->seat->xwl->scale;

  wl_touch_send_motion(host->resource, time, id, x * scale, y * scale);
}

static void xwl_host_touch_frame(void *data, struct wl_touch *touch) {
  struct xwl_host_touch *host = wl_touch_get_user_data(touch);

  if (host->focus_resource)
    xwl_set_last_event_serial(wl_resource_get_user_data(host->focus_resource));

  wl_touch_send_frame(host->resource);
}

static void xwl_host_touch_cancel(void *data, struct wl_touch *touch) {
  struct xwl_host_touch *host = wl_touch_get_user_data(touch);

  wl_touch_send_cancel(host->resource);
}

static const struct wl_touch_listener xwl_touch_listener = {
    xwl_host_touch_down, xwl_host_touch_up, xwl_host_touch_motion,
    xwl_host_touch_frame, xwl_host_touch_cancel};

static void xwl_destroy_host_pointer(struct wl_resource *resource) {
  struct xwl_host_pointer *host = wl_resource_get_user_data(resource);

  wl_pointer_destroy(host->proxy);
  wl_list_remove(&host->focus_resource_listener.link);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_pointer_focus_resource_destroyed(struct wl_listener *listener,
                                                 void *data) {
  struct xwl_host_pointer *host;

  host = wl_container_of(listener, host, focus_resource_listener);
  xwl_pointer_set_focus(host, host->focus_serial, NULL, 0, 0);
}

static void xwl_host_seat_get_host_pointer(struct wl_client *client,
                                           struct wl_resource *resource,
                                           uint32_t id) {
  struct xwl_host_seat *host = wl_resource_get_user_data(resource);
  struct xwl_host_pointer *host_pointer;

  host_pointer = malloc(sizeof(*host_pointer));
  assert(host_pointer);

  host_pointer->seat = host->seat;
  host_pointer->resource = wl_resource_create(
      client, &wl_pointer_interface, wl_resource_get_version(resource), id);
  wl_resource_set_implementation(host_pointer->resource,
                                 &xwl_pointer_implementation, host_pointer,
                                 xwl_destroy_host_pointer);
  host_pointer->proxy = wl_seat_get_pointer(host->proxy);
  wl_pointer_set_user_data(host_pointer->proxy, host_pointer);
  wl_pointer_add_listener(host_pointer->proxy, &xwl_pointer_listener,
                          host_pointer);
  wl_list_init(&host_pointer->focus_resource_listener.link);
  host_pointer->focus_resource_listener.notify =
      xwl_pointer_focus_resource_destroyed;
  host_pointer->focus_resource = NULL;
  host_pointer->focus_serial = 0;
}

static void xwl_destroy_host_keyboard(struct wl_resource *resource) {
  struct xwl_host_keyboard *host = wl_resource_get_user_data(resource);

  wl_keyboard_destroy(host->proxy);
  wl_list_remove(&host->focus_resource_listener.link);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_keyboard_focus_resource_destroyed(struct wl_listener *listener,
                                                  void *data) {
  struct xwl_host_keyboard *host;

  host = wl_container_of(listener, host, focus_resource_listener);
  xwl_keyboard_set_focus(host, host->focus_serial, NULL, NULL);
}

static void xwl_host_seat_get_host_keyboard(struct wl_client *client,
                                            struct wl_resource *resource,
                                            uint32_t id) {
  struct xwl_host_seat *host = wl_resource_get_user_data(resource);
  struct xwl_host_keyboard *host_keyboard;

  host_keyboard = malloc(sizeof(*host_keyboard));
  assert(host_keyboard);

  host_keyboard->seat = host->seat;
  host_keyboard->resource = wl_resource_create(
      client, &wl_keyboard_interface, wl_resource_get_version(resource), id);
  wl_resource_set_implementation(host_keyboard->resource,
                                 &xwl_keyboard_implementation, host_keyboard,
                                 xwl_destroy_host_keyboard);
  host_keyboard->proxy = wl_seat_get_keyboard(host->proxy);
  wl_keyboard_set_user_data(host_keyboard->proxy, host_keyboard);
  wl_keyboard_add_listener(host_keyboard->proxy, &xwl_keyboard_listener,
                           host_keyboard);
  wl_list_init(&host_keyboard->focus_resource_listener.link);
  host_keyboard->focus_resource_listener.notify =
      xwl_keyboard_focus_resource_destroyed;
  host_keyboard->focus_resource = NULL;
  host_keyboard->focus_serial = 0;
}

static void xwl_destroy_host_touch(struct wl_resource *resource) {
  struct xwl_host_touch *host = wl_resource_get_user_data(resource);

  wl_touch_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_touch_focus_resource_destroyed(struct wl_listener *listener,
                                               void *data) {
  struct xwl_host_touch *host;

  host = wl_container_of(listener, host, focus_resource_listener);
  wl_list_remove(&host->focus_resource_listener.link);
  wl_list_init(&host->focus_resource_listener.link);
  host->focus_resource = NULL;
}

static void xwl_host_seat_get_host_touch(struct wl_client *client,
                                         struct wl_resource *resource,
                                         uint32_t id) {
  struct xwl_host_seat *host = wl_resource_get_user_data(resource);
  struct xwl_host_touch *host_touch;

  host_touch = malloc(sizeof(*host_touch));
  assert(host_touch);

  host_touch->seat = host->seat;
  host_touch->resource = wl_resource_create(
      client, &wl_touch_interface, wl_resource_get_version(resource), id);
  wl_resource_set_implementation(host_touch->resource,
                                 &xwl_touch_implementation, host_touch,
                                 xwl_destroy_host_touch);
  host_touch->proxy = wl_seat_get_touch(host->proxy);
  wl_touch_set_user_data(host_touch->proxy, host_touch);
  wl_touch_add_listener(host_touch->proxy, &xwl_touch_listener, host_touch);
  wl_list_init(&host_touch->focus_resource_listener.link);
  host_touch->focus_resource_listener.notify =
      xwl_touch_focus_resource_destroyed;
  host_touch->focus_resource = NULL;
}

static void xwl_host_seat_release(struct wl_client *client,
                                  struct wl_resource *resource) {
  struct xwl_host_seat *host = wl_resource_get_user_data(resource);

  wl_seat_release(host->proxy);
}

static const struct wl_seat_interface xwl_seat_implementation = {
    xwl_host_seat_get_host_pointer, xwl_host_seat_get_host_keyboard,
    xwl_host_seat_get_host_touch, xwl_host_seat_release};

static void xwl_seat_capabilities(void *data, struct wl_seat *seat,
                                  uint32_t capabilities) {
  struct xwl_host_seat *host = wl_seat_get_user_data(seat);

  wl_seat_send_capabilities(host->resource, capabilities);
}

static void xwl_seat_name(void *data, struct wl_seat *seat, const char *name) {
  struct xwl_host_seat *host = wl_seat_get_user_data(seat);

  wl_seat_send_name(host->resource, name);
}

static const struct wl_seat_listener xwl_seat_listener = {xwl_seat_capabilities,
                                                          xwl_seat_name};

static void xwl_destroy_host_seat(struct wl_resource *resource) {
  struct xwl_host_seat *host = wl_resource_get_user_data(resource);

  if (host->seat->xwl->default_seat == host)
    host->seat->xwl->default_seat = NULL;

  wl_seat_destroy(host->proxy);
  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_bind_host_seat(struct wl_client *client, void *data,
                               uint32_t version, uint32_t id) {
  struct xwl_seat *seat = (struct xwl_seat *)data;
  struct xwl_host_seat *host;

  host = malloc(sizeof(*host));
  assert(host);
  host->seat = seat;
  host->resource = wl_resource_create(client, &wl_seat_interface,
                                      MIN(version, seat->version), id);
  wl_resource_set_implementation(host->resource, &xwl_seat_implementation, host,
                                 xwl_destroy_host_seat);
  host->proxy = wl_registry_bind(wl_display_get_registry(seat->xwl->display),
                                 seat->id, &wl_seat_interface,
                                 wl_resource_get_version(host->resource));
  wl_seat_set_user_data(host->proxy, host);
  wl_seat_add_listener(host->proxy, &xwl_seat_listener, host);

  if (!seat->xwl->default_seat) {
    seat->xwl->default_seat = host;
    if (seat->xwl->data_device_manager) {
      seat->xwl->selection_data_device = wl_data_device_manager_get_data_device(
          seat->xwl->data_device_manager->internal, host->proxy);
      wl_data_device_add_listener(seat->xwl->selection_data_device,
                                  &xwl_data_device_listener, seat->xwl);
    }
  }
}

static void xwl_drm_authenticate(struct wl_client *client,
                                 struct wl_resource *resource, uint32_t id) {}

static void xwl_drm_create_buffer(struct wl_client *client,
                                  struct wl_resource *resource, uint32_t id,
                                  uint32_t name, int32_t width, int32_t height,
                                  uint32_t stride, uint32_t format) {
  assert(0);
}

static void xwl_drm_create_planar_buffer(
    struct wl_client *client, struct wl_resource *resource, uint32_t id,
    uint32_t name, int32_t width, int32_t height, uint32_t format,
    int32_t offset0, int32_t stride0, int32_t offset1, int32_t stride1,
    int32_t offset2, int32_t stride2) {
  assert(0);
}

static void xwl_drm_create_prime_buffer(
    struct wl_client *client, struct wl_resource *resource, uint32_t id,
    int32_t name, int32_t width, int32_t height, uint32_t format,
    int32_t offset0, int32_t stride0, int32_t offset1, int32_t stride1,
    int32_t offset2, int32_t stride2) {
  struct xwl_host_drm *host = wl_resource_get_user_data(resource);
  struct xwl_host_buffer *host_buffer;
  struct zwp_linux_buffer_params_v1 *buffer_params;

  assert(name >= 0);
  assert(!offset1);
  assert(!stride1);
  assert(!offset2);
  assert(!stride2);

  host_buffer = malloc(sizeof(*host_buffer));
  assert(host_buffer);

  host_buffer->width = width;
  host_buffer->height = height;
  host_buffer->resource =
      wl_resource_create(client, &wl_buffer_interface, 1, id);
  wl_resource_set_implementation(host_buffer->resource,
                                 &xwl_buffer_implementation, host_buffer,
                                 xwl_destroy_host_buffer);
  buffer_params =
      zwp_linux_dmabuf_v1_create_params(host->linux_dmabuf->internal);
  zwp_linux_buffer_params_v1_add(buffer_params, name, 0, offset0, stride0, 0,
                                 0);
  host_buffer->proxy = zwp_linux_buffer_params_v1_create_immed(
      buffer_params, width, height, format, 0);
  zwp_linux_buffer_params_v1_destroy(buffer_params);
  close(name);
  wl_buffer_set_user_data(host_buffer->proxy, host_buffer);
  wl_buffer_add_listener(host_buffer->proxy, &xwl_buffer_listener, host_buffer);
}

static const struct wl_drm_interface xwl_drm_implementation = {
    xwl_drm_authenticate, xwl_drm_create_buffer, xwl_drm_create_planar_buffer,
    xwl_drm_create_prime_buffer};

static void xwl_destroy_host_drm(struct wl_resource *resource) {
  struct xwl_host_drm *host = wl_resource_get_user_data(resource);

  wl_resource_set_user_data(resource, NULL);
  free(host);
}

static void xwl_bind_host_drm(struct wl_client *client, void *data,
                              uint32_t version, uint32_t id) {
  struct xwl_linux_dmabuf *linux_dmabuf = (struct xwl_linux_dmabuf *)data;
  struct xwl_host_drm *host;

  host = malloc(sizeof(*host));
  assert(host);
  host->linux_dmabuf = linux_dmabuf;
  host->version = MIN(version, 2);
  host->resource =
      wl_resource_create(client, &wl_drm_interface, host->version, id);
  wl_resource_set_implementation(host->resource, &xwl_drm_implementation, host,
                                 xwl_destroy_host_drm);

  wl_drm_send_device(host->resource, linux_dmabuf->xwl->drm_device);
  wl_drm_send_format(host->resource, WL_DRM_FORMAT_ARGB8888);
  wl_drm_send_format(host->resource, WL_DRM_FORMAT_XRGB8888);
  wl_drm_send_format(host->resource, WL_DRM_FORMAT_RGB565);
  if (host->version >= WL_DRM_CREATE_PRIME_BUFFER_SINCE_VERSION)
    wl_drm_send_capabilities(host->resource, WL_DRM_CAPABILITY_PRIME);
}

static void xwl_registry_handler(void *data, struct wl_registry *registry,
                                 uint32_t id, const char *interface,
                                 uint32_t version) {
  struct xwl *xwl = (struct xwl *)data;

  if (strcmp(interface, "wl_compositor") == 0) {
    struct xwl_compositor *compositor = malloc(sizeof(struct xwl_compositor));
    assert(compositor);
    compositor->xwl = xwl;
    compositor->id = id;
    assert(version >= 3);
    compositor->version = 3;
    compositor->host_global = wl_global_create(
        xwl->host_display, &wl_compositor_interface, compositor->version,
        compositor, xwl_bind_host_compositor);
    compositor->internal = wl_registry_bind(
        registry, id, &wl_compositor_interface, compositor->version);
    assert(!xwl->compositor);
    xwl->compositor = compositor;
  } else if (strcmp(interface, "wl_shm") == 0) {
    struct xwl_shm *shm = malloc(sizeof(struct xwl_shm));
    assert(shm);
    shm->xwl = xwl;
    shm->id = id;
    shm->host_global = wl_global_create(xwl->host_display, &wl_shm_interface, 1,
                                        shm, xwl_bind_host_shm);
    assert(!xwl->shm);
    xwl->shm = shm;
  } else if (strcmp(interface, "wl_shell") == 0) {
    struct xwl_shell *shell = malloc(sizeof(struct xwl_shell));
    assert(shell);
    shell->xwl = xwl;
    shell->id = id;
    shell->host_global = wl_global_create(
        xwl->host_display, &wl_shell_interface, 1, shell, xwl_bind_host_shell);
    assert(!xwl->shell);
    xwl->shell = shell;
  } else if (strcmp(interface, "wl_output") == 0) {
    struct xwl_output *output = malloc(sizeof(struct xwl_output));
    assert(output);
    output->xwl = xwl;
    output->id = id;
    output->version = MIN(2, version);
    output->host_global =
        wl_global_create(xwl->host_display, &wl_output_interface,
                         output->version, output, xwl_bind_host_output);
    wl_list_insert(&xwl->outputs, &output->link);
  } else if (strcmp(interface, "wl_seat") == 0) {
    struct xwl_seat *seat = malloc(sizeof(struct xwl_seat));
    assert(seat);
    seat->xwl = xwl;
    seat->id = id;
    seat->version = MIN(5, version);
    seat->host_global =
        wl_global_create(xwl->host_display, &wl_seat_interface, seat->version,
                         seat, xwl_bind_host_seat);
    seat->last_serial = 0;
    wl_list_insert(&xwl->seats, &seat->link);
  } else if (strcmp(interface, "wl_data_device_manager") == 0) {
    struct xwl_data_device_manager *data_device_manager =
        malloc(sizeof(struct xwl_data_device_manager));
    assert(data_device_manager);
    data_device_manager->xwl = xwl;
    data_device_manager->id = id;
    data_device_manager->version = MIN(3, version);
    data_device_manager->internal =
        wl_registry_bind(registry, id, &wl_data_device_manager_interface,
                         data_device_manager->version);
    xwl->data_device_manager = data_device_manager;
  } else if (strcmp(interface, "zxdg_shell_v6") == 0) {
    struct xwl_xdg_shell *xdg_shell = malloc(sizeof(struct xwl_xdg_shell));
    assert(xdg_shell);
    xdg_shell->xwl = xwl;
    xdg_shell->id = id;
    xdg_shell->internal =
        wl_registry_bind(registry, id, &zxdg_shell_v6_interface, 1);
    zxdg_shell_v6_add_listener(xdg_shell->internal, &xwl_xdg_shell_listener,
                               NULL);
    assert(!xwl->xdg_shell);
    xwl->xdg_shell = xdg_shell;
  } else if (strcmp(interface, "zaura_shell") == 0) {
    struct xwl_aura_shell *aura_shell = malloc(sizeof(struct xwl_aura_shell));
    assert(aura_shell);
    aura_shell->xwl = xwl;
    aura_shell->id = id;
    aura_shell->version = MIN(3, version);
    aura_shell->internal = wl_registry_bind(
        registry, id, &zaura_shell_interface, aura_shell->version);
    assert(!xwl->aura_shell);
    xwl->aura_shell = aura_shell;
  } else if (strcmp(interface, "wp_viewporter") == 0) {
    struct xwl_viewporter *viewporter = malloc(sizeof(struct xwl_viewporter));
    assert(viewporter);
    viewporter->xwl = xwl;
    viewporter->id = id;
    viewporter->internal =
        wl_registry_bind(registry, id, &wp_viewporter_interface, 1);
    assert(!xwl->viewporter);
    xwl->viewporter = viewporter;
  } else if (strcmp(interface, "zwp_linux_dmabuf_v1") == 0) {
    struct xwl_linux_dmabuf *linux_dmabuf =
        malloc(sizeof(struct xwl_linux_dmabuf));
    assert(linux_dmabuf);
    linux_dmabuf->xwl = xwl;
    linux_dmabuf->id = id;
    linux_dmabuf->version = MIN(2, version);
    linux_dmabuf->host_drm_global = NULL;
    linux_dmabuf->internal = wl_registry_bind(
        registry, id, &zwp_linux_dmabuf_v1_interface, linux_dmabuf->version);
    assert(!xwl->linux_dmabuf);
    xwl->linux_dmabuf = linux_dmabuf;

    // Register wl_drm global if DRM device is available and DMABuf interface
    // version is sufficient.
    if (xwl->drm_device && linux_dmabuf->version >= 2) {
      linux_dmabuf->host_drm_global =
          wl_global_create(xwl->host_display, &wl_drm_interface, 2,
                           linux_dmabuf, xwl_bind_host_drm);
    }
  }
}

static void xwl_registry_remover(void *data, struct wl_registry *registry,
                                 uint32_t id) {
  struct xwl *xwl = (struct xwl *)data;
  struct xwl_output *output;
  struct xwl_seat *seat;

  if (xwl->compositor && xwl->compositor->id == id) {
    wl_global_destroy(xwl->compositor->host_global);
    wl_compositor_destroy(xwl->compositor->internal);
    free(xwl->compositor);
    xwl->compositor = NULL;
    return;
  }
  if (xwl->shm && xwl->shm->id == id) {
    wl_global_destroy(xwl->shm->host_global);
    free(xwl->shm);
    xwl->shm = NULL;
    return;
  }
  if (xwl->shell && xwl->shell->id == id) {
    wl_global_destroy(xwl->shell->host_global);
    free(xwl->shell);
    xwl->shell = NULL;
    return;
  }
  if (xwl->data_device_manager && xwl->data_device_manager->id == id) {
    wl_data_device_manager_destroy(xwl->data_device_manager->internal);
    free(xwl->data_device_manager);
    xwl->data_device_manager = NULL;
    return;
  }
  if (xwl->xdg_shell && xwl->xdg_shell->id == id) {
    zxdg_shell_v6_destroy(xwl->xdg_shell->internal);
    free(xwl->xdg_shell);
    xwl->xdg_shell = NULL;
    return;
  }
  if (xwl->aura_shell && xwl->aura_shell->id == id) {
    zaura_shell_destroy(xwl->aura_shell->internal);
    free(xwl->aura_shell);
    xwl->aura_shell = NULL;
    return;
  }
  if (xwl->viewporter && xwl->viewporter->id == id) {
    wp_viewporter_destroy(xwl->viewporter->internal);
    free(xwl->viewporter);
    xwl->viewporter = NULL;
    return;
  }
  if (xwl->linux_dmabuf && xwl->linux_dmabuf->id == id) {
    if (xwl->linux_dmabuf->host_drm_global)
      wl_global_destroy(xwl->linux_dmabuf->host_drm_global);
    zwp_linux_dmabuf_v1_destroy(xwl->linux_dmabuf->internal);
    free(xwl->linux_dmabuf);
    xwl->linux_dmabuf = NULL;
    return;
  }
  wl_list_for_each(output, &xwl->outputs, link) {
    if (output->id == id) {
      wl_global_destroy(output->host_global);
      wl_list_remove(&output->link);
      free(output);
      return;
    }
  }
  wl_list_for_each(seat, &xwl->seats, link) {
    if (seat->id == id) {
      wl_global_destroy(seat->host_global);
      wl_list_remove(&seat->link);
      free(seat);
      return;
    }
  }

  // Not reached.
  assert(0);
}

static const struct wl_registry_listener xwl_registry_listener = {
    xwl_registry_handler, xwl_registry_remover};

static int xwl_handle_event(int fd, uint32_t mask, void *data) {
  struct xwl *xwl = (struct xwl *)data;
  int count = 0;

  if ((mask & WL_EVENT_HANGUP) || (mask & WL_EVENT_ERROR))
    return 0;

  if (mask & WL_EVENT_READABLE)
    count = wl_display_dispatch(xwl->display);
  if (mask & WL_EVENT_WRITABLE)
    wl_display_flush(xwl->display);

  if (mask == 0) {
    count = wl_display_dispatch_pending(xwl->display);
    wl_display_flush(xwl->display);
  }

  return count;
}

static void xwl_create_window(struct xwl *xwl, xcb_window_t id, int x, int y,
                              int width, int height, int border_width) {
  struct xwl_window *window = malloc(sizeof(struct xwl_window));
  uint32_t values[1];
  assert(window);
  window->xwl = xwl;
  window->id = id;
  window->frame_id = XCB_WINDOW_NONE;
  window->host_surface_id = 0;
  window->unpaired = 1;
  window->x = x;
  window->y = y;
  window->width = width;
  window->height = height;
  window->border_width = border_width;
  window->depth = 0;
  window->managed = 0;
  window->realized = 0;
  window->activated = 0;
  window->transient_for = XCB_WINDOW_NONE;
  window->decorated = 0;
  window->name = NULL;
  window->clazz = NULL;
  window->size_flags = P_POSITION;
  window->xdg_surface = NULL;
  window->xdg_toplevel = NULL;
  window->xdg_popup = NULL;
  window->aura_surface = NULL;
  window->next_config.serial = 0;
  window->next_config.mask = 0;
  window->next_config.states_length = 0;
  window->pending_config.serial = 0;
  window->pending_config.mask = 0;
  window->pending_config.states_length = 0;
  wl_list_insert(&xwl->unpaired_windows, &window->link);
  values[0] = XCB_EVENT_MASK_PROPERTY_CHANGE | XCB_EVENT_MASK_FOCUS_CHANGE;
  xcb_change_window_attributes(xwl->connection, window->id, XCB_CW_EVENT_MASK,
                               values);
}

static void xwl_destroy_window(struct xwl_window *window) {
  if (window->frame_id != XCB_WINDOW_NONE)
    xcb_destroy_window(window->xwl->connection, window->frame_id);

  if (window->xwl->host_focus_window == window) {
    window->xwl->host_focus_window = NULL;
    window->xwl->needs_set_input_focus = 1;
  }

  if (window->xdg_popup)
    zxdg_popup_v6_destroy(window->xdg_popup);
  if (window->xdg_toplevel)
    zxdg_toplevel_v6_destroy(window->xdg_toplevel);
  if (window->xdg_surface)
    zxdg_surface_v6_destroy(window->xdg_surface);
  if (window->aura_surface)
    zaura_surface_destroy(window->aura_surface);

  if (window->name)
    free(window->name);
  if (window->clazz)
    free(window->clazz);

  wl_list_remove(&window->link);
  free(window);
}

static int xwl_is_window(struct xwl_window *window, xcb_window_t id) {
  if (window->id == id)
    return 1;

  if (window->frame_id != XCB_WINDOW_NONE) {
    if (window->frame_id == id)
      return 1;
  }

  return 0;
}

static struct xwl_window *xwl_lookup_window(struct xwl *xwl, xcb_window_t id) {
  struct xwl_window *window;

  wl_list_for_each(window, &xwl->windows, link) {
    if (xwl_is_window(window, id))
      return window;
  }
  wl_list_for_each(window, &xwl->unpaired_windows, link) {
    if (xwl_is_window(window, id))
      return window;
  }
  return NULL;
}

static int xwl_is_our_window(struct xwl *xwl, xcb_window_t id) {
  const xcb_setup_t *setup = xcb_get_setup(xwl->connection);

  return (id & ~setup->resource_id_mask) == setup->resource_id_base;
}

static void xwl_handle_create_notify(struct xwl *xwl,
                                     xcb_create_notify_event_t *event) {
  if (xwl_is_our_window(xwl, event->window))
    return;

  xwl_create_window(xwl, event->window, event->x, event->y, event->width,
                    event->height, event->border_width);
}

static void xwl_handle_destroy_notify(struct xwl *xwl,
                                      xcb_destroy_notify_event_t *event) {
  struct xwl_window *window;

  if (xwl_is_our_window(xwl, event->window))
    return;

  window = xwl_lookup_window(xwl, event->window);
  if (!window)
    return;

  xwl_destroy_window(window);
}

static void xwl_handle_reparent_notify(struct xwl *xwl,
                                       xcb_reparent_notify_event_t *event) {
  struct xwl_window *window;

  if (event->parent == xwl->screen->root) {
    int width = 1;
    int height = 1;
    int border_width = 0;
    xcb_get_geometry_reply_t *geometry_reply = xcb_get_geometry_reply(
        xwl->connection, xcb_get_geometry(xwl->connection, event->window),
        NULL);

    if (geometry_reply) {
      width = geometry_reply->width;
      height = geometry_reply->height;
      border_width = geometry_reply->border_width;
      free(geometry_reply);
    }
    xwl_create_window(xwl, event->window, event->x, event->y, width, height,
                      border_width);
    return;
  }

  if (xwl_is_our_window(xwl, event->parent))
    return;

  window = xwl_lookup_window(xwl, event->window);
  if (!window)
    return;

  xwl_destroy_window(window);
}

static void xwl_handle_map_request(struct xwl *xwl,
                                   xcb_map_request_event_t *event) {
  struct xwl_window *window = xwl_lookup_window(xwl, event->window);
  struct {
    int type;
    xcb_atom_t atom;
  } properties[] = {
      {PROPERTY_WM_NAME, XCB_ATOM_WM_NAME},
      {PROPERTY_WM_CLASS, XCB_ATOM_WM_CLASS},
      {PROPERTY_WM_TRANSIENT_FOR, XCB_ATOM_WM_TRANSIENT_FOR},
      {PROPERTY_WM_NORMAL_HINTS, XCB_ATOM_WM_NORMAL_HINTS},
      {PROPERTY_MOTIF_WM_HINTS, xwl->atoms[ATOM_MOTIF_WM_HINTS].value},
  };
  xcb_get_geometry_cookie_t geometry_cookie;
  xcb_get_property_cookie_t property_cookies[ARRAY_SIZE(properties)];
  struct xwl_wm_size_hints {
    uint32_t flags;
    int32_t x, y;
    int32_t width, height;
    int32_t min_width, min_height;
    int32_t max_width, max_height;
    int32_t width_inc, height_inc;
    struct {
      int32_t x;
      int32_t y;
    } min_aspect, max_aspect;
    int32_t base_width, base_height;
    int32_t win_gravity;
  } size_hints = {0};
  struct xwl_mwm_hints {
    uint32_t flags;
    uint32_t functions;
    uint32_t decorations;
    int32_t input_mode;
    uint32_t status;
  } mwm_hints = {0};
  uint32_t values[5];
  int i;

  if (!window)
    return;

  assert(!xwl_is_our_window(xwl, event->window));

  window->managed = 1;
  if (window->frame_id == XCB_WINDOW_NONE)
    geometry_cookie = xcb_get_geometry(xwl->connection, window->id);

  for (i = 0; i < ARRAY_SIZE(properties); ++i) {
    property_cookies[i] =
        xcb_get_property(xwl->connection, 0, window->id, properties[i].atom,
                         XCB_ATOM_ANY, 0, 2048);
  }

  if (window->frame_id == XCB_WINDOW_NONE) {
    xcb_get_geometry_reply_t *geometry_reply =
        xcb_get_geometry_reply(xwl->connection, geometry_cookie, NULL);
    if (geometry_reply) {
      window->x = geometry_reply->x;
      window->y = geometry_reply->y;
      window->width = geometry_reply->width;
      window->height = geometry_reply->height;
      window->depth = geometry_reply->depth;
      free(geometry_reply);
    }
  }

  if (window->name) {
    free(window->name);
    window->name = NULL;
  }
  if (window->clazz) {
    free(window->clazz);
    window->clazz = NULL;
  }
  window->transient_for = XCB_WINDOW_NONE;
  window->decorated = 1;
  window->size_flags = 0;

  for (i = 0; i < ARRAY_SIZE(properties); ++i) {
    xcb_get_property_reply_t *reply =
        xcb_get_property_reply(xwl->connection, property_cookies[i], NULL);

    if (!reply)
      continue;

    if (reply->type == XCB_ATOM_NONE) {
      free(reply);
      continue;
    }

    switch (properties[i].type) {
    case PROPERTY_WM_NAME:
      window->name = strndup(xcb_get_property_value(reply),
                             xcb_get_property_value_length(reply));
      break;
    case PROPERTY_WM_CLASS: {
      // WM_CLASS property contains two consecutive null-terminated strings.
      // These specify the Instance and Class names. If a global app ID is
      // not set then use Class name for app ID.
      const char *value = xcb_get_property_value(reply);
      int value_length = xcb_get_property_value_length(reply);
      int instance_length = strnlen(value, value_length);
      if (value_length > instance_length) {
        window->clazz = strndup(value + instance_length + 1,
                                value_length - instance_length - 1);
      }
    } break;
    case PROPERTY_WM_TRANSIENT_FOR:
      if (xcb_get_property_value_length(reply) >= 4)
        window->transient_for = *((uint32_t *)xcb_get_property_value(reply));
      break;
    case PROPERTY_WM_NORMAL_HINTS:
      if (xcb_get_property_value_length(reply) >= sizeof(size_hints))
        memcpy(&size_hints, xcb_get_property_value(reply), sizeof(size_hints));
      break;
    case PROPERTY_MOTIF_WM_HINTS:
      if (xcb_get_property_value_length(reply) >= sizeof(mwm_hints))
        memcpy(&mwm_hints, xcb_get_property_value(reply), sizeof(mwm_hints));
      break;
    default:
      break;
    }
    free(reply);
  }

  if (mwm_hints.flags & MWM_HINTS_DECORATIONS) {
    if (mwm_hints.decorations & MWM_DECOR_ALL)
      window->decorated = ~mwm_hints.decorations & MWM_DECOR_TITLE;
    else
      window->decorated = mwm_hints.decorations & MWM_DECOR_TITLE;
  }

  // Allow user/program controlled position for transients.
  if (window->transient_for)
    window->size_flags |= size_hints.flags & (US_POSITION | P_POSITION);

  window->border_width = 0;
  xwl_adjust_window_size_for_screen_size(window);
  if (window->size_flags & (US_POSITION | P_POSITION)) {
    // x/y fields are obsolete but some clients still expect them to be
    // honored so use them if greater than zero.
    if (size_hints.x > 0)
      window->x = size_hints.x;
    if (size_hints.y > 0)
      window->y = size_hints.y;
  } else {
    xwl_adjust_window_position_for_screen_size(window);
  }

  values[0] = window->width;
  values[1] = window->height;
  values[2] = 0;
  xcb_configure_window(xwl->connection, window->id,
                       XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
                           XCB_CONFIG_WINDOW_BORDER_WIDTH,
                       values);
  values[0] = 0;
  values[1] = 0;
  values[2] = window->decorated ? CAPTION_HEIGHT * xwl->scale : 0;
  values[3] = 0;
  xcb_change_property(xwl->connection, XCB_PROP_MODE_REPLACE, window->id,
                      xwl->atoms[ATOM_NET_FRAME_EXTENTS].value,
                      XCB_ATOM_CARDINAL, 32, 4, values);

  if (window->frame_id == XCB_WINDOW_NONE) {
    int depth = window->depth ? window->depth : xwl->screen->root_depth;

    values[0] = xwl->screen->black_pixel;
    values[1] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT;
    values[2] = xwl->colormaps[depth];

    window->frame_id = xcb_generate_id(xwl->connection);
    xcb_create_window(
        xwl->connection, depth, window->frame_id, xwl->screen->root, window->x,
        window->y, window->width, window->height, 0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT, xwl->visual_ids[depth],
        XCB_CW_BORDER_PIXEL | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP, values);
    values[0] = XCB_STACK_MODE_BELOW;
    xcb_configure_window(xwl->connection, window->frame_id,
                         XCB_CONFIG_WINDOW_STACK_MODE, values);
    xcb_reparent_window(xwl->connection, window->id, window->frame_id, 0, 0);
  } else {
    values[0] = window->x;
    values[1] = window->y;
    values[2] = window->width;
    values[3] = window->height;
    values[4] = XCB_STACK_MODE_BELOW;
    xcb_configure_window(
        xwl->connection, window->frame_id,
        XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH |
            XCB_CONFIG_WINDOW_HEIGHT | XCB_CONFIG_WINDOW_STACK_MODE,
        values);
  }

  xwl_window_set_wm_state(window, WM_STATE_NORMAL);
  xwl_send_configure_notify(window);

  xcb_map_window(xwl->connection, window->id);
  xcb_map_window(xwl->connection, window->frame_id);
}

static void xwl_handle_map_notify(struct xwl *xwl,
                                  xcb_map_notify_event_t *event) {}

static void xwl_handle_unmap_notify(struct xwl *xwl,
                                    xcb_unmap_notify_event_t *event) {
  struct xwl_window *window;

  if (xwl_is_our_window(xwl, event->window))
    return;

  if (event->response_type & SEND_EVENT_MASK)
    return;

  window = xwl_lookup_window(xwl, event->window);
  if (!window)
    return;

  if (xwl->host_focus_window == window) {
    xwl->host_focus_window = NULL;
    xwl->needs_set_input_focus = 1;
  }

  if (window->host_surface_id) {
    window->host_surface_id = 0;
    xwl_window_update(window);
  }

  xwl_window_set_wm_state(window, WM_STATE_WITHDRAWN);

  if (window->frame_id != XCB_WINDOW_NONE)
    xcb_unmap_window(xwl->connection, window->frame_id);
}

static void xwl_handle_configure_request(struct xwl *xwl,
                                         xcb_configure_request_event_t *event) {
  struct xwl_window *window = xwl_lookup_window(xwl, event->window);
  int width = window->width;
  int height = window->height;
  uint32_t values[7];

  assert(!xwl_is_our_window(xwl, event->window));

  if (!window->managed) {
    int i = 0;

    if (event->value_mask & XCB_CONFIG_WINDOW_X)
      values[i++] = event->x;
    if (event->value_mask & XCB_CONFIG_WINDOW_Y)
      values[i++] = event->y;
    if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH)
      values[i++] = event->width;
    if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT)
      values[i++] = event->height;
    if (event->value_mask & XCB_CONFIG_WINDOW_BORDER_WIDTH)
      values[i++] = event->border_width;
    if (event->value_mask & XCB_CONFIG_WINDOW_SIBLING)
      values[i++] = event->sibling;
    if (event->value_mask & XCB_CONFIG_WINDOW_STACK_MODE)
      values[i++] = event->stack_mode;

    xcb_configure_window(xwl->connection, window->id, event->value_mask,
                         values);
    return;
  }

  // Ack configure events as satisfying request removes the guarantee
  // that matching contents will arrive.
  if (window->xdg_toplevel) {
    if (window->pending_config.serial) {
      zxdg_surface_v6_ack_configure(window->xdg_surface,
                                    window->pending_config.serial);
      window->pending_config.serial = 0;
      window->pending_config.mask = 0;
      window->pending_config.states_length = 0;
    }
    if (window->next_config.serial) {
      zxdg_surface_v6_ack_configure(window->xdg_surface,
                                    window->next_config.serial);
      window->next_config.serial = 0;
      window->next_config.mask = 0;
      window->next_config.states_length = 0;
    }
  }

  if (event->value_mask & XCB_CONFIG_WINDOW_X)
    window->x = event->x;
  if (event->value_mask & XCB_CONFIG_WINDOW_Y)
    window->y = event->y;
  if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH)
    window->width = event->width;
  if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT)
    window->height = event->height;

  xwl_adjust_window_size_for_screen_size(window);
  if (window->size_flags & (US_POSITION | P_POSITION))
    xwl_window_update(window);
  else
    xwl_adjust_window_position_for_screen_size(window);

  values[0] = window->x;
  values[1] = window->y;
  values[2] = window->width;
  values[3] = window->height;
  values[4] = 0;
  xcb_configure_window(xwl->connection, window->frame_id,
                       XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y |
                           XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
                       values);

  // We need to send a synthetic configure notify if:
  // - Not changing the size, location, border width.
  // - Moving the window without resizing it or changing its border width.
  if (width != window->width || height != window->height ||
      window->border_width) {
    xcb_configure_window(xwl->connection, window->id,
                         XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT |
                             XCB_CONFIG_WINDOW_BORDER_WIDTH,
                         &values[2]);
    window->border_width = 0;
  } else {
    xwl_send_configure_notify(window);
  }
}

static void xwl_handle_configure_notify(struct xwl *xwl,
                                        xcb_configure_notify_event_t *event) {
  struct xwl_window *window;

  if (xwl_is_our_window(xwl, event->window))
    return;

  if (event->window == xwl->screen->root) {
    xcb_get_geometry_reply_t *geometry_reply = xcb_get_geometry_reply(
        xwl->connection, xcb_get_geometry(xwl->connection, event->window),
        NULL);
    int width = xwl->screen->width_in_pixels;
    int height = xwl->screen->height_in_pixels;

    if (geometry_reply) {
      width = geometry_reply->width;
      height = geometry_reply->height;
      free(geometry_reply);
    }

    if (width == xwl->screen->width_in_pixels ||
        height == xwl->screen->height_in_pixels) {
      return;
    }

    xwl->screen->width_in_pixels = width;
    xwl->screen->height_in_pixels = height;

    // Re-center managed windows.
    wl_list_for_each(window, &xwl->windows, link) {
      int x, y;

      if (window->size_flags & (US_POSITION | P_POSITION))
        continue;

      x = window->x;
      y = window->y;
      xwl_adjust_window_position_for_screen_size(window);
      if (window->x != x || window->y != y) {
        uint32_t values[2];

        values[0] = window->x;
        values[1] = window->y;
        xcb_configure_window(xwl->connection, window->frame_id,
                             XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
        xwl_send_configure_notify(window);
      }
    }
    return;
  }

  window = xwl_lookup_window(xwl, event->window);
  if (!window)
    return;

  if (window->managed)
    return;

  window->width = event->width;
  window->height = event->height;
  window->border_width = event->border_width;
  if (event->x != window->x || event->y != window->y) {
    window->x = event->x;
    window->y = event->y;
    xwl_window_update(window);
  }
}

static uint32_t xwl_resize_edge(int net_wm_moveresize_size) {
  switch (net_wm_moveresize_size) {
  case NET_WM_MOVERESIZE_SIZE_TOPLEFT:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_TOP_LEFT;
  case NET_WM_MOVERESIZE_SIZE_TOP:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_TOP;
  case NET_WM_MOVERESIZE_SIZE_TOPRIGHT:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_TOP_RIGHT;
  case NET_WM_MOVERESIZE_SIZE_RIGHT:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_RIGHT;
  case NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_BOTTOM_RIGHT;
  case NET_WM_MOVERESIZE_SIZE_BOTTOM:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_BOTTOM;
  case NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_BOTTOM_LEFT;
  case NET_WM_MOVERESIZE_SIZE_LEFT:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_LEFT;
  default:
    return ZXDG_TOPLEVEL_V6_RESIZE_EDGE_NONE;
  }
}

static void xwl_handle_client_message(struct xwl *xwl,
                                      xcb_client_message_event_t *event) {
  if (event->type == xwl->atoms[ATOM_WL_SURFACE_ID].value) {
    struct xwl_window *window, *unpaired_window = NULL;

    wl_list_for_each(window, &xwl->unpaired_windows, link) {
      if (xwl_is_window(window, event->window)) {
        unpaired_window = window;
        break;
      }
    }

    if (unpaired_window) {
      unpaired_window->host_surface_id = event->data.data32[0];
      xwl_window_update(unpaired_window);
    }
  } else if (event->type == xwl->atoms[ATOM_NET_WM_MOVERESIZE].value) {
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);

    if (window && window->xdg_toplevel) {
      struct xwl_host_seat *seat = window->xwl->default_seat;

      if (!seat)
        return;

      if (event->data.data32[2] == NET_WM_MOVERESIZE_MOVE) {
        zxdg_toplevel_v6_move(window->xdg_toplevel, seat->proxy,
                              seat->seat->last_serial);
      } else {
        uint32_t edge = xwl_resize_edge(event->data.data32[2]);

        if (edge == ZXDG_TOPLEVEL_V6_RESIZE_EDGE_NONE)
          return;

        zxdg_toplevel_v6_resize(window->xdg_toplevel, seat->proxy,
                                seat->seat->last_serial, edge);
      }
    }
  } else if (event->type == xwl->atoms[ATOM_NET_WM_STATE].value) {
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);

    if (window && window->xdg_toplevel) {
      int changed[ATOM_LAST + 1];
      uint32_t action = event->data.data32[0];
      int i;

      for (i = 0; i < ARRAY_SIZE(xwl->atoms); ++i) {
        changed[i] = event->data.data32[1] == xwl->atoms[i].value ||
                     event->data.data32[2] == xwl->atoms[i].value;
      }

      if (changed[ATOM_NET_WM_STATE_FULLSCREEN]) {
        if (action == NET_WM_STATE_ADD)
          zxdg_toplevel_v6_set_fullscreen(window->xdg_toplevel, NULL);
        else if (action == NET_WM_STATE_REMOVE)
          zxdg_toplevel_v6_unset_fullscreen(window->xdg_toplevel);
      }

      if (changed[ATOM_NET_WM_STATE_MAXIMIZED_VERT] &&
          changed[ATOM_NET_WM_STATE_MAXIMIZED_HORZ]) {
        if (action == NET_WM_STATE_ADD)
          zxdg_toplevel_v6_set_maximized(window->xdg_toplevel);
        else if (action == NET_WM_STATE_REMOVE)
          zxdg_toplevel_v6_unset_maximized(window->xdg_toplevel);
      }
    }
  }
}

static void xwl_handle_focus_in(struct xwl *xwl, xcb_focus_in_event_t *event) {}

static void xwl_handle_focus_out(struct xwl *xwl,
                                 xcb_focus_out_event_t *event) {}

static int xwl_handle_selection_fd_writable(int fd, uint32_t mask, void *data) {
  struct xwl *xwl = data;
  uint8_t *value;
  int bytes, bytes_left;

  value = xcb_get_property_value(xwl->selection_property_reply);
  bytes_left = xcb_get_property_value_length(xwl->selection_property_reply) -
               xwl->selection_property_offset;

  bytes = write(fd, value + xwl->selection_property_offset, bytes_left);
  if (bytes == -1) {
    fprintf(stderr, "write error to target fd: %m\n");
    close(fd);
  } else if (bytes == bytes_left) {
    if (xwl->selection_incremental_transfer) {
      xcb_delete_property(xwl->connection, xwl->selection_window,
                          xwl->atoms[ATOM_WL_SELECTION].value);
    } else {
      close(fd);
    }
  } else {
    xwl->selection_property_offset += bytes;
    return 1;
  }

  free(xwl->selection_property_reply);
  xwl->selection_property_reply = NULL;
  if (xwl->selection_send_event_source) {
    wl_event_source_remove(xwl->selection_send_event_source);
    xwl->selection_send_event_source = NULL;
  }
  return 1;
}

static void xwl_write_selection_property(struct xwl *xwl,
                                         xcb_get_property_reply_t *reply) {
  xwl->selection_property_offset = 0;
  xwl->selection_property_reply = reply;
  xwl_handle_selection_fd_writable(xwl->selection_data_source_send_fd,
                                   WL_EVENT_WRITABLE, xwl);

  if (!xwl->selection_property_reply)
    return;

  assert(!xwl->selection_send_event_source);
  xwl->selection_send_event_source = wl_event_loop_add_fd(
      wl_display_get_event_loop(xwl->host_display),
      xwl->selection_data_source_send_fd, WL_EVENT_WRITABLE,
      xwl_handle_selection_fd_writable, xwl);
}

static void xwl_send_selection_notify(struct xwl *xwl, xcb_atom_t property) {
  xcb_selection_notify_event_t event = {
      .response_type = XCB_SELECTION_NOTIFY,
      .sequence = 0,
      .time = xwl->selection_request.time,
      .requestor = xwl->selection_request.requestor,
      .selection = xwl->selection_request.selection,
      .target = xwl->selection_request.target,
      .property = property,
      .pad0 = 0};

  xcb_send_event(xwl->connection, 0, xwl->selection_request.requestor,
                 XCB_EVENT_MASK_NO_EVENT, (char *)&event);
}

static void xwl_send_selection_data(struct xwl *xwl) {
  assert(!xwl->selection_data_ack_pending);
  xcb_change_property(
      xwl->connection, XCB_PROP_MODE_REPLACE, xwl->selection_request.requestor,
      xwl->selection_request.property, xwl->atoms[ATOM_UTF8_STRING].value, 8,
      xwl->selection_data.size, xwl->selection_data.data);
  xwl->selection_data_ack_pending = 1;
  xwl->selection_data.size = 0;
}

static const uint32_t xwl_incr_chunk_size = 64 * 1024;

static int xwl_handle_selection_fd_readable(int fd, uint32_t mask, void *data) {
  struct xwl *xwl = data;
  int bytes, offset, bytes_left;
  void *p;

  offset = xwl->selection_data.size;
  if (xwl->selection_data.size < xwl_incr_chunk_size)
    p = wl_array_add(&xwl->selection_data, xwl_incr_chunk_size);
  else
    p = (char *)xwl->selection_data.data + xwl->selection_data.size;
  bytes_left = xwl->selection_data.alloc - offset;

  bytes = read(fd, p, bytes_left);
  if (bytes == -1) {
    fprintf(stderr, "read error from data source: %m\n");
    xwl_send_selection_notify(xwl, XCB_ATOM_NONE);
    xwl->selection_data_offer_receive_fd = -1;
    close(fd);
  } else {
    xwl->selection_data.size = offset + bytes;
    if (xwl->selection_data.size >= xwl_incr_chunk_size) {
      if (!xwl->selection_incremental_transfer) {
        xwl->selection_incremental_transfer = 1;
        xcb_change_property(
            xwl->connection, XCB_PROP_MODE_REPLACE,
            xwl->selection_request.requestor, xwl->selection_request.property,
            xwl->atoms[ATOM_INCR].value, 32, 1, &xwl_incr_chunk_size);
        xwl->selection_data_ack_pending = 1;
        xwl_send_selection_notify(xwl, xwl->selection_request.property);
      } else if (!xwl->selection_data_ack_pending) {
        xwl_send_selection_data(xwl);
      }
    } else if (bytes == 0) {
      if (!xwl->selection_data_ack_pending)
        xwl_send_selection_data(xwl);
      if (!xwl->selection_incremental_transfer) {
        xwl_send_selection_notify(xwl, xwl->selection_request.property);
        xwl->selection_request.requestor = XCB_NONE;
        wl_array_release(&xwl->selection_data);
      }
      xcb_flush(xwl->connection);
      xwl->selection_data_offer_receive_fd = -1;
      close(fd);
    } else {
      xwl->selection_data.size = offset + bytes;
      return 1;
    }
  }

  wl_event_source_remove(xwl->selection_event_source);
  xwl->selection_event_source = NULL;
  return 1;
}

static void xwl_handle_property_notify(struct xwl *xwl,
                                       xcb_property_notify_event_t *event) {
  if (event->atom == XCB_ATOM_WM_NAME) {
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);
    if (!window)
      return;

    if (window->name) {
      free(window->name);
      window->name = NULL;
    }

    if (event->state != XCB_PROPERTY_DELETE) {
      xcb_get_property_reply_t *reply = xcb_get_property_reply(
          xwl->connection,
          xcb_get_property(xwl->connection, 0, window->id, XCB_ATOM_WM_NAME,
                           XCB_ATOM_ANY, 0, 2048),
          NULL);
      if (reply) {
        window->name = strndup(xcb_get_property_value(reply),
                               xcb_get_property_value_length(reply));
        free(reply);
      }
    }

    if (!window->xdg_toplevel || !xwl->show_window_title)
      return;

    if (window->name) {
      zxdg_toplevel_v6_set_title(window->xdg_toplevel, window->name);
    } else {
      zxdg_toplevel_v6_set_title(window->xdg_toplevel, "");
    }
  } else if (event->atom == xwl->atoms[ATOM_WL_SELECTION].value) {
    if (event->window == xwl->selection_window &&
        event->state == XCB_PROPERTY_NEW_VALUE &&
        xwl->selection_incremental_transfer) {
      xcb_get_property_reply_t *reply = xcb_get_property_reply(
          xwl->connection,
          xcb_get_property(xwl->connection, 0, xwl->selection_window,
                           xwl->atoms[ATOM_WL_SELECTION].value,
                           XCB_GET_PROPERTY_TYPE_ANY, 0, 0x1fffffff),
          NULL);

      if (!reply)
        return;

      if (xcb_get_property_value_length(reply) > 0) {
        xwl_write_selection_property(xwl, reply);
      } else {
        assert(!xwl->selection_send_event_source);
        close(xwl->selection_data_source_send_fd);
        free(reply);
      }
    }
  } else if (event->atom == xwl->selection_request.property) {
    if (event->window == xwl->selection_request.requestor &&
        event->state == XCB_PROPERTY_DELETE &&
        xwl->selection_incremental_transfer) {
      int data_size = xwl->selection_data.size;

      xwl->selection_data_ack_pending = 0;

      // Handle the case when there's more data to be received.
      if (xwl->selection_data_offer_receive_fd >= 0) {
        // Avoid sending empty data until transfer is complete.
        if (data_size)
          xwl_send_selection_data(xwl);

        if (!xwl->selection_event_source) {
          xwl->selection_event_source = wl_event_loop_add_fd(
              wl_display_get_event_loop(xwl->host_display),
              xwl->selection_data_offer_receive_fd, WL_EVENT_READABLE,
              xwl_handle_selection_fd_readable, xwl);
        }
        return;
      }

      xwl_send_selection_data(xwl);

      // Release data if transfer is complete.
      if (!data_size) {
        xwl->selection_request.requestor = XCB_NONE;
        wl_array_release(&xwl->selection_data);
      }
    }
  }
}

static void xwl_data_source_target(void *data,
                                   struct wl_data_source *data_source,
                                   const char *mime_type) {}

static void xwl_data_source_send(void *data, struct wl_data_source *data_source,
                                 const char *mime_type, int32_t fd) {
  struct xwl_data_source *host = data;
  struct xwl *xwl = host->xwl;

  if (strcmp(mime_type, xwl_utf8_mime_type) == 0) {
    xcb_convert_selection(
        xwl->connection, xwl->selection_window,
        xwl->atoms[ATOM_CLIPBOARD].value, xwl->atoms[ATOM_UTF8_STRING].value,
        xwl->atoms[ATOM_WL_SELECTION].value, XCB_CURRENT_TIME);
    fcntl(fd, F_SETFL, O_WRONLY | O_NONBLOCK);
    xwl->selection_data_source_send_fd = fd;
  } else {
    close(fd);
  }
}

static void xwl_data_source_cancelled(void *data,
                                      struct wl_data_source *data_source) {
  struct xwl_data_source *host = data;

  if (host->xwl->selection_data_source == host)
    host->xwl->selection_data_source = NULL;

  wl_data_source_destroy(data_source);
}

static const struct wl_data_source_listener xwl_data_source_listener = {
    xwl_data_source_target, xwl_data_source_send, xwl_data_source_cancelled};

static void xwl_get_selection_targets(struct xwl *xwl) {
  struct xwl_data_source *data_source = NULL;
  xcb_get_property_reply_t *reply;
  xcb_atom_t *value;
  uint32_t i;

  reply = xcb_get_property_reply(
      xwl->connection,
      xcb_get_property(xwl->connection, 1, xwl->selection_window,
                       xwl->atoms[ATOM_WL_SELECTION].value,
                       XCB_GET_PROPERTY_TYPE_ANY, 0, 4096),
      NULL);
  if (!reply)
    return;

  if (reply->type != XCB_ATOM_ATOM) {
    free(reply);
    return;
  }

  if (xwl->data_device_manager) {
    data_source = malloc(sizeof(*data_source));
    assert(data_source);

    data_source->xwl = xwl;
    data_source->internal = wl_data_device_manager_create_data_source(
        xwl->data_device_manager->internal);
    wl_data_source_add_listener(data_source->internal,
                                &xwl_data_source_listener, data_source);

    value = xcb_get_property_value(reply);
    for (i = 0; i < reply->value_len; i++) {
      if (value[i] == xwl->atoms[ATOM_UTF8_STRING].value)
        wl_data_source_offer(data_source->internal, "text/plain;charset=utf-8");
    }

    if (xwl->selection_data_device && xwl->default_seat) {
      wl_data_device_set_selection(xwl->selection_data_device,
                                   data_source->internal,
                                   xwl->default_seat->seat->last_serial);
    }

    if (xwl->selection_data_source) {
      wl_data_source_destroy(xwl->selection_data_source->internal);
      free(xwl->selection_data_source);
    }
    xwl->selection_data_source = data_source;
  }

  free(reply);
}

static void xwl_get_selection_data(struct xwl *xwl) {
  xcb_get_property_reply_t *reply = xcb_get_property_reply(
      xwl->connection,
      xcb_get_property(xwl->connection, 1, xwl->selection_window,
                       xwl->atoms[ATOM_WL_SELECTION].value,
                       XCB_GET_PROPERTY_TYPE_ANY, 0, 0x1fffffff),
      NULL);
  if (!reply)
    return;

  if (reply->type == xwl->atoms[ATOM_INCR].value) {
    xwl->selection_incremental_transfer = 1;
    free(reply);
  } else {
    xwl->selection_incremental_transfer = 0;
    xwl_write_selection_property(xwl, reply);
  }
}

static void xwl_handle_selection_notify(struct xwl *xwl,
                                        xcb_selection_notify_event_t *event) {
  if (event->property == XCB_ATOM_NONE)
    return;

  if (event->target == xwl->atoms[ATOM_TARGETS].value)
    xwl_get_selection_targets(xwl);
  else
    xwl_get_selection_data(xwl);
}

static void xwl_send_targets(struct xwl *xwl) {
  xcb_atom_t targets[] = {
      xwl->atoms[ATOM_TIMESTAMP].value, xwl->atoms[ATOM_TARGETS].value,
      xwl->atoms[ATOM_UTF8_STRING].value, xwl->atoms[ATOM_TEXT].value,
  };

  xcb_change_property(xwl->connection, XCB_PROP_MODE_REPLACE,
                      xwl->selection_request.requestor,
                      xwl->selection_request.property, XCB_ATOM_ATOM, 32,
                      ARRAY_SIZE(targets), targets);

  xwl_send_selection_notify(xwl, xwl->selection_request.property);
}

static void xwl_send_timestamp(struct xwl *xwl) {
  xcb_change_property(xwl->connection, XCB_PROP_MODE_REPLACE,
                      xwl->selection_request.requestor,
                      xwl->selection_request.property, XCB_ATOM_INTEGER, 32, 1,
                      &xwl->selection_timestamp);

  xwl_send_selection_notify(xwl, xwl->selection_request.property);
}

static void xwl_send_data(struct xwl *xwl) {
  int p[2];

  if (!xwl->selection_data_offer || !xwl->selection_data_offer->utf8_text) {
    xwl_send_selection_notify(xwl, XCB_ATOM_NONE);
    return;
  }

  if (pipe2(p, O_CLOEXEC | O_NONBLOCK) == -1) {
    fprintf(stderr, "pipe2 failed: %m\n");
    xwl_send_selection_notify(xwl, XCB_ATOM_NONE);
    return;
  }

  wl_array_init(&xwl->selection_data);
  xwl->selection_data_offer_receive_fd = p[0];
  xwl->selection_data_ack_pending = 0;

  assert(!xwl->selection_event_source);
  xwl->selection_event_source = wl_event_loop_add_fd(
      wl_display_get_event_loop(xwl->host_display),
      xwl->selection_data_offer_receive_fd, WL_EVENT_READABLE,
      xwl_handle_selection_fd_readable, xwl);

  wl_data_offer_receive(xwl->selection_data_offer->internal,
                        "text/plain;charset=utf-8", p[1]);
  close(p[1]);
}

static void xwl_handle_selection_request(struct xwl *xwl,
                                         xcb_selection_request_event_t *event) {
  xwl->selection_request = *event;
  xwl->selection_incremental_transfer = 0;

  if (event->selection == xwl->atoms[ATOM_CLIPBOARD_MANAGER].value) {
    xwl_send_selection_notify(xwl, xwl->selection_request.property);
    return;
  }

  if (event->target == xwl->atoms[ATOM_TARGETS].value) {
    xwl_send_targets(xwl);
  } else if (event->target == xwl->atoms[ATOM_TIMESTAMP].value) {
    xwl_send_timestamp(xwl);
  } else if (event->target == xwl->atoms[ATOM_UTF8_STRING].value ||
             event->target == xwl->atoms[ATOM_TEXT].value) {
    xwl_send_data(xwl);
  } else {
    xwl_send_selection_notify(xwl, XCB_ATOM_NONE);
  }
}

static void
xwl_handle_xfixes_selection_notify(struct xwl *xwl,
                                   xcb_xfixes_selection_notify_event_t *event) {
  if (event->selection != xwl->atoms[ATOM_CLIPBOARD].value)
    return;

  if (event->owner == XCB_WINDOW_NONE) {
    // If client selection is gone. Set NULL selection for each seat.
    if (xwl->selection_owner != xwl->selection_window) {
      if (xwl->selection_data_device && xwl->default_seat) {
        wl_data_device_set_selection(xwl->selection_data_device, NULL,
                                     xwl->default_seat->seat->last_serial);
      }
    }
    xwl->selection_owner = XCB_WINDOW_NONE;
    return;
  }

  xwl->selection_owner = event->owner;

  // Save timestamp if it's our selection.
  if (event->owner == xwl->selection_window) {
    xwl->selection_timestamp = event->timestamp;
    return;
  }

  xwl->selection_incremental_transfer = 0;
  xcb_convert_selection(xwl->connection, xwl->selection_window,
                        xwl->atoms[ATOM_CLIPBOARD].value,
                        xwl->atoms[ATOM_TARGETS].value,
                        xwl->atoms[ATOM_WL_SELECTION].value, event->timestamp);
}

static int xwl_handle_x_connection_event(int fd, uint32_t mask, void *data) {
  struct xwl *xwl = (struct xwl *)data;
  xcb_generic_event_t *event;
  uint32_t count = 0;

  if ((mask & WL_EVENT_HANGUP) || (mask & WL_EVENT_ERROR))
    return 0;

  while ((event = xcb_poll_for_event(xwl->connection))) {
    switch (event->response_type & ~SEND_EVENT_MASK) {
    case XCB_CREATE_NOTIFY:
      xwl_handle_create_notify(xwl, (xcb_create_notify_event_t *)event);
      break;
    case XCB_DESTROY_NOTIFY:
      xwl_handle_destroy_notify(xwl, (xcb_destroy_notify_event_t *)event);
      break;
    case XCB_REPARENT_NOTIFY:
      xwl_handle_reparent_notify(xwl, (xcb_reparent_notify_event_t *)event);
      break;
    case XCB_MAP_REQUEST:
      xwl_handle_map_request(xwl, (xcb_map_request_event_t *)event);
      break;
    case XCB_MAP_NOTIFY:
      xwl_handle_map_notify(xwl, (xcb_map_notify_event_t *)event);
      break;
    case XCB_UNMAP_NOTIFY:
      xwl_handle_unmap_notify(xwl, (xcb_unmap_notify_event_t *)event);
      break;
    case XCB_CONFIGURE_REQUEST:
      xwl_handle_configure_request(xwl, (xcb_configure_request_event_t *)event);
      break;
    case XCB_CONFIGURE_NOTIFY:
      xwl_handle_configure_notify(xwl, (xcb_configure_notify_event_t *)event);
      break;
    case XCB_CLIENT_MESSAGE:
      xwl_handle_client_message(xwl, (xcb_client_message_event_t *)event);
      break;
    case XCB_FOCUS_IN:
      xwl_handle_focus_in(xwl, (xcb_focus_in_event_t *)event);
      break;
    case XCB_FOCUS_OUT:
      xwl_handle_focus_out(xwl, (xcb_focus_out_event_t *)event);
      break;
    case XCB_PROPERTY_NOTIFY:
      xwl_handle_property_notify(xwl, (xcb_property_notify_event_t *)event);
      break;
    case XCB_SELECTION_NOTIFY:
      xwl_handle_selection_notify(xwl, (xcb_selection_notify_event_t *)event);
      break;
    case XCB_SELECTION_REQUEST:
      xwl_handle_selection_request(xwl, (xcb_selection_request_event_t *)event);
      break;
    }

    switch (event->response_type - xwl->xfixes_extension->first_event) {
    case XCB_XFIXES_SELECTION_NOTIFY:
      xwl_handle_xfixes_selection_notify(
          xwl, (xcb_xfixes_selection_notify_event_t *)event);
      break;
    }

    free(event);
    ++count;
  }

  if ((mask & ~WL_EVENT_WRITABLE) == 0)
    xcb_flush(xwl->connection);

  return count;
}

static void xwl_connect(struct xwl *xwl) {
  const char wm_name[] = "WLWM";
  const xcb_setup_t *setup;
  xcb_screen_iterator_t screen_iterator;
  uint32_t values[1];
  xcb_void_cookie_t change_attributes_cookie, redirect_subwindows_cookie;
  xcb_generic_error_t *error;
  xcb_intern_atom_reply_t *atom_reply;
  xcb_depth_iterator_t depth_iterator;
  xcb_xfixes_query_version_reply_t *xfixes_query_version_reply;
  const xcb_query_extension_reply_t *composite_extension;
  unsigned i;

  xwl->connection = xcb_connect_to_fd(xwl->wm_fd, NULL);
  assert(!xcb_connection_has_error(xwl->connection));

  xcb_prefetch_extension_data(xwl->connection, &xcb_xfixes_id);
  xcb_prefetch_extension_data(xwl->connection, &xcb_composite_id);

  for (i = 0; i < ARRAY_SIZE(xwl->atoms); ++i) {
    const char *name = xwl->atoms[i].name;
    xwl->atoms[i].cookie =
        xcb_intern_atom(xwl->connection, 0, strlen(name), name);
  }

  setup = xcb_get_setup(xwl->connection);
  screen_iterator = xcb_setup_roots_iterator(setup);
  xwl->screen = screen_iterator.data;

  // Select for substructure redirect.
  values[0] = XCB_EVENT_MASK_STRUCTURE_NOTIFY |
              XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
              XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT;
  change_attributes_cookie = xcb_change_window_attributes(
      xwl->connection, xwl->screen->root, XCB_CW_EVENT_MASK, values);

  wl_event_loop_add_fd(wl_display_get_event_loop(xwl->host_display),
                       xcb_get_file_descriptor(xwl->connection),
                       WL_EVENT_READABLE, &xwl_handle_x_connection_event, xwl);

  xwl->xfixes_extension =
      xcb_get_extension_data(xwl->connection, &xcb_xfixes_id);
  assert(xwl->xfixes_extension->present);

  xfixes_query_version_reply = xcb_xfixes_query_version_reply(
      xwl->connection,
      xcb_xfixes_query_version(xwl->connection, XCB_XFIXES_MAJOR_VERSION,
                               XCB_XFIXES_MINOR_VERSION),
      NULL);
  assert(xfixes_query_version_reply);
  assert(xfixes_query_version_reply->major_version >= 5);
  free(xfixes_query_version_reply);

  composite_extension =
      xcb_get_extension_data(xwl->connection, &xcb_composite_id);
  assert(composite_extension->present);

  redirect_subwindows_cookie = xcb_composite_redirect_subwindows_checked(
      xwl->connection, xwl->screen->root, XCB_COMPOSITE_REDIRECT_MANUAL);

  // Another window manager should not be running.
  error = xcb_request_check(xwl->connection, change_attributes_cookie);
  assert(!error);

  // Redirecting subwindows of root for compositing should have succeeded.
  error = xcb_request_check(xwl->connection, redirect_subwindows_cookie);
  assert(!error);

  xwl->window = xcb_generate_id(xwl->connection);
  xcb_create_window(xwl->connection, 0, xwl->window, xwl->screen->root, 0, 0, 1,
                    1, 0, XCB_WINDOW_CLASS_INPUT_ONLY, XCB_COPY_FROM_PARENT, 0,
                    NULL);

  for (i = 0; i < ARRAY_SIZE(xwl->atoms); ++i) {
    atom_reply =
        xcb_intern_atom_reply(xwl->connection, xwl->atoms[i].cookie, &error);
    assert(!error);
    xwl->atoms[i].value = atom_reply->atom;
    free(atom_reply);
  }

  depth_iterator = xcb_screen_allowed_depths_iterator(xwl->screen);
  while (depth_iterator.rem > 0) {
    int depth = depth_iterator.data->depth;
    if (depth == xwl->screen->root_depth) {
      xwl->visual_ids[depth] = xwl->screen->root_visual;
      xwl->colormaps[depth] = xwl->screen->default_colormap;
    } else {
      xcb_visualtype_iterator_t visualtype_iterator =
          xcb_depth_visuals_iterator(depth_iterator.data);

      xwl->visual_ids[depth] = visualtype_iterator.data->visual_id;
      xwl->colormaps[depth] = xcb_generate_id(xwl->connection);
      xcb_create_colormap(xwl->connection, XCB_COLORMAP_ALLOC_NONE,
                          xwl->colormaps[depth], xwl->screen->root,
                          xwl->visual_ids[depth]);
    }
    xcb_depth_next(&depth_iterator);
  }
  assert(xwl->visual_ids[xwl->screen->root_depth]);

  if (xwl->clipboard_manager) {
    values[0] = XCB_EVENT_MASK_PROPERTY_CHANGE;
    xwl->selection_window = xcb_generate_id(xwl->connection);
    xcb_create_window(xwl->connection, XCB_COPY_FROM_PARENT,
                      xwl->selection_window, xwl->screen->root, 0, 0, 1, 1, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, xwl->screen->root_visual,
                      XCB_CW_EVENT_MASK, values);
    xcb_set_selection_owner(xwl->connection, xwl->selection_window,
                            xwl->atoms[ATOM_CLIPBOARD_MANAGER].value,
                            XCB_CURRENT_TIME);
    xcb_xfixes_select_selection_input(
        xwl->connection, xwl->selection_window,
        xwl->atoms[ATOM_CLIPBOARD].value,
        XCB_XFIXES_SELECTION_EVENT_MASK_SET_SELECTION_OWNER |
            XCB_XFIXES_SELECTION_EVENT_MASK_SELECTION_WINDOW_DESTROY |
            XCB_XFIXES_SELECTION_EVENT_MASK_SELECTION_CLIENT_CLOSE);
    xwl_set_selection(xwl, NULL);
  }

  xcb_change_property(xwl->connection, XCB_PROP_MODE_REPLACE, xwl->window,
                      xwl->atoms[ATOM_NET_SUPPORTING_WM_CHECK].value,
                      XCB_ATOM_WINDOW, 32, 1, &xwl->window);
  xcb_change_property(xwl->connection, XCB_PROP_MODE_REPLACE, xwl->window,
                      xwl->atoms[ATOM_NET_WM_NAME].value,
                      xwl->atoms[ATOM_UTF8_STRING].value, 8, strlen(wm_name),
                      wm_name);
  xcb_change_property(xwl->connection, XCB_PROP_MODE_REPLACE, xwl->screen->root,
                      xwl->atoms[ATOM_NET_SUPPORTING_WM_CHECK].value,
                      XCB_ATOM_WINDOW, 32, 1, &xwl->window);
  xcb_set_selection_owner(xwl->connection, xwl->window,
                          xwl->atoms[ATOM_WM_S0].value, XCB_CURRENT_TIME);

  xcb_set_input_focus(xwl->connection, XCB_INPUT_FOCUS_NONE, XCB_NONE,
                      XCB_CURRENT_TIME);
  xcb_flush(xwl->connection);
}

static int xwl_handle_sigchld(int signal_number, void *data) {
  struct xwl *xwl = (struct xwl *)data;
  int status;
  pid_t pid;

  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (pid == xwl->child_pid) {
      xwl->child_pid = 0;
      if (WIFEXITED(status) && WEXITSTATUS(status)) {
        fprintf(stderr, "Child exited with status: %d\n", WEXITSTATUS(status));
      }
      if (xwl->exit_with_child)
        kill(xwl->xwayland_pid, SIGTERM);
    } else if (pid == xwl->xwayland_pid) {
      xwl->xwayland_pid = 0;
      if (WIFEXITED(status) && WEXITSTATUS(status)) {
        fprintf(stderr, "Xwayland exited with status: %d\n",
                WEXITSTATUS(status));
        exit(WEXITSTATUS(status));
      }
      exit(0);
    }
  }

  return 1;
}

static void xwl_runprog(struct xwl *xwl) {
  pid_t pid = fork();
  assert(pid >= 0);
  if (pid == 0) {
    execvp(xwl->runprog[0], xwl->runprog);
    perror(xwl->runprog[0]);
    _exit(EXIT_FAILURE);
  }

  xwl->child_pid = pid;
}

static int xwl_handle_display_event(int fd, uint32_t mask, void *data) {
  struct xwl *xwl = (struct xwl *)data;
  char display_name[9];
  int bytes_read = 0;

  if (!(mask & WL_EVENT_READABLE))
    return 0;

  display_name[0] = ':';
  do {
    int bytes_left = sizeof(display_name) - bytes_read - 1;
    int bytes;

    if (!bytes_left)
      break;

    bytes = read(fd, &display_name[bytes_read + 1], bytes_left);
    if (!bytes)
      break;

    bytes_read += bytes;
  } while (display_name[bytes_read] != '\n');

  display_name[bytes_read] = '\0';
  setenv("DISPLAY", display_name, 1);

  setenv("XWL_VERSION", VERSION, 1);

  xwl_connect(xwl);

  wl_event_source_remove(xwl->display_event_source);
  xwl->display_event_source = NULL;
  close(fd);

  sd_notify(0, "READY=1");

  xwl_runprog(xwl);
  return 1;
}

static void xwl_usage() {
  printf("xwl-run "
         "[--scale=SCALE] "
         "[--app-id=ID] "
         "[--display=DISPLAY] "
         "[--no-exit-with-child] "
         "[--no-clipboard-manager] "
         "[--frame-color=COLOR] "
         "[--drm-device=DEVICE] "
         "[--glamor] "
         "PROGRAM [ARGS...]\n");
}

int main(int argc, char **argv) {
  struct xwl xwl = {
      .runprog = NULL,
      .display = NULL,
      .host_display = NULL,
      .client = NULL,
      .compositor = NULL,
      .shm = NULL,
      .shell = NULL,
      .data_device_manager = NULL,
      .xdg_shell = NULL,
      .aura_shell = NULL,
      .viewporter = NULL,
      .linux_dmabuf = NULL,
      .display_event_source = NULL,
      .sigchld_event_source = NULL,
      .wm_fd = -1,
      .drm_device = NULL,
      .xwayland_pid = -1,
      .child_pid = -1,
      .connection = NULL,
      .xfixes_extension = NULL,
      .screen = NULL,
      .window = 0,
      .host_focus_window = NULL,
      .needs_set_input_focus = 0,
      .scale = 1.0,
      .app_id = NULL,
      .exit_with_child = 1,
      .clipboard_manager = 1,
      .frame_color = 0,
      .has_frame_color = 0,
      .show_window_title = 0,
      .default_seat = NULL,
      .selection_window = XCB_WINDOW_NONE,
      .selection_owner = XCB_WINDOW_NONE,
      .selection_incremental_transfer = 0,
      .selection_request = {.requestor = XCB_NONE, .property = XCB_ATOM_NONE},
      .selection_timestamp = XCB_CURRENT_TIME,
      .selection_data_device = NULL,
      .selection_data_offer = NULL,
      .selection_data_source = NULL,
      .selection_data_source_send_fd = -1,
      .selection_send_event_source = NULL,
      .selection_property_reply = NULL,
      .selection_property_offset = 0,
      .selection_event_source = NULL,
      .selection_data_offer_receive_fd = -1,
      .selection_data_ack_pending = 0,
      .atoms =
          {
                  [ATOM_WM_S0] = {"WM_S0"},
                  [ATOM_WM_PROTOCOLS] = {"WM_PROTOCOLS"},
                  [ATOM_WM_STATE] = {"WM_STATE"},
                  [ATOM_WM_DELETE_WINDOW] = {"WM_DELETE_WINDOW"},
                  [ATOM_WM_TAKE_FOCUS] = {"WM_TAKE_FOCUS"},
                  [ATOM_WL_SURFACE_ID] = {"WL_SURFACE_ID"},
                  [ATOM_UTF8_STRING] = {"UTF8_STRING"},
                  [ATOM_MOTIF_WM_HINTS] = {"_MOTIF_WM_HINTS"},
                  [ATOM_NET_FRAME_EXTENTS] = {"_NET_FRAME_EXTENTS"},
                  [ATOM_NET_SUPPORTING_WM_CHECK] = {"_NET_SUPPORTING_WM_CHECK"},
                  [ATOM_NET_WM_NAME] = {"_NET_WM_NAME"},
                  [ATOM_NET_WM_MOVERESIZE] = {"_NET_WM_MOVERESIZE"},
                  [ATOM_NET_WM_STATE] = {"_NET_WM_STATE"},
                  [ATOM_NET_WM_STATE_FULLSCREEN] = {"_NET_WM_STATE_FULLSCREEN"},
                  [ATOM_NET_WM_STATE_MAXIMIZED_VERT] =
                      {"_NET_WM_STATE_MAXIMIZED_VERT"},
                  [ATOM_NET_WM_STATE_MAXIMIZED_HORZ] =
                      {"_NET_WM_STATE_MAXIMIZED_HORZ"},
                  [ATOM_CLIPBOARD] = {"CLIPBOARD"},
                  [ATOM_CLIPBOARD_MANAGER] = {"CLIPBOARD_MANAGER"},
                  [ATOM_TARGETS] = {"TARGETS"},
                  [ATOM_TIMESTAMP] = {"TIMESTAMP"}, [ATOM_TEXT] = {"TEXT"},
                  [ATOM_INCR] = {"INCR"},
                  [ATOM_WL_SELECTION] = {"_WL_SELECTION"},
          },
      .visual_ids = {0},
      .colormaps = {0}};
  const char *scale = getenv("XWL_SCALE");
  const char *clipboard_manager = getenv("XWL_CLIPBOARD_MANAGER");
  const char *frame_color = getenv("XWL_FRAME_COLOR");
  const char *show_window_title = getenv("XWL_SHOW_WINDOW_TITLE");
  const char *drm_device = getenv("XWL_DRM_DEVICE");
  const char *glamor = getenv("XWL_GLAMOR");
  struct wl_event_loop *event_loop;
  int sv[2], ds[2], wm[2];
  pid_t pid;
  int display = -1;
  int rv;
  int i;

  for (i = 1; i < argc; ++i) {
    const char *arg = argv[i];
    if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0 ||
        strcmp(arg, "-?") == 0) {
      xwl_usage();
      return 0;
    }
    if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0) {
      printf("Version: %s\n", VERSION);
      return 0;
    }
    if (strstr(arg, "--scale=") == arg) {
      const char *s = strchr(arg, '=');
      ++s;
      scale = s;
    } else if (strstr(arg, "--app-id=") == arg) {
      const char *s = strchr(arg, '=');
      ++s;
      xwl.app_id = s;
    } else if (strstr(arg, "--display=") == arg) {
      const char *s = strchr(arg, '=');
      ++s;
      display = atoi(s);
    } else if (strstr(arg, "--no-exit-with-child") == arg) {
      xwl.exit_with_child = 0;
    } else if (strstr(arg, "--no-clipboard-manager") == arg) {
      clipboard_manager = "0";
    } else if (strstr(arg, "--frame-color") == arg) {
      const char *s = strchr(arg, '=');
      ++s;
      frame_color = s;
    } else if (strstr(arg, "--show-window-title") == arg) {
      show_window_title = "1";
    } else if (strstr(arg, "--drm-device") == arg) {
      const char *s = strchr(arg, '=');
      ++s;
      drm_device = s;
    } else if (strstr(arg, "--glamor") == arg) {
      glamor = "1";
    } else if (arg[0] == '-') {
      if (strcmp(arg, "--") != 0) {
        fprintf(stderr, "Option `%s' is unknown.\n", arg);
        return 1;
      }
      xwl.runprog = &argv[i + 1];
      break;
    } else {
      xwl.runprog = &argv[i];
      break;
    }
  }

  if (!xwl.runprog || !xwl.runprog[0]) {
    xwl_usage();
    return 1;
  }

  if (scale)
    xwl.scale = MIN(MAX_SCALE, MAX(MIN_SCALE, atof(scale)));

  if (clipboard_manager)
    xwl.clipboard_manager = !!strcmp(clipboard_manager, "0");

  if (frame_color) {
    int r, g, b;
    if (sscanf(frame_color, "#%02x%02x%02x", &r, &g, &b) == 3) {
      xwl.frame_color = 0xff000000 | (r << 16) | (g << 8) | (b << 0);
      xwl.has_frame_color = 1;
    }
  }

  if (show_window_title)
    xwl.show_window_title = !!strcmp(show_window_title, "0");

  if (drm_device && !access(drm_device, 0))
    xwl.drm_device = drm_device;

  xwl.display = wl_display_connect(NULL);
  assert(xwl.display);

  wl_list_init(&xwl.outputs);
  wl_list_init(&xwl.seats);
  wl_list_init(&xwl.windows);
  wl_list_init(&xwl.unpaired_windows);

  xwl.host_display = wl_display_create();
  assert(xwl.host_display);

  event_loop = wl_display_get_event_loop(xwl.host_display);

  wl_event_loop_add_fd(event_loop, wl_display_get_fd(xwl.display),
                       WL_EVENT_READABLE, xwl_handle_event, &xwl);

  wl_registry_add_listener(wl_display_get_registry(xwl.display),
                           &xwl_registry_listener, &xwl);

  wl_display_roundtrip(xwl.display);

  if (!xwl.viewporter)
    xwl.scale = ceil(xwl.scale);

  xwl.sigchld_event_source =
      wl_event_loop_add_signal(event_loop, SIGCHLD, xwl_handle_sigchld, &xwl);

  // Wayland connection from Xwayland.
  rv = socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, sv);
  assert(!rv);

  xwl.client = wl_client_create(xwl.host_display, sv[0]);

  // Xwayland display ready socket.
  rv = socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, ds);
  assert(!rv);

  xwl.display_event_source = wl_event_loop_add_fd(
      event_loop, ds[0], WL_EVENT_READABLE, xwl_handle_display_event, &xwl);

  // X connection to Xwayland.
  rv = socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, wm);
  assert(!rv);

  xwl.wm_fd = wm[0];

  pid = fork();
  assert(pid != -1);
  if (pid == 0) {
    char fd_str[8], display_str[8], display_fd_str[8], wm_fd_str[8];
    char *args[32];
    int i = 0;
    int fd;

    // SOCK_CLOEXEC closes both ends, so we need to unset the flag on the
    // client fd.
    fd = dup(sv[1]);
    snprintf(fd_str, sizeof(fd_str), "%d", fd);
    setenv("WAYLAND_SOCKET", fd_str, 1);
    fd = dup(ds[1]);
    snprintf(display_fd_str, sizeof(display_fd_str), "%d", fd);
    fd = dup(wm[1]);
    snprintf(wm_fd_str, sizeof(wm_fd_str), "%d", fd);

    args[i++] = XWAYLAND_PATH "/Xwayland";
    if (display > 0) {
      snprintf(display_str, sizeof(display_str), ":%d", display);
      args[i++] = display_str;
    }
    args[i++] = "-nolisten";
    args[i++] = "tcp";
    args[i++] = "-rootless";
    if (xwl.drm_device) {
      // Use DRM and software rendering unless glamor is enabled.
      if (!glamor || !strcmp(glamor, "0"))
        args[i++] = "-drm";
    } else {
      args[i++] = "-shm";
    }
    args[i++] = "-displayfd";
    args[i++] = display_fd_str;
    args[i++] = "-wm";
    args[i++] = wm_fd_str;
    args[i++] = NULL;
    execvp(XWAYLAND_PATH "/Xwayland", args);
    perror("Xwayland");
    _exit(EXIT_FAILURE);
  }

  xwl.xwayland_pid = pid;

  close(sv[1]);
  close(wm[1]);

  do {
    wl_display_flush_clients(xwl.host_display);
    if (xwl.connection) {
      if (xwl.needs_set_input_focus) {
        xwl_set_input_focus(&xwl, xwl.host_focus_window);
        xwl.needs_set_input_focus = 0;
      }
      xcb_flush(xwl.connection);
    }
    wl_display_flush(xwl.display);
  } while (wl_event_loop_dispatch(event_loop, -1) != -1);

  return 0;
}
