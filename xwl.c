// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <aura-shell-client-protocol.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#define WL_HIDE_DEPRECATED
#include <wayland-client.h>
#include <wayland-server.h>
#include <xcb/composite.h>
#include <xcb/xcb.h>
#include <xdg-shell-unstable-v6-client-protocol.h>

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
    uint32_t contents_width;
    uint32_t contents_height;
    int is_cursor;
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
    uint32_t net_wm_moveresize_serial;
    struct wl_list link;
};

struct xwl_host_pointer {
    struct xwl_seat *seat;
    struct wl_resource *resource;
    struct wl_pointer *proxy;
};

struct xwl_host_keyboard {
    struct xwl_seat *seat;
    struct wl_resource *resource;
    struct wl_keyboard *proxy;
};

struct xwl_host_touch {
    struct xwl_seat *seat;
    struct wl_resource *resource;
    struct wl_touch *proxy;
};

struct xwl_host_seat {
    struct xwl_seat *seat;
    struct wl_resource *resource;
    struct wl_seat *proxy;
};

struct xwl_xdg_shell {
    struct xwl *xwl;
    uint32_t id;
    struct zxdg_shell_v6 *internal;
};

struct xwl_aura_shell {
    struct xwl *xwl;
    uint32_t id;
    struct zaura_shell *internal;
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
    uint32_t host_surface_id;
    int unpaired;
    int x;
    int y;
    int width;
    int height;
    int border_width;
    int mapped;
    int override_redirect;
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
    ATOM_WM_DELETE_WINDOW,
    ATOM_WL_SURFACE_ID,
    ATOM_UTF8_STRING,
    ATOM_MOTIF_WM_HINTS,
    ATOM_NET_SUPPORTING_WM_CHECK,
    ATOM_NET_WM_NAME,
    ATOM_NET_WM_MOVERESIZE,
    ATOM_NET_WM_STATE,
    ATOM_NET_WM_STATE_FULLSCREEN,
    ATOM_NET_WM_STATE_MAXIMIZED_VERT,
    ATOM_NET_WM_STATE_MAXIMIZED_HORZ,
    ATOM_LAST = ATOM_NET_WM_STATE_MAXIMIZED_HORZ,
};

struct xwl {
    char **runprog;
    struct wl_display *display;
    struct wl_display *host_display;
    struct wl_client *client;
    struct xwl_compositor* compositor;
    struct xwl_shm* shm;
    struct xwl_shell* shell;
    struct xwl_xdg_shell* xdg_shell;
    struct xwl_aura_shell* aura_shell;
    struct wl_list outputs;
    struct wl_list seats;
    struct wl_event_source *display_event_source;
    struct wl_event_source *sigchld_event_source;
    int wm_fd;
    pid_t xwayland_pid;
    pid_t child_pid;
    xcb_connection_t *connection;
    xcb_screen_t *screen;
    xcb_window_t window;
    struct wl_list windows, unpaired_windows;
    struct xwl_window *host_focus_window;
    xcb_window_t focus_window;
    int32_t scale;
    const char* app_id;
    struct xwl_host_seat *net_wm_moveresize_seat;
    union {
        const char *name;
        xcb_intern_atom_cookie_t cookie;
        xcb_atom_t value;
    } atoms[ATOM_LAST + 1];
};

enum {
    PROPERTY_WM_NAME,
    PROPERTY_WM_TRANSIENT_FOR,
    PROPERTY_MOTIF_WM_HINTS,
};

struct xwl_mwm_hints {
    uint32_t flags;
    uint32_t functions;
    uint32_t decorations;
    int32_t input_mode;
    uint32_t status;
};

#define MWM_HINTS_FUNCTIONS   (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)
#define MWM_HINTS_INPUT_MODE  (1L << 2)
#define MWM_HINTS_STATUS      (1L << 3)

#define MWM_DECOR_ALL         (1L << 0)
#define MWM_DECOR_BORDER      (1L << 1)
#define MWM_DECOR_RESIZEH     (1L << 2)
#define MWM_DECOR_TITLE       (1L << 3)
#define MWM_DECOR_MENU        (1L << 4)
#define MWM_DECOR_MINIMIZE    (1L << 5)
#define MWM_DECOR_MAXIMIZE    (1L << 6)

#define NET_WM_MOVERESIZE_SIZE_TOPLEFT     0
#define NET_WM_MOVERESIZE_SIZE_TOP         1
#define NET_WM_MOVERESIZE_SIZE_TOPRIGHT    2
#define NET_WM_MOVERESIZE_SIZE_RIGHT       3
#define NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT 4
#define NET_WM_MOVERESIZE_SIZE_BOTTOM      5
#define NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT  6
#define NET_WM_MOVERESIZE_SIZE_LEFT        7
#define NET_WM_MOVERESIZE_MOVE             8

#define NET_WM_STATE_REMOVE 0
#define NET_WM_STATE_ADD    1
#define NET_WM_STATE_TOGGLE 2

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static void
xwl_xdg_shell_ping(void *data,
                   struct zxdg_shell_v6 *xdg_shell,
                   uint32_t serial)
{
    zxdg_shell_v6_pong(xdg_shell, serial);
}

static const struct zxdg_shell_v6_listener xwl_xdg_shell_listener = {
    xwl_xdg_shell_ping
};

static void
xwl_send_configure_notify(struct xwl_window *window)
{
    xcb_configure_notify_event_t event;

    event.response_type = XCB_CONFIGURE_NOTIFY;
    event.event = window->id;
    event.window = window->id;
    event.above_sibling = XCB_WINDOW_NONE;
    event.x = window->x;
    event.y = window->y;
    event.width = window->width;
    event.height = window->height;
    event.border_width = window->border_width;
    event.override_redirect = 0;
    event.pad0 = 0;
    event.pad1 = 0;

    xcb_send_event(window->xwl->connection,
                   0,
                   window->id,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY,
                   (char *) &event);
}

static void
xwl_configure_window(struct xwl_window *window)
{
    assert(!window->pending_config.serial);

    if (window->next_config.mask) {
        int i = 0;

        xcb_configure_window(window->xwl->connection,
                             window->id,
                             window->next_config.mask,
                             window->next_config.values);
        if (window->next_config.mask & XCB_CONFIG_WINDOW_X)
            window->x = window->next_config.values[i++];
        if (window->next_config.mask & XCB_CONFIG_WINDOW_Y)
            window->y = window->next_config.values[i++];
        if (window->next_config.mask & XCB_CONFIG_WINDOW_WIDTH)
            window->width = window->next_config.values[i++];
        if (window->next_config.mask & XCB_CONFIG_WINDOW_HEIGHT)
            window->height = window->next_config.values[i++];
        if (window->next_config.mask & XCB_CONFIG_WINDOW_BORDER_WIDTH)
            window->border_width = window->next_config.values[i++];
    }

    xcb_change_property(window->xwl->connection,
                        XCB_PROP_MODE_REPLACE,
                        window->id,
                        window->xwl->atoms[ATOM_NET_WM_STATE].value,
                        XCB_ATOM_ATOM,
                        32,
                        window->next_config.states_length,
                        window->next_config.states);

    window->pending_config = window->next_config;
    window->next_config.serial = 0;
    window->next_config.mask = 0;
    window->next_config.states_length = 0;
}

static int
xwl_process_pending_configure_acks(struct xwl_window *window,
                                   struct xwl_host_surface *host_surface)
{
    if (!window->pending_config.serial)
        return 0;

    if (host_surface) {
        int width = window->width + window->border_width * 2;
        int height = window->height + window->border_width * 2;
        if (width != host_surface->contents_width ||
            height != host_surface->contents_height) {
          return 0;
        }
    }

    assert(window->xdg_surface);

    zxdg_surface_v6_ack_configure(window->xdg_surface,
                                  window->pending_config.serial);
    window->pending_config.serial = 0;

    if (window->next_config.serial)
        xwl_configure_window(window);

    return 1;
}

static void
xwl_xdg_surface_configure(void *data,
			  struct zxdg_surface_v6 *xdg_surface,
			  uint32_t serial)
{
    struct xwl_window *window = zxdg_surface_v6_get_user_data(xdg_surface);

    window->next_config.serial = serial;
    if (!window->pending_config.serial) {
        struct wl_resource *host_resource;
        struct xwl_host_surface *host_surface = NULL;

        host_resource = wl_client_get_object(window->xwl->client,
                                             window->host_surface_id);
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
    xwl_xdg_surface_configure
};

static void
xwl_xdg_toplevel_configure(void *data,
                           struct zxdg_toplevel_v6 *xdg_toplevel,
                           int32_t width,
                           int32_t height,
                           struct wl_array *states)
{
    struct xwl_window *window = zxdg_toplevel_v6_get_user_data(xdg_toplevel);
    uint32_t *state;
    int i = 0;

    if (width && height) {
        int32_t width_in_pixels = width * window->xwl->scale;
        int32_t height_in_pixels = height * window->xwl->scale;

        window->next_config.mask = XCB_CONFIG_WINDOW_X |
                                   XCB_CONFIG_WINDOW_Y |
                                   XCB_CONFIG_WINDOW_WIDTH |
                                   XCB_CONFIG_WINDOW_HEIGHT |
                                   XCB_CONFIG_WINDOW_BORDER_WIDTH;
        window->next_config.values[0] =
            window->xwl->screen->width_in_pixels / 2 - width_in_pixels / 2;
        window->next_config.values[1] =
            window->xwl->screen->height_in_pixels / 2 - height_in_pixels / 2;
        window->next_config.values[2] = width_in_pixels;
        window->next_config.values[3] = height_in_pixels;
        window->next_config.values[4] = 0;
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
    }
    window->next_config.states_length = i;
}

static void
xwl_xdg_toplevel_close(void *data,
                       struct zxdg_toplevel_v6 *xdg_toplevel)
{
    struct xwl_window *window = zxdg_toplevel_v6_get_user_data(xdg_toplevel);
    xcb_client_message_event_t event = {
        .response_type = XCB_CLIENT_MESSAGE,
        .format = 32,
        .window = window->id,
        .type = window->xwl->atoms[ATOM_WM_PROTOCOLS].value,
        .data.data32 = {
            window->xwl->atoms[ATOM_WM_DELETE_WINDOW].value,
            XCB_CURRENT_TIME,
        },
    };

    xcb_send_event(window->xwl->connection,
                   0,
                   window->id,
                   XCB_EVENT_MASK_NO_EVENT,
                   (const char *) &event);
}

static const struct zxdg_toplevel_v6_listener xwl_xdg_toplevel_listener = {
    xwl_xdg_toplevel_configure,
    xwl_xdg_toplevel_close
};

static void
xwl_xdg_popup_configure(void *data,
                        struct zxdg_popup_v6 *xdg_popup,
                        int32_t x,
                        int32_t y,
                        int32_t width,
                        int32_t height)
{
}

static void
xwl_xdg_popup_done(void *data,
                   struct zxdg_popup_v6 *zxdg_popup_v6)
{
}

static const struct zxdg_popup_v6_listener xwl_xdg_popup_listener = {
    xwl_xdg_popup_configure,
    xwl_xdg_popup_done
};

static void
xwl_window_update(struct xwl_window *window)
{
    struct wl_resource *host_resource = NULL;
    struct xwl_host_surface *host_surface;
    struct xwl *xwl = window->xwl;
    struct xwl_window *parent = NULL;
    xcb_window_t parent_window = 0;
    uint32_t frame_type = ZAURA_SURFACE_FRAME_TYPE_NORMAL;
    char* name = NULL;

    if (window->host_surface_id) {
        host_resource = wl_client_get_object(xwl->client,
                                             window->host_surface_id);
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

    if (!window->mapped) {
        if (window->xdg_popup) {
            zxdg_popup_v6_destroy(window->xdg_popup);
            window->xdg_popup = NULL;
        }
        if (window->xdg_toplevel) {
            zxdg_toplevel_v6_destroy(window->xdg_toplevel);
            window->xdg_toplevel = NULL;
        }
        if (window->xdg_surface) {
            zxdg_surface_v6_destroy(window->xdg_surface);
            window->xdg_surface = NULL;
        }
        if (window->aura_surface) {
            zaura_surface_destroy(window->aura_surface);
            window->aura_surface = NULL;
        }
        return;
    }

    if (window->xdg_surface)
        return;

    if (!host_resource)
        return;

    host_surface = wl_resource_get_user_data(host_resource);
    assert(host_surface);

    assert(xwl->xdg_shell);
    assert(xwl->xdg_shell->internal);

    if (window->override_redirect) {
        parent_window = xwl->focus_window;
        frame_type = ZAURA_SURFACE_FRAME_TYPE_SHADOW;
    } else {
        struct {
            int type;
            xcb_atom_t atom;
        } properties[] = {
            { PROPERTY_WM_NAME, XCB_ATOM_WM_NAME },
            { PROPERTY_WM_TRANSIENT_FOR, XCB_ATOM_WM_TRANSIENT_FOR },
            { PROPERTY_MOTIF_WM_HINTS, xwl->atoms[ATOM_MOTIF_WM_HINTS].value },
        };
        xcb_get_property_cookie_t cookies[ARRAY_SIZE(properties)];
        struct xwl_mwm_hints mwm_hints;
        int i;

        for (i = 0; i < ARRAY_SIZE(properties); ++i) {
            cookies[i] = xcb_get_property(xwl->connection,
                                          0,
                                          window->id,
                                          properties[i].atom,
                                          XCB_ATOM_ANY,
                                          0,
                                          2048);
        }

        for (i = 0; i < ARRAY_SIZE(properties); ++i)  {
            xcb_get_property_reply_t *reply =
                xcb_get_property_reply(xwl->connection, cookies[i], NULL);

            if (!reply)
                continue;

            if (reply->type == XCB_ATOM_NONE) {
                free(reply);
                continue;
            }

            switch (properties[i].type) {
                case PROPERTY_WM_NAME:
                    assert(!name);
                    name = strndup(xcb_get_property_value(reply),
                                   xcb_get_property_value_length(reply));
                    break;
                case PROPERTY_WM_TRANSIENT_FOR:
                    parent_window =
                        *((uint32_t *) xcb_get_property_value(reply));
                    break;
                case PROPERTY_MOTIF_WM_HINTS:
                    memcpy(&mwm_hints,
                           xcb_get_property_value(reply),
                           sizeof(mwm_hints));
                    if (mwm_hints.flags & MWM_HINTS_DECORATIONS) {
                        int decorate;

                        if (mwm_hints.decorations & MWM_DECOR_ALL)
                            decorate = ~mwm_hints.decorations & MWM_DECOR_TITLE;
                        else
                            decorate = mwm_hints.decorations & MWM_DECOR_TITLE;

                        if (!decorate)
                            frame_type = ZAURA_SURFACE_FRAME_TYPE_SHADOW;
                    }
                    break;
                default:
                    break;
            }
            free(reply);
        }
    }

    if (parent_window) {
        struct xwl_window *sibling;

        wl_list_for_each(sibling, &xwl->windows, link) {
            if (sibling->id == parent_window) {
                parent = sibling;
                break;
            }
        }
    }

    window->xdg_surface = zxdg_shell_v6_get_xdg_surface(
        xwl->xdg_shell->internal,
        host_surface->proxy);
    zxdg_surface_v6_set_user_data(window->xdg_surface, window);
    zxdg_surface_v6_add_listener(window->xdg_surface,
                                 &xwl_xdg_surface_listener,
                                 window);

    if (xwl->aura_shell) {
        window->aura_surface =
            zaura_shell_get_aura_surface(xwl->aura_shell->internal,
                                         host_surface->proxy);
        zaura_surface_set_frame(window->aura_surface, frame_type);
    }

    if (window->override_redirect) {
        struct zxdg_positioner_v6 *positioner;

        positioner = zxdg_shell_v6_create_positioner(xwl->xdg_shell->internal);
        assert(positioner);
        zxdg_positioner_v6_set_anchor(positioner,
                                      ZXDG_POSITIONER_V6_ANCHOR_TOP |
                                      ZXDG_POSITIONER_V6_ANCHOR_LEFT);
        zxdg_positioner_v6_set_gravity(positioner,
                                       ZXDG_POSITIONER_V6_GRAVITY_BOTTOM |
                                       ZXDG_POSITIONER_V6_GRAVITY_RIGHT);
        zxdg_positioner_v6_set_anchor_rect(positioner,
                                           (window->x - parent->x) / xwl->scale,
                                           (window->y - parent->y) / xwl->scale,
                                           1,
                                           1);

        window->xdg_popup = zxdg_surface_v6_get_popup(window->xdg_surface,
                                                      parent->xdg_surface,
                                                      positioner);
        zxdg_popup_v6_set_user_data(window->xdg_popup, window);
        zxdg_popup_v6_add_listener(window->xdg_popup,
                                   &xwl_xdg_popup_listener,
                                   window);

        zxdg_positioner_v6_destroy(positioner);
    } else {
        window->xdg_toplevel =
            zxdg_surface_v6_get_toplevel(window->xdg_surface);
        zxdg_toplevel_v6_set_user_data(window->xdg_toplevel, window);
        zxdg_toplevel_v6_add_listener(window->xdg_toplevel,
                                      &xwl_xdg_toplevel_listener,
                                      window);

        if (parent) {
            zxdg_toplevel_v6_set_parent(window->xdg_toplevel,
                                        parent->xdg_toplevel);
        }

        if (name)
            zxdg_toplevel_v6_set_title(window->xdg_toplevel, name);

        if (xwl->app_id)
            zxdg_toplevel_v6_set_app_id(window->xdg_toplevel, xwl->app_id);
    }

    if (name)
        free(name);

    host_surface->is_cursor = 0;
    wl_surface_commit(host_surface->proxy);
}

static void
xwl_host_surface_destroy(struct wl_client *client,
                         struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static void
xwl_host_surface_attach(struct wl_client *client,
                        struct wl_resource *resource,
                        struct wl_resource *buffer_resource,
                        int32_t x,
                        int32_t y)
{
    struct xwl_host_surface *host = wl_resource_get_user_data(resource);
    struct xwl_host_buffer *host_buffer = wl_resource_get_user_data(
        buffer_resource);
    struct wl_buffer *buffer_proxy = NULL;
    struct xwl_window *window;
    int32_t scale = host->xwl->scale;

    if (host_buffer) {
        host->contents_width = host_buffer->width;
        host->contents_height = host_buffer->height;
        buffer_proxy = host_buffer->proxy;
    }

    wl_surface_attach(host->proxy, buffer_proxy, x / scale, y / scale);
    wl_surface_set_buffer_scale(host->proxy, scale);

    wl_list_for_each(window, &host->xwl->windows, link) {
        if (window->host_surface_id == wl_resource_get_id(resource)) {
            while (xwl_process_pending_configure_acks(window, host))
                continue;

            break;
        }
    }
}

static void
xwl_host_surface_damage(struct wl_client *client,
                        struct wl_resource *resource,
                        int32_t x,
                        int32_t y,
                        int32_t width,
                        int32_t height)
{
    struct xwl_host_surface *host = wl_resource_get_user_data(resource);
    int32_t scale = host->xwl->scale;
    int32_t x1, y1, x2, y2;

    // Round to enclosing rect.
    x1 = x / scale;
    y1 = y / scale;
    x2 = (x + width + scale - 1) / scale;
    y2 = (y + height + scale - 1) / scale;

    wl_surface_damage(host->proxy, x1, y1, x2 - x1, y2 - y1);
}

static void
xwl_frame_callback_done(void *data,
                        struct wl_callback *callback,
                        uint32_t time)
{
    struct xwl_host_callback *host = wl_callback_get_user_data(callback);

    wl_callback_send_done(host->resource, time);
}

static const struct wl_callback_listener xwl_frame_callback_listener = {
    xwl_frame_callback_done
};

static void
xwl_host_callback_destroy(struct wl_resource *resource)
{
    struct xwl_host_callback *host = wl_resource_get_user_data(resource);

    wl_callback_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_host_surface_frame(struct wl_client *client,
                       struct wl_resource *resource,
                       uint32_t callback)
{
    struct xwl_host_surface *host = wl_resource_get_user_data(resource);
    struct xwl_host_callback *host_callback;

    host_callback = malloc(sizeof(*host_callback));
    assert(host_callback);

    host_callback->resource = wl_resource_create(client,
                                                 &wl_callback_interface,
                                                 1,
                                                 callback);
    wl_resource_set_implementation(host_callback->resource,
                                   NULL,
                                   host_callback,
                                   xwl_host_callback_destroy);
    host_callback->proxy = wl_surface_frame(host->proxy);
    wl_callback_set_user_data(host_callback->proxy, host_callback);
    wl_callback_add_listener(host_callback->proxy,
                             &xwl_frame_callback_listener,
                             host_callback);
}

static void
xwl_host_surface_set_opaque_region(struct wl_client *client,
                                   struct wl_resource *resource,
                                   struct wl_resource *region_resource)
{
    /* Not implemented */
    wl_resource_post_no_memory(resource);
}

static void
xwl_host_surface_set_input_region(struct wl_client *client,
                                  struct wl_resource *resource,
                                  struct wl_resource *region_resource)
{
    /* Not implemented */
    wl_resource_post_no_memory(resource);
}

static void
xwl_host_surface_commit(struct wl_client *client,
                        struct wl_resource *resource)
{
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
            }
            break;
        }
    }
}

static void
xwl_host_surface_set_buffer_transform(struct wl_client *client,
                                      struct wl_resource *resource,
                                      int32_t transform)
{
    /* Not implemented */
    wl_resource_post_no_memory(resource);
}

static void
xwl_host_surface_set_buffer_scale(struct wl_client *client,
                                  struct wl_resource *resource,
                                  int32_t scale)
{
    /* Not implemented */
    wl_resource_post_no_memory(resource);
}

#ifdef WL_SURFACE_DAMAGE_BUFFER_SINCE_VERSION
static void
xwl_host_surface_damage_buffer(struct wl_client *client,
                               struct wl_resource *resource,
                               int32_t x,
                               int32_t y,
                               int32_t width,
                               int32_t height)
{
    /* Not implemented */
    wl_resource_post_no_memory(resource);
}
#endif

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
#ifdef WL_SURFACE_DAMAGE_BUFFER_SINCE_VERSION
    xwl_host_surface_damage_buffer
#endif
};

static void
xwl_destroy_host_surface(struct wl_resource *resource)
{
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

    wl_surface_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_compositor_create_host_surface(struct wl_client *client,
                                   struct wl_resource *resource,
                                   uint32_t id)
{
    struct xwl_host_compositor *host = wl_resource_get_user_data(resource);
    struct xwl_host_surface *host_surface;
    struct xwl_window *window, *unpaired_window = NULL;

    host_surface = malloc(sizeof(*host_surface));
    assert(host_surface);

    host_surface->xwl = host->compositor->xwl;
    host_surface->contents_width = 0;
    host_surface->contents_height = 0;
    host_surface->is_cursor = 0;
    host_surface->resource = wl_resource_create(
        client,
        &wl_surface_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_implementation(host_surface->resource,
                                   &xwl_surface_implementation,
                                   host_surface,
                                   xwl_destroy_host_surface);
    host_surface->proxy = wl_compositor_create_surface(host->proxy);
    wl_surface_set_user_data(host_surface->proxy, host_surface);

    wl_list_for_each(window, &host->compositor->xwl->unpaired_windows, link) {
        if (window->host_surface_id == id) {
            unpaired_window = window;
            break;
        }
    }

    if (unpaired_window)
        xwl_window_update(window);
}

static void
xwl_compositor_create_host_region(struct wl_client *client,
                                  struct wl_resource *resource,
                                  uint32_t id)
{
    /* Not implemented */
    wl_resource_post_no_memory(resource);
}

static const struct wl_compositor_interface xwl_compositor_implementation = {
    xwl_compositor_create_host_surface,
    xwl_compositor_create_host_region
};

static void
xwl_destroy_host_compositor(struct wl_resource *resource)
{
    struct xwl_host_compositor *host = wl_resource_get_user_data(resource);

    wl_compositor_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_bind_host_compositor(struct wl_client *client,
                         void *data,
                         uint32_t version,
                         uint32_t id)
{
    struct xwl_compositor* compositor = (struct xwl_compositor *) data;
    struct xwl_host_compositor *host;

    host = malloc(sizeof(*host));
    assert(host);
    host->compositor = compositor;
    host->resource = wl_resource_create(client,
                                        &wl_compositor_interface,
                                        MIN(version, compositor->version),
                                        id);
    wl_resource_set_implementation(host->resource,
                                   &xwl_compositor_implementation,
                                   host,
                                   xwl_destroy_host_compositor);
    host->proxy = wl_registry_bind(
        wl_display_get_registry(compositor->xwl->display),
        compositor->id,
        &wl_compositor_interface,
        compositor->version);
    wl_compositor_set_user_data(host->proxy, host);
}

static void
xwl_host_buffer_destroy(struct wl_client *client,
                        struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static const struct wl_buffer_interface xwl_buffer_implementation = {
    xwl_host_buffer_destroy
};

static void
xwl_buffer_release(void *data,
                   struct wl_buffer *buffer)
{
    struct xwl_host_buffer *host = wl_buffer_get_user_data(buffer);

    wl_buffer_send_release(host->resource);
}

static const struct wl_buffer_listener xwl_buffer_listener = {
    xwl_buffer_release
};

static void
xwl_destroy_host_buffer(struct wl_resource *resource)
{
    struct xwl_host_buffer *host = wl_resource_get_user_data(resource);

    wl_buffer_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_host_shm_pool_create_host_buffer(struct wl_client *client,
                                     struct wl_resource *resource,
                                     uint32_t id,
                                     int32_t offset,
                                     int32_t width,
                                     int32_t height,
                                     int32_t stride,
                                     uint32_t format)
{
    struct xwl_host_shm_pool *host = wl_resource_get_user_data(resource);
    struct xwl_host_buffer *host_buffer;

    host_buffer = malloc(sizeof(*host_buffer));
    assert(host_buffer);

    host_buffer->width = width;
    host_buffer->height = height;
    host_buffer->resource = wl_resource_create(client,
                                               &wl_buffer_interface,
                                               1,
                                               id);
    wl_resource_set_implementation(host_buffer->resource,
                                   &xwl_buffer_implementation,
                                   host_buffer,
                                   xwl_destroy_host_buffer);
    host_buffer->proxy = wl_shm_pool_create_buffer(host->proxy,
                                                   offset,
                                                   width,
                                                   height,
                                                   stride,
                                                   format);
    wl_buffer_set_user_data(host_buffer->proxy, host_buffer);
    wl_buffer_add_listener(host_buffer->proxy,
                           &xwl_buffer_listener,
                           host_buffer);
}

static void
xwl_host_shm_pool_destroy(struct wl_client *client,
                          struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static void
xwl_host_shm_pool_resize(struct wl_client *client,
                         struct wl_resource *resource,
                         int32_t size)
{
    struct xwl_host_shm_pool *host = wl_resource_get_user_data(resource);

    wl_shm_pool_resize(host->proxy, size);
}

static const struct wl_shm_pool_interface xwl_shm_pool_implementation = {
    xwl_host_shm_pool_create_host_buffer,
    xwl_host_shm_pool_destroy,
    xwl_host_shm_pool_resize
};

static void
xwl_destroy_host_shm_pool(struct wl_resource *resource)
{
    struct xwl_host_shm_pool *host = wl_resource_get_user_data(resource);

    wl_shm_pool_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_shm_create_host_pool(struct wl_client *client,
                         struct wl_resource *resource,
                         uint32_t id,
                         int fd,
                         int32_t size)
{
    struct xwl_host_shm *host = wl_resource_get_user_data(resource);
    struct xwl_host_shm_pool *host_shm_pool;

    host_shm_pool = malloc(sizeof(*host_shm_pool));
    assert(host_shm_pool);

    host_shm_pool->resource = wl_resource_create(client,
                                                 &wl_shm_pool_interface,
                                                 1,
                                                 id);
    wl_resource_set_implementation(host_shm_pool->resource,
                                   &xwl_shm_pool_implementation,
                                   host_shm_pool,
                                   xwl_destroy_host_shm_pool);
    host_shm_pool->proxy = wl_shm_create_pool(host->proxy, fd, size);
    wl_shm_pool_set_user_data(host_shm_pool->proxy, host_shm_pool);

    close(fd);
}

static const struct wl_shm_interface xwl_shm_implementation = {
    xwl_shm_create_host_pool
};

static void
xwl_destroy_host_shm(struct wl_resource *resource)
{
    struct xwl_host_shm *host = wl_resource_get_user_data(resource);

    wl_shm_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_bind_host_shm(struct wl_client *client,
                  void *data,
                  uint32_t version,
                  uint32_t id)
{
    struct xwl_shm* shm = (struct xwl_shm *) data;
    struct xwl_host_shm *host;

    host = malloc(sizeof(*host));
    assert(host);
    host->shm = shm;
    host->resource = wl_resource_create(client,
                                        &wl_shm_interface,
                                        1,
                                        id);
    wl_resource_set_implementation(host->resource,
                                   &xwl_shm_implementation,
                                   host,
                                   xwl_destroy_host_shm);
    host->proxy = wl_registry_bind(wl_display_get_registry(shm->xwl->display),
                                   shm->id,
                                   &wl_shm_interface,
                                   wl_resource_get_version(host->resource));
    wl_shm_set_user_data(host->proxy, host);
}

static void
xwl_host_shell_get_host_shell_surface(struct wl_client *client,
                                      struct wl_resource *resource,
                                      uint32_t id,
                                      struct wl_resource *surface_resource)
{
    /* Not implemented */
    wl_resource_post_no_memory(resource);
}

static const struct wl_shell_interface xwl_shell_implementation = {
    xwl_host_shell_get_host_shell_surface
};

static void
xwl_destroy_host_shell(struct wl_resource *resource)
{
    struct xwl_host_shell *host = wl_resource_get_user_data(resource);

    wl_shell_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_bind_host_shell(struct wl_client *client,
                    void *data,
                    uint32_t version,
                    uint32_t id)
{
    struct xwl_shell* shell = (struct xwl_shell *) data;
    struct xwl_host_shell *host;

    host = malloc(sizeof(*host));
    assert(host);
    host->shell = shell;
    host->resource = wl_resource_create(client,
                                        &wl_shell_interface,
                                        1,
                                        id);
    wl_resource_set_implementation(host->resource,
                                   &xwl_shell_implementation,
                                   host,
                                   xwl_destroy_host_shell);
    host->proxy = wl_registry_bind(wl_display_get_registry(shell->xwl->display),
                                   shell->id,
                                   &wl_shell_interface,
                                   wl_resource_get_version(host->resource));
    wl_shell_set_user_data(host->proxy, host);
}

static void
xwl_output_geometry(void *data,
                    struct wl_output *output,
                    int x,
                    int y,
                    int physical_width,
                    int physical_height,
                    int subpixel,
                    const char *make,
                    const char *model,
                    int transform)
{
    struct xwl_host_output *host = wl_output_get_user_data(output);

    wl_output_send_geometry(host->resource,
                            x,
                            y,
                            physical_width,
                            physical_height,
                            subpixel,
                            make,
                            model,
                            transform);
}

static void
xwl_output_mode(void *data,
                struct wl_output *output,
                uint32_t flags,
                int width,
                int height,
                int refresh)
{
    struct xwl_host_output *host = wl_output_get_user_data(output);

    wl_output_send_mode(host->resource, flags, width, height, refresh);
}

static void
xwl_output_done(void *data,
                struct wl_output *output)
{
    struct xwl_host_output *host = wl_output_get_user_data(output);

    wl_output_send_done(host->resource);
}

static void
xwl_output_scale(void *data,
                 struct wl_output *output,
                 int32_t scale)
{
    struct xwl_host_output *host = wl_output_get_user_data(output);

    // Always 1 as device scale factor is emulated.
    wl_output_send_scale(host->resource, 1);
}

static const struct wl_output_listener xwl_output_listener = {
    xwl_output_geometry,
    xwl_output_mode,
    xwl_output_done,
    xwl_output_scale
};

static void
xwl_destroy_host_output(struct wl_resource *resource)
{
    struct xwl_host_output *host = wl_resource_get_user_data(resource);

    wl_output_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_bind_host_output(struct wl_client *client,
                     void *data,
                     uint32_t version,
                     uint32_t id)
{
    struct xwl_output* output = (struct xwl_output *) data;
    struct xwl_host_output *host;

    host = malloc(sizeof(*host));
    assert(host);
    host->output = output;
    host->resource = wl_resource_create(client,
                                        &wl_output_interface,
                                        MIN(version, output->version),
                                        id);
    wl_resource_set_implementation(host->resource,
                                   NULL,
                                   host,
                                   xwl_destroy_host_output);
    host->proxy = wl_registry_bind(
        wl_display_get_registry(output->xwl->display),
        output->id,
        &wl_output_interface,
        wl_resource_get_version(host->resource));
    wl_output_set_user_data(host->proxy, host);
    wl_output_add_listener(host->proxy, &xwl_output_listener, host);
}

static void
xwl_host_pointer_set_cursor(struct wl_client *client,
                            struct wl_resource *resource,
                            uint32_t serial,
                            struct wl_resource *surface_resource,
                            int32_t hotspot_x,
                            int32_t hotspot_y)
{
    struct xwl_host_pointer *host = wl_resource_get_user_data(resource);
    struct xwl_host_surface *host_surface = NULL;
    int32_t scale = host->seat->xwl->scale;

    if (surface_resource) {
        host_surface = wl_resource_get_user_data(surface_resource);
        host_surface->is_cursor = 1;
        if (host_surface->contents_width && host_surface->contents_width)
            wl_surface_commit(host_surface->proxy);
    }

    wl_pointer_set_cursor(host->proxy,
                          serial,
                          host_surface ? host_surface->proxy : NULL,
                          hotspot_x / scale,
                          hotspot_y / scale);
}

static void
xwl_host_pointer_release(struct wl_client *client,
                         struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static const struct wl_pointer_interface xwl_pointer_implementation = {
    xwl_host_pointer_set_cursor,
    xwl_host_pointer_release
};

static void
xwl_pointer_enter(void *data,
                  struct wl_pointer *pointer,
                  uint32_t serial,
                  struct wl_surface *surface,
                  wl_fixed_t x,
                  wl_fixed_t y)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);
    struct xwl_host_surface *host_surface = wl_surface_get_user_data(surface);
    int32_t scale = host->seat->xwl->scale;

    wl_pointer_send_enter(host->resource,
                          serial,
                          host_surface->resource,
                          x * scale,
                          y * scale);
}

static void
xwl_pointer_leave(void *data,
                  struct wl_pointer *pointer,
                  uint32_t serial,
                  struct wl_surface *surface)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);
    struct xwl_host_surface *host_surface =
        surface ? wl_surface_get_user_data(surface) : NULL;

    if (!host_surface)
        return;

    wl_pointer_send_leave(host->resource, serial, host_surface->resource);
}

static void
xwl_pointer_motion(void *data,
                   struct wl_pointer *pointer,
                   uint32_t time,
                   wl_fixed_t x,
                   wl_fixed_t y)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);
    int32_t scale = host->seat->xwl->scale;

    wl_pointer_send_motion(host->resource, time, x * scale, y * scale);
}

static void
xwl_pointer_button(void *data,
                   struct wl_pointer *pointer,
                   uint32_t serial,
                   uint32_t time,
                   uint32_t button,
                   uint32_t state)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

    wl_pointer_send_button(host->resource,
                           serial,
                           time,
                           button,
                           state);

    host->seat->net_wm_moveresize_serial = serial;
}

static void
xwl_pointer_axis(void *data,
                 struct wl_pointer *pointer,
                 uint32_t time,
                 uint32_t axis,
                 wl_fixed_t value)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);
    int32_t scale = host->seat->xwl->scale;

    wl_pointer_send_axis(host->resource, time, axis, value * scale);
}

#ifdef WL_POINTER_FRAME_SINCE_VERSION
static void
xwl_pointer_frame(void *data,
                  struct wl_pointer *pointer)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

    wl_pointer_send_frame(host->resource);
}
#endif

#ifdef WL_POINTER_AXIS_SOURCE_SINCE_VERSION
void
xwl_pointer_axis_source(void *data,
                        struct wl_pointer *pointer,
                        uint32_t axis_source)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

    wl_pointer_send_axis_source(host->resource, axis_source);
}
#endif

#ifdef WL_POINTER_AXIS_STOP_SINCE_VERSION
static void
xwl_pointer_axis_stop(void *data,
                      struct wl_pointer *pointer,
                      uint32_t time,
                      uint32_t axis)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

    wl_pointer_send_axis_stop(host->resource, time, axis);
}
#endif

#ifdef WL_POINTER_AXIS_DISCRETE_SINCE_VERSION
static void
xwl_pointer_axis_discrete(void *data,
                          struct wl_pointer *pointer,
                          uint32_t axis,
                          int32_t discrete)
{
    struct xwl_host_pointer *host = wl_pointer_get_user_data(pointer);

    wl_pointer_send_axis_discrete(host->resource, axis, discrete);
}
#endif

static const struct wl_pointer_listener xwl_pointer_listener = {
    xwl_pointer_enter,
    xwl_pointer_leave,
    xwl_pointer_motion,
    xwl_pointer_button,
    xwl_pointer_axis,
#ifdef WL_POINTER_FRAME_SINCE_VERSION
    xwl_pointer_frame,
#endif
#ifdef WL_POINTER_AXIS_SOURCE_SINCE_VERSION
    xwl_pointer_axis_source,
#endif
#ifdef WL_POINTER_AXIS_DISCRETE_SINCE_VERSION
    xwl_pointer_axis_stop,
#endif
#ifdef WL_POINTER_AXIS_DISCRETE_SINCE_VERSION
    xwl_pointer_axis_discrete
#endif
};

static void
xwl_host_keyboard_release(struct wl_client *client,
                          struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static const struct wl_keyboard_interface xwl_keyboard_implementation = {
    xwl_host_keyboard_release
};

static void
xwl_keyboard_keymap(void *data,
                    struct wl_keyboard *keyboard,
                    uint32_t format,
                    int32_t fd,
                    uint32_t size)
{
    struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

    wl_keyboard_send_keymap(host->resource, format, fd, size);

    close(fd);
}

static void
xwl_keyboard_enter(void *data,
                   struct wl_keyboard *keyboard,
                   uint32_t serial,
                   struct wl_surface *surface,
                   struct wl_array *keys)
{
    struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);
    struct xwl_host_surface *host_surface = wl_surface_get_user_data(surface);
    struct xwl *xwl = host->seat->xwl;
    struct xwl_window *window;

    wl_keyboard_send_enter(host->resource,
                           serial,
                           host_surface->resource,
                           keys);

    wl_list_for_each(window, &xwl->windows, link) {
        uint32_t host_resource_id = wl_resource_get_id(host_surface->resource);
        if (window->host_surface_id == host_resource_id) {
            uint32_t values[1];

            xwl->host_focus_window = window;

            if (window->override_redirect)
               return;

            xcb_set_input_focus(xwl->connection,
                                XCB_INPUT_FOCUS_NONE,
                                window->id,
                                XCB_CURRENT_TIME);
            values[0] = XCB_STACK_MODE_ABOVE;
            xcb_configure_window(xwl->connection,
                                 window->id,
                                 XCB_CONFIG_WINDOW_STACK_MODE,
                                 values);
            return;
        }
    }
}

static void
xwl_keyboard_leave(void *data,
                   struct wl_keyboard *keyboard,
                   uint32_t serial,
                   struct wl_surface *surface)
{
    struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);
    struct xwl_host_surface *host_surface =
        surface ? wl_surface_get_user_data(surface) : NULL;
    struct xwl *xwl = host->seat->xwl;

    if (!host_surface)
      return;

    wl_keyboard_send_leave(host->resource, serial, host_surface->resource);

    if (xwl->host_focus_window) {
        uint32_t host_resource_id = wl_resource_get_id(host_surface->resource);
        if (xwl->host_focus_window->host_surface_id == host_resource_id) {
            xwl->host_focus_window = NULL;
            xcb_set_input_focus(xwl->connection,
                                XCB_INPUT_FOCUS_NONE,
                                XCB_NONE,
                                XCB_CURRENT_TIME);
        }
    }
}

static void
xwl_keyboard_key(void *data,
                 struct wl_keyboard *keyboard,
                 uint32_t serial,
                 uint32_t time,
                 uint32_t key,
                 uint32_t state)
{
    struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

    wl_keyboard_send_key(host->resource,
                         serial,
                         time,
                         key,
                         state);
}

static void
xwl_keyboard_modifiers(void *data,
                       struct wl_keyboard *keyboard,
                       uint32_t serial,
                       uint32_t mods_depressed,
                       uint32_t mods_latched,
                       uint32_t mods_locked,
                       uint32_t group)
{
    struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

    wl_keyboard_send_modifiers(host->resource,
                               serial,
                               mods_depressed,
                               mods_latched,
                               mods_locked,
                               group);
}

#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
static void
xwl_keyboard_repeat_info(void *data,
                         struct wl_keyboard *keyboard,
                         int32_t rate,
                         int32_t delay)
{
    struct xwl_host_keyboard *host = wl_keyboard_get_user_data(keyboard);

    wl_keyboard_send_repeat_info(host->resource, rate, delay);
}
#endif

static const struct wl_keyboard_listener xwl_keyboard_listener = {
    xwl_keyboard_keymap,
    xwl_keyboard_enter,
    xwl_keyboard_leave,
    xwl_keyboard_key,
    xwl_keyboard_modifiers,
#ifdef WL_KEYBOARD_REPEAT_INFO_SINCE_VERSION
    xwl_keyboard_repeat_info
#endif
};

static void
xwl_host_touch_release(struct wl_client *client,
                       struct wl_resource *resource)
{
    wl_resource_destroy(resource);
}

static const struct wl_touch_interface xwl_touch_implementation = {
    xwl_host_touch_release
};

static void
xwl_host_touch_down(void *data,
                    struct wl_touch *touch,
                    uint32_t serial,
                    uint32_t time,
                    struct wl_surface *surface,
                    int32_t id,
                    wl_fixed_t x,
                    wl_fixed_t y)
{
    struct xwl_host_touch *host = wl_touch_get_user_data(touch);
    struct xwl_host_surface *host_surface = wl_surface_get_user_data(surface);
    int32_t scale = host->seat->xwl->scale;

    wl_touch_send_down(host->resource,
                       serial,
                       time,
                       host_surface->resource,
                       id,
                       x * scale,
                       y * scale);
}

static void
xwl_host_touch_up(void *data,
                  struct wl_touch *touch,
                  uint32_t serial,
                  uint32_t time,
                  int32_t id)
{
    struct xwl_host_touch *host = wl_touch_get_user_data(touch);

    wl_touch_send_up(host->resource, serial, time, id);
}

static void
xwl_host_touch_motion(void *data,
                      struct wl_touch *touch,
                      uint32_t time,
                      int32_t id,
                      wl_fixed_t x,
                      wl_fixed_t y)
{
    struct xwl_host_touch *host = wl_touch_get_user_data(touch);
    int32_t scale = host->seat->xwl->scale;

    wl_touch_send_motion(host->resource, time, id, x * scale, y * scale);
}

static void
xwl_host_touch_frame(void *data,
                     struct wl_touch *touch)
{
    struct xwl_host_touch *host = wl_touch_get_user_data(touch);

    wl_touch_send_frame(host->resource);
}

static void
xwl_host_touch_cancel(void *data,
                      struct wl_touch *touch)
{
    struct xwl_host_touch *host = wl_touch_get_user_data(touch);

    wl_touch_send_cancel(host->resource);
}

static const struct wl_touch_listener xwl_touch_listener = {
    xwl_host_touch_down,
    xwl_host_touch_up,
    xwl_host_touch_motion,
    xwl_host_touch_frame,
    xwl_host_touch_cancel
};

static void
xwl_destroy_host_pointer(struct wl_resource *resource)
{
    struct xwl_host_pointer *host = wl_resource_get_user_data(resource);

    wl_pointer_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_host_seat_get_host_pointer(struct wl_client *client,
                               struct wl_resource *resource,
                               uint32_t id)
{
    struct xwl_host_seat *host = wl_resource_get_user_data(resource);
    struct xwl_host_pointer *host_pointer;

    host_pointer = malloc(sizeof(*host_pointer));
    assert(host_pointer);

    host_pointer->seat = host->seat;
    host_pointer->resource = wl_resource_create(
        client,
        &wl_pointer_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_implementation(host_pointer->resource,
                                   &xwl_pointer_implementation,
                                   host_pointer,
                                   xwl_destroy_host_pointer);
    host_pointer->proxy = wl_seat_get_pointer(host->proxy);
    wl_pointer_set_user_data(host_pointer->proxy, host_pointer);
    wl_pointer_add_listener(host_pointer->proxy,
                            &xwl_pointer_listener,
                            host_pointer);
}

static void
xwl_destroy_host_keyboard(struct wl_resource *resource)
{
    struct xwl_host_keyboard *host = wl_resource_get_user_data(resource);

    wl_keyboard_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_host_seat_get_host_keyboard(struct wl_client *client,
                                struct wl_resource *resource,
                                uint32_t id)
{
    struct xwl_host_seat *host = wl_resource_get_user_data(resource);
    struct xwl_host_keyboard *host_keyboard;

    host_keyboard = malloc(sizeof(*host_keyboard));
    assert(host_keyboard);

    host_keyboard->seat = host->seat;
    host_keyboard->resource = wl_resource_create(
        client,
        &wl_keyboard_interface,
        wl_resource_get_version(resource),
        id);
    wl_resource_set_implementation(host_keyboard->resource,
                                   &xwl_keyboard_implementation,
                                   host_keyboard,
                                   xwl_destroy_host_keyboard);
    host_keyboard->proxy = wl_seat_get_keyboard(host->proxy);
    wl_keyboard_set_user_data(host_keyboard->proxy, host_keyboard);
    wl_keyboard_add_listener(host_keyboard->proxy,
                             &xwl_keyboard_listener,
                             host_keyboard);
}

static void
xwl_destroy_host_touch(struct wl_resource *resource)
{
    struct xwl_host_touch *host = wl_resource_get_user_data(resource);

    wl_touch_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_host_seat_get_host_touch(struct wl_client *client,
                             struct wl_resource *resource,
                             uint32_t id)
{
    struct xwl_host_seat *host = wl_resource_get_user_data(resource);
    struct xwl_host_touch *host_touch;

    host_touch = malloc(sizeof(*host_touch));
    assert(host_touch);

    host_touch->seat = host->seat;
    host_touch->resource = wl_resource_create(client,
                                              &wl_touch_interface,
                                              wl_resource_get_version(resource),
                                              id);
    wl_resource_set_implementation(host_touch->resource,
                                   &xwl_touch_implementation,
                                   host_touch,
                                   xwl_destroy_host_touch);
    host_touch->proxy = wl_seat_get_touch(host->proxy);
    wl_touch_set_user_data(host_touch->proxy, host_touch);
    wl_touch_add_listener(host_touch->proxy, &xwl_touch_listener, host_touch);
}

#ifdef WL_SEAT_RELEASE_SINCE_VERSION
static void
xwl_host_seat_release(struct wl_client *client,
                      struct wl_resource *resource)
{
    struct xwl_host_seat *host = wl_resource_get_user_data(resource);

    wl_seat_release(host->proxy);
}
#endif

static const struct wl_seat_interface xwl_seat_implementation = {
    xwl_host_seat_get_host_pointer,
    xwl_host_seat_get_host_keyboard,
    xwl_host_seat_get_host_touch,
#ifdef WL_SEAT_RELEASE_SINCE_VERSION
    xwl_host_seat_release
#endif
};

static void
xwl_seat_capabilities(void *data,
                      struct wl_seat *seat,
                      uint32_t capabilities)
{
    struct xwl_host_seat *host = wl_seat_get_user_data(seat);

    wl_seat_send_capabilities(host->resource, capabilities);
}

static void
xwl_seat_name(void *data,
              struct wl_seat *seat,
              const char *name)
{
    struct xwl_host_seat *host = wl_seat_get_user_data(seat);

    wl_seat_send_name(host->resource, name);
}

static const struct wl_seat_listener xwl_seat_listener = {
    xwl_seat_capabilities,
    xwl_seat_name
};

static void
xwl_destroy_host_seat(struct wl_resource *resource)
{
    struct xwl_host_seat *host = wl_resource_get_user_data(resource);

    if (host->seat->xwl->net_wm_moveresize_seat == host)
        host->seat->xwl->net_wm_moveresize_seat = NULL;

    wl_seat_destroy(host->proxy);
    wl_resource_set_user_data(resource, NULL);
    free(host);
}

static void
xwl_bind_host_seat(struct wl_client *client,
                   void *data,
                   uint32_t version,
                   uint32_t id)
{
    struct xwl_seat* seat = (struct xwl_seat *) data;
    struct xwl_host_seat *host;

    host = malloc(sizeof(*host));
    assert(host);
    host->seat = seat;
    host->resource = wl_resource_create(client,
                                        &wl_seat_interface,
                                        MIN(version, seat->version),
                                        id);
    wl_resource_set_implementation(host->resource,
                                   &xwl_seat_implementation,
                                   host,
                                   xwl_destroy_host_seat);
    host->proxy = wl_registry_bind(wl_display_get_registry(seat->xwl->display),
                                   seat->id,
                                   &wl_seat_interface,
                                   wl_resource_get_version(host->resource));
    wl_seat_set_user_data(host->proxy, host);
    wl_seat_add_listener(host->proxy, &xwl_seat_listener, host);

    seat->xwl->net_wm_moveresize_seat = host;
}

static void
xwl_registry_handler(void* data,
                     struct wl_registry *registry,
                     uint32_t id,
                     const char *interface,
                     uint32_t version)
{
    struct xwl *xwl = (struct xwl *) data;

    if (strcmp(interface, "wl_compositor") == 0) {
        struct xwl_compositor *compositor =
            malloc(sizeof(struct xwl_compositor));
        assert(compositor);
        compositor->xwl = xwl;
        compositor->id = id;
        assert(version >= 3);
        compositor->version = 3;
        compositor->host_global = wl_global_create(xwl->host_display,
                                                   &wl_compositor_interface,
                                                   compositor->version,
                                                   compositor,
                                                   xwl_bind_host_compositor);
        compositor->internal = wl_registry_bind(registry,
                                                id,
                                                &wl_compositor_interface,
                                                compositor->version);
        assert(!xwl->compositor);
        xwl->compositor = compositor;
    } else if (strcmp(interface, "wl_shm") == 0) {
        struct xwl_shm *shm = malloc(sizeof(struct xwl_shm));
        assert(shm);
        shm->xwl = xwl;
        shm->id = id;
        shm->host_global = wl_global_create(xwl->host_display,
                                            &wl_shm_interface,
                                            1,
                                            shm,
                                            xwl_bind_host_shm);
        assert(!xwl->shm);
        xwl->shm = shm;
    } else if (strcmp(interface, "wl_shell") == 0) {
        struct xwl_shell *shell = malloc(sizeof(struct xwl_shell));
        assert(shell);
        shell->xwl = xwl;
        shell->id = id;
        shell->host_global = wl_global_create(xwl->host_display,
                                              &wl_shell_interface,
                                              1,
                                              shell,
                                              xwl_bind_host_shell);
        assert(!xwl->shell);
        xwl->shell = shell;
    } else if (strcmp(interface, "wl_output") == 0) {
        struct xwl_output *output = malloc(sizeof(struct xwl_output));
        assert(output);
        output->xwl = xwl;
        output->id = id;
        output->version = MIN(2, version);
        output->host_global = wl_global_create(xwl->host_display,
                                               &wl_output_interface,
                                               output->version,
                                               output,
                                               xwl_bind_host_output);
        wl_list_insert(&xwl->outputs, &output->link);
    } else if (strcmp(interface, "wl_seat") == 0) {
        struct xwl_seat *seat = malloc(sizeof(struct xwl_seat));
        assert(seat);
        seat->xwl = xwl;
        seat->id = id;
#ifdef WL_POINTER_FRAME_SINCE_VERSION
        seat->version = MIN(5, version);
#else
        seat->version = MIN(3, version);
#endif
        seat->host_global = wl_global_create(xwl->host_display,
                                             &wl_seat_interface,
                                             seat->version,
                                             seat,
                                             xwl_bind_host_seat);
        seat->net_wm_moveresize_serial = 0;
        wl_list_insert(&xwl->seats, &seat->link);
    } else if (strcmp(interface, "zxdg_shell_v6") == 0) {
        struct xwl_xdg_shell *xdg_shell = malloc(sizeof(struct xwl_xdg_shell));
        assert(xdg_shell);
        xdg_shell->xwl = xwl;
        xdg_shell->id = id;
        xdg_shell->internal = wl_registry_bind(registry,
                                               id,
                                               &zxdg_shell_v6_interface,
                                               1);
        zxdg_shell_v6_add_listener(xdg_shell->internal,
                                   &xwl_xdg_shell_listener,
                                   NULL);
        assert(!xwl->xdg_shell);
        xwl->xdg_shell = xdg_shell;
    } else if (strcmp(interface, "zaura_shell") == 0) {
        struct xwl_aura_shell *aura_shell = malloc(sizeof(struct xwl_aura_shell));
        assert(aura_shell);
        aura_shell->xwl = xwl;
        aura_shell->id = id;
        aura_shell->internal = wl_registry_bind(registry,
                                                id,
                                                &zaura_shell_interface,
                                                1);
        assert(!xwl->aura_shell);
        xwl->aura_shell = aura_shell;
    }
}

static void
xwl_registry_remover(void *data,
                     struct wl_registry *registry,
                     uint32_t id)
{
    struct xwl *xwl = (struct xwl *) data;
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

    /* Not reached */
    assert(0);
}

static const struct wl_registry_listener xwl_registry_listener = {
    xwl_registry_handler,
    xwl_registry_remover
};

static int
xwl_handle_event(int fd,
                 uint32_t mask,
                 void *data)
{
    struct xwl *xwl = (struct xwl *) data;
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

static struct xwl_window *
xwl_lookup_window(struct xwl* xwl,
                  xcb_window_t id)
{
    struct xwl_window *window;

    wl_list_for_each(window, &xwl->windows, link) {
        if (window->id == id)
            return window;
    }
    wl_list_for_each(window, &xwl->unpaired_windows, link) {
        if (window->id == id)
            return window;
    }
    return NULL;
}

static void
xwl_handle_create_notify(struct xwl *xwl,
                         xcb_create_notify_event_t *event)
{
    struct xwl_window *window = malloc(sizeof(struct xwl_window));
    assert(window);
    window->xwl = xwl;
    window->id = event->window;
    window->host_surface_id = 0;
    window->unpaired = 1;
    window->x = event->x;
    window->y = event->y;
    window->width = event->width;
    window->height = event->height;
    window->border_width = event->border_width;
    window->mapped = 0;
    window->override_redirect = event->override_redirect;
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
    if (!event->override_redirect) {
        uint32_t values[1];

        values[0] = XCB_EVENT_MASK_PROPERTY_CHANGE |
                    XCB_EVENT_MASK_FOCUS_CHANGE;
        xcb_change_window_attributes(xwl->connection,
                                     window->id,
                                     XCB_CW_EVENT_MASK,
                                     values);
    }
}

static void
xwl_handle_destroy_notify(struct xwl *xwl,
                          xcb_destroy_notify_event_t *event)
{
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);
    if (window) {
        if (xwl->host_focus_window == window)
            xwl->host_focus_window = NULL;
        if (xwl->focus_window == event->window)
            xwl->focus_window = 0;
        if (window->xdg_surface)
            zxdg_surface_v6_destroy(window->xdg_surface);
        if (window->xdg_toplevel)
            zxdg_toplevel_v6_destroy(window->xdg_toplevel);
        if (window->aura_surface)
            zaura_surface_destroy(window->aura_surface);
        wl_list_remove(&window->link);
        free(window);
    }
}

static void
xwl_handle_map_request(struct xwl *xwl,
                       xcb_map_request_event_t *event)
{
    xcb_map_window(xwl->connection, event->window);
}

static void
xwl_handle_map_notify(struct xwl *xwl,
                      xcb_map_notify_event_t *event)
{
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);
    if (window) {
        assert(!window->mapped);
        window->mapped = 1;
        xwl_window_update(window);
    }
}

static void
xwl_handle_unmap_notify(struct xwl *xwl,
                        xcb_unmap_notify_event_t *event)
{
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);
    if (window && window->mapped) {
        window->mapped = 0;
        xwl_window_update(window);
    }
}

static void
xwl_handle_configure_request(struct xwl *xwl,
                             xcb_configure_request_event_t *event)
{
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);
    int x = window->x;
    int y = window->y;
    int width = window->width;
    int height = window->height;

    // Ack configure events as satisfying request removes the guarantee
    // that matching contents will arrive.
    if (window && window->xdg_toplevel) {
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

    if (event->value_mask & XCB_CONFIG_WINDOW_WIDTH) {
        // Keep all managed windows centered horizontally.
        x = xwl->screen->width_in_pixels / 2 - event->width / 2;
        width = event->width;
    }
    if (event->value_mask & XCB_CONFIG_WINDOW_HEIGHT) {
        // Keep all managed windows centered vertically.
        y = xwl->screen->height_in_pixels / 2 - event->height / 2;
        height = event->height;
    }

    if (x != window->x ||
        y != window->y ||
        width != window->width ||
        height != window->height ||
        window->border_width) {
        uint32_t values[16];
        int i = 0;

        values[i++] = x;
        values[i++] = y;
        values[i++] = width;
        values[i++] = height;
        values[i++] = 0;
        xcb_configure_window(xwl->connection,
                             window->id,
                             XCB_CONFIG_WINDOW_X |
                             XCB_CONFIG_WINDOW_Y |
                             XCB_CONFIG_WINDOW_WIDTH |
                             XCB_CONFIG_WINDOW_HEIGHT |
                             XCB_CONFIG_WINDOW_BORDER_WIDTH,
                             values);
        window->x = x;
        window->y = y;
        window->width = width;
        window->height = height;
        window->border_width = 0;
    } else {
        xwl_send_configure_notify(window);
    }
}

static void
xwl_handle_configure_notify(struct xwl *xwl,
                            xcb_configure_notify_event_t *event)
{
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);
    if (!window)
        return;

    if (event->override_redirect) {
        window->x = event->x;
        window->y = event->y;
        window->width = event->width;
        window->height = event->height;
        window->border_width = event->border_width;
    }
}

static uint32_t
xwl_resize_edge(int net_wm_moveresize_size)
{
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

static void
xwl_handle_client_message(struct xwl *xwl,
                          xcb_client_message_event_t *event)
{
    if (event->type == xwl->atoms[ATOM_WL_SURFACE_ID].value) {
        struct xwl_window *window, *unpaired_window = NULL;

        wl_list_for_each(window, &xwl->unpaired_windows, link) {
            if (window->id == event->window) {
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
            struct xwl_host_seat *seat = window->xwl->net_wm_moveresize_seat;

            if (!seat)
                return;

            if (event->data.data32[2] == NET_WM_MOVERESIZE_MOVE) {
                zxdg_toplevel_v6_move(window->xdg_toplevel,
                                      seat->proxy,
                                      seat->seat->net_wm_moveresize_serial);
            } else {
                uint32_t edge = xwl_resize_edge(event->data.data32[2]);

                if (edge == ZXDG_TOPLEVEL_V6_RESIZE_EDGE_NONE)
                    return;

                zxdg_toplevel_v6_resize(window->xdg_toplevel,
                                        seat->proxy,
                                        seat->seat->net_wm_moveresize_serial,
                                        edge);
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

static void
xwl_handle_focus_in(struct xwl *xwl, xcb_focus_in_event_t *event)
{
    xwl->focus_window = event->event;
}

static void
xwl_handle_focus_out(struct xwl *xwl, xcb_focus_out_event_t *event)
{
}

static void
xwl_handle_property_notify(struct xwl *xwl, xcb_property_notify_event_t *event)
{
    struct xwl_window *window = xwl_lookup_window(xwl, event->window);
    if (!window)
        return;

    if (event->atom == XCB_ATOM_WM_NAME) {
        char *name = NULL;

        if (!window->xdg_toplevel)
            return;

        if (event->state != XCB_PROPERTY_DELETE) {
            xcb_get_property_reply_t *reply =
                xcb_get_property_reply(xwl->connection,
                                       xcb_get_property(xwl->connection,
                                                        0,
                                                        window->id,
                                                        XCB_ATOM_WM_NAME,
                                                        XCB_ATOM_ANY,
                                                        0,
                                                        2048),
                                       NULL);
            if (reply) {
                name = strndup(xcb_get_property_value(reply),
                               xcb_get_property_value_length(reply));
                free(reply);
            }
        }

        if (name) {
            zxdg_toplevel_v6_set_title(window->xdg_toplevel, name);
            free(name);
        } else {
            zxdg_toplevel_v6_set_title(window->xdg_toplevel, "");
        }
    }
}

static int
xwl_handle_x_connection_event(int fd,
                              uint32_t mask,
                              void *data)
{
    struct xwl *xwl = (struct xwl *) data;
    xcb_generic_event_t *event;
    uint32_t count = 0;

    if ((mask & WL_EVENT_HANGUP) || (mask & WL_EVENT_ERROR))
        return 0;

    while ((event = xcb_poll_for_event(xwl->connection))) {
        switch (event->response_type & ~0x80) {
            case XCB_CREATE_NOTIFY:
                xwl_handle_create_notify(
                    xwl, (xcb_create_notify_event_t *) event);
                break;
            case XCB_DESTROY_NOTIFY:
                xwl_handle_destroy_notify(
                    xwl, (xcb_destroy_notify_event_t *) event);
                break;
            case XCB_MAP_REQUEST:
                xwl_handle_map_request(xwl,
                                       (xcb_map_request_event_t *) event);
                break;
            case XCB_MAP_NOTIFY:
                xwl_handle_map_notify(xwl,
                                      (xcb_map_notify_event_t *) event);
                break;
            case XCB_UNMAP_NOTIFY:
                xwl_handle_unmap_notify(xwl,
                                        (xcb_unmap_notify_event_t *) event);
                break;
            case XCB_CONFIGURE_REQUEST:
                xwl_handle_configure_request(
                    xwl, (xcb_configure_request_event_t *) event);
                break;
            case XCB_CONFIGURE_NOTIFY:
                xwl_handle_configure_notify(
                    xwl, (xcb_configure_notify_event_t *) event);
                break;
            case XCB_CLIENT_MESSAGE:
                xwl_handle_client_message(
                    xwl, (xcb_client_message_event_t *) event);
                break;
            case XCB_FOCUS_IN:
                xwl_handle_focus_in(xwl, (xcb_focus_in_event_t *) event);
                break;
            case XCB_FOCUS_OUT:
                xwl_handle_focus_out(xwl, (xcb_focus_out_event_t *) event);
                break;
            case XCB_PROPERTY_NOTIFY:
                xwl_handle_property_notify(
                    xwl, (xcb_property_notify_event_t *) event);
                break;
            default:
                break;
        }
        free(event);
        ++count;
    }

    if ((mask & ~WL_EVENT_WRITABLE) == 0)
        xcb_flush(xwl->connection);

    return count;
}

static void
xwl_connect(struct xwl *xwl)
{
    const char wm_name[] = "WLWM";
    const xcb_setup_t *setup;
    xcb_screen_iterator_t screen_iterator;
    uint32_t values[1];
    xcb_void_cookie_t change_attributes_cookie, redirect_subwindows_cookie;
    xcb_generic_error_t *error;
    xcb_intern_atom_reply_t *atom_reply;
    const xcb_query_extension_reply_t *composite_extension;
    unsigned i;

    xwl->connection = xcb_connect_to_fd(xwl->wm_fd, NULL);
    assert(!xcb_connection_has_error(xwl->connection));

    xcb_prefetch_extension_data(xwl->connection, &xcb_composite_id);

    for (i = 0; i < ARRAY_SIZE(xwl->atoms); ++i) {
        const char *name = xwl->atoms[i].name;
        xwl->atoms[i].cookie = xcb_intern_atom(xwl->connection,
                                               0,
                                               strlen(name),
                                               name);
    }

    setup = xcb_get_setup(xwl->connection);
    screen_iterator = xcb_setup_roots_iterator(setup);
    xwl->screen = screen_iterator.data;

    /* Select for substructure redirect. */
    values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT;
    change_attributes_cookie = xcb_change_window_attributes(xwl->connection,
                                                            xwl->screen->root,
                                                            XCB_CW_EVENT_MASK,
                                                            values);

    wl_event_loop_add_fd(wl_display_get_event_loop(xwl->host_display),
                         xcb_get_file_descriptor(xwl->connection),
                         WL_EVENT_READABLE,
                         &xwl_handle_x_connection_event,
                         xwl);

    composite_extension = xcb_get_extension_data(xwl->connection,
                                                 &xcb_composite_id);
    assert(composite_extension->present);

    redirect_subwindows_cookie = xcb_composite_redirect_subwindows_checked(
        xwl->connection, xwl->screen->root, XCB_COMPOSITE_REDIRECT_MANUAL);

    /* Another window manager should not be running */
    error = xcb_request_check(xwl->connection, change_attributes_cookie);
    assert(!error);

    /* Redirecting subwindows of root for compositing should have succeeded */
    error = xcb_request_check(xwl->connection, redirect_subwindows_cookie);
    assert(!error);

    xwl->window = xcb_generate_id(xwl->connection);
    xcb_create_window(xwl->connection, 0, xwl->window, xwl->screen->root,
                      0, 0, 1, 1, 0, XCB_WINDOW_CLASS_INPUT_ONLY,
                      XCB_COPY_FROM_PARENT, 0, NULL);

    for (i = 0; i < ARRAY_SIZE(xwl->atoms); ++i) {
        atom_reply = xcb_intern_atom_reply(xwl->connection,
                                           xwl->atoms[i].cookie,
                                           &error);
        assert(!error);
        xwl->atoms[i].value = atom_reply->atom;
        free(atom_reply);
    }

    xcb_change_property(xwl->connection,
                        XCB_PROP_MODE_REPLACE,
                        xwl->window,
                        xwl->atoms[ATOM_NET_SUPPORTING_WM_CHECK].value,
                        XCB_ATOM_WINDOW,
                        32,
                        1,
                        &xwl->window);
    xcb_change_property(xwl->connection,
                        XCB_PROP_MODE_REPLACE,
                        xwl->window,
                        xwl->atoms[ATOM_NET_WM_NAME].value,
                        xwl->atoms[ATOM_UTF8_STRING].value,
                        8,
                        strlen(wm_name),
                        wm_name);
    xcb_change_property(xwl->connection,
                        XCB_PROP_MODE_REPLACE,
                        xwl->screen->root,
                        xwl->atoms[ATOM_NET_SUPPORTING_WM_CHECK].value,
                        XCB_ATOM_WINDOW,
                        32,
                        1,
                        &xwl->window);
    xcb_set_selection_owner(xwl->connection,
                            xwl->window,
                            xwl->atoms[ATOM_WM_S0].value,
                            XCB_CURRENT_TIME);
    xcb_set_input_focus(xwl->connection,
                        XCB_INPUT_FOCUS_NONE,
                        XCB_NONE,
                        XCB_CURRENT_TIME);
    xcb_flush(xwl->connection);
}

static int
xwl_handle_sigchld(int signal_number, void *data)
{
    struct xwl *xwl = (struct xwl *) data;
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (pid == xwl->child_pid) {
            xwl->child_pid = 0;
            if (WIFEXITED(status) && WEXITSTATUS(status)) {
                fprintf(stderr,
                        "Child exited with status: %d\n",
                        WEXITSTATUS(status));
            }
            kill(xwl->xwayland_pid, SIGTERM);
        } else if (pid == xwl->xwayland_pid) {
            xwl->xwayland_pid = 0;
            if (WIFEXITED(status) && WEXITSTATUS(status)) {
                fprintf(stderr,
                        "Xwayland exited with status: %d\n",
                        WEXITSTATUS(status));
                exit(WEXITSTATUS(status));
            }
            exit(0);
        }
    }

    return 1;
}

static void
xwl_runprog(struct xwl *xwl)
{
    pid_t pid = fork();
    assert(pid >= 0);
    if (pid == 0) {
        execvp(xwl->runprog[0], xwl->runprog);
        perror(xwl->runprog[0]);
        _exit(EXIT_FAILURE);
    }

    xwl->child_pid = pid;
}

static int
xwl_handle_display_event(int fd,
                         uint32_t mask,
                         void *data)
{
    struct xwl *xwl = (struct xwl *) data;
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

    // OpenJDK AWT assume that WMs use reparenting unless this is set.
    setenv("_JAVA_AWT_WM_NONREPARENTING", "1", 1);

    xwl_connect(xwl);

    wl_event_source_remove(xwl->display_event_source);
    xwl->display_event_source = NULL;
    close(fd);

    xwl_runprog(xwl);
    return 1;
}

static void
xwl_usage() {
    printf("xwl-run [--scale=SCALE] [--app-id=ID] PROGRAM [ARGS...]\n");
}

int
main(int argc, char **argv)
{
    struct xwl xwl = {
        .runprog = NULL,
        .display = NULL,
        .host_display = NULL,
        .client = NULL,
        .compositor = NULL,
        .shm = NULL,
        .shell = NULL,
        .xdg_shell = NULL,
        .aura_shell = NULL,
        .display_event_source = NULL,
        .sigchld_event_source = NULL,
        .wm_fd = -1,
        .xwayland_pid = -1,
        .child_pid = -1,
        .connection = NULL,
        .screen = NULL,
        .window = 0,
        .host_focus_window = NULL,
        .focus_window = 0,
        .scale = 1,
        .app_id = NULL,
        .net_wm_moveresize_seat = NULL,
        .atoms = {
            [ATOM_WM_S0] = {"WM_S0"},
            [ATOM_WM_PROTOCOLS] = {"WM_PROTOCOLS"},
            [ATOM_WM_DELETE_WINDOW] = {"WM_DELETE_WINDOW"},
            [ATOM_WL_SURFACE_ID] = {"WL_SURFACE_ID"},
            [ATOM_UTF8_STRING] = {"UTF8_STRING"},
            [ATOM_MOTIF_WM_HINTS] = {"_MOTIF_WM_HINTS"},
            [ATOM_NET_SUPPORTING_WM_CHECK] = {"_NET_SUPPORTING_WM_CHECK"},
            [ATOM_NET_WM_NAME] = {"_NET_WM_NAME"},
            [ATOM_NET_WM_MOVERESIZE] = {"_NET_WM_MOVERESIZE"},
            [ATOM_NET_WM_STATE] = {"_NET_WM_STATE"},
            [ATOM_NET_WM_STATE_FULLSCREEN] = {"_NET_WM_STATE_FULLSCREEN"},
            [ATOM_NET_WM_STATE_MAXIMIZED_VERT] =
            {"_NET_WM_STATE_MAXIMIZED_VERT"},
            [ATOM_NET_WM_STATE_MAXIMIZED_HORZ] =
            {"_NET_WM_STATE_MAXIMIZED_HORZ"},
        }
    };
    struct wl_event_loop *event_loop;
    int sv[2], ds[2], wm[2];
    pid_t pid;
    int rv;
    int i;

    for (i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "--help") == 0 ||
            strcmp(arg, "-h") == 0 ||
            strcmp(arg, "-?") == 0) {
            xwl_usage();
            return 0;
        } else if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0) {
            printf("Version: %s\n", VERSION);
            return 0;
        } else if (strstr(arg, "--scale=") == arg) {
            const char *s = strchr(arg, '=');
            ++s;
            xwl.scale = atoi(s);
            assert(xwl.scale >= 1);
        } else if (strstr(arg, "--app-id=") == arg) {
            const char *s = strchr(arg, '=');
            ++s;
            xwl.app_id = s;
        } else if (arg[0] == '-') {
            if (strcmp(arg, "--") != 0) {
                fprintf (stderr, "Option `%s' is unknown.\n", arg);
                return 1;
            } else {
                xwl.runprog = &argv[i + 1];
                break;
            }
        } else {
            xwl.runprog = &argv[i];
            break;
        }
    }

    if (!xwl.runprog || !xwl.runprog[0]) {
        xwl_usage();
        return 1;
    }

    xwl.display = wl_display_connect(NULL);
    assert(xwl.display);

    wl_list_init(&xwl.outputs);
    wl_list_init(&xwl.seats);
    wl_list_init(&xwl.windows);
    wl_list_init(&xwl.unpaired_windows);

    xwl.host_display = wl_display_create();
    assert(xwl.host_display);

    event_loop = wl_display_get_event_loop(xwl.host_display);

    wl_event_loop_add_fd(event_loop,
                         wl_display_get_fd(xwl.display),
                         WL_EVENT_READABLE,
                         xwl_handle_event,
                         &xwl);

    wl_registry_add_listener(wl_display_get_registry(xwl.display),
                             &xwl_registry_listener,
                             &xwl);

    wl_display_roundtrip(xwl.display);

    xwl.sigchld_event_source = wl_event_loop_add_signal(event_loop,
                                                        SIGCHLD,
                                                        xwl_handle_sigchld,
                                                        &xwl);

    /* Wayland connection from Xwayland */
    rv = socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, sv);
    assert(!rv);

    xwl.client = wl_client_create(xwl.host_display, sv[0]);

    /* Xwayland display ready socket */
    rv = socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, ds);
    assert(!rv);

    xwl.display_event_source = wl_event_loop_add_fd(event_loop,
                                                    ds[0],
                                                    WL_EVENT_READABLE,
                                                    xwl_handle_display_event,
                                                    &xwl);

    /* X connection to Xwayland */
    rv = socketpair(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0, wm);
    assert(!rv);

    xwl.wm_fd = wm[0];

    pid = fork();
    assert(pid != -1);
    if (pid == 0) {
        char fd_str[8], display_fd_str[8], wm_fd_str[8];
        int fd;

        /* SOCK_CLOEXEC closes both ends, so we need to unset
         * the flag on the client fd. */
        fd = dup(sv[1]);
        snprintf(fd_str, sizeof(fd_str), "%d", fd);
        setenv("WAYLAND_SOCKET", fd_str, 1);
        fd = dup(ds[1]);
        snprintf(display_fd_str, sizeof(display_fd_str), "%d", fd);
        fd = dup(wm[1]);
        snprintf(wm_fd_str, sizeof(wm_fd_str), "%d", fd);

        execlp("Xwayland", "Xwayland",
               "-rootless",
               "-terminate",
               "-shm",
               "-displayfd", display_fd_str,
               "-wm", wm_fd_str,
               NULL);
        perror("Xwayland");
        _exit(EXIT_FAILURE);
    }

    xwl.xwayland_pid = pid;

    close(sv[1]);
    close(wm[1]);

    do {
        wl_display_flush_clients(xwl.host_display);
        if (xwl.connection)
            xcb_flush(xwl.connection);
        wl_display_flush(xwl.display);
    } while (wl_event_loop_dispatch(event_loop, -1) != -1);

    return 0;
}
