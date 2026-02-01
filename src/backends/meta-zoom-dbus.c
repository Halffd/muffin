/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2026 Muffin Developers
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include "backends/meta-zoom-dbus.h"
#include "core/display-private.h"
#include "meta/meta-backend.h"
#include "meta-dbus-zoom.h"
#include "meta/main.h"

#define META_ZOOM_DBUS_SERVICE "org.cinnamon.Muffin.Zoom"
#define META_ZOOM_DBUS_PATH "/org/cinnamon/Muffin/Zoom"

G_DEFINE_TYPE (MetaZoomDBus, meta_zoom_dbus, G_TYPE_OBJECT)

static gboolean
handle_zoom_in (MetaDBusZoom          *skeleton,
                GDBusMethodInvocation *invocation,
                gpointer               user_data)
{
  MetaZoomDBus *zoom = user_data;
  if (zoom->display)
    {
      meta_display_a11y_zoom (zoom->display, TRUE);
      meta_dbus_zoom_complete_zoom_in (skeleton, invocation);
    }
  else
    {
      g_dbus_method_invocation_return_error (invocation,
                                            G_DBUS_ERROR,
                                            G_DBUS_ERROR_FAILED,
                                            "Display not available");
    }

  return TRUE;
}

static gboolean
handle_zoom_out (MetaDBusZoom          *skeleton,
                 GDBusMethodInvocation *invocation,
                 gpointer               user_data)
{
  MetaZoomDBus *zoom = user_data;
  if (zoom->display)
    {
      meta_display_a11y_zoom (zoom->display, FALSE);
      meta_dbus_zoom_complete_zoom_out (skeleton, invocation);
    }
  else
    {
      g_dbus_method_invocation_return_error (invocation,
                                            G_DBUS_ERROR,
                                            G_DBUS_ERROR_FAILED,
                                            "Display not available");
    }

  return TRUE;
}

static gboolean
handle_reset_zoom (MetaDBusZoom          *skeleton,
                   GDBusMethodInvocation *invocation,
                   gpointer               user_data)
{
  MetaZoomDBus *zoom = user_data;
  if (zoom->display)
    {
      meta_display_reset_a11y_zoom (zoom->display);
      meta_dbus_zoom_complete_reset_zoom (skeleton, invocation);
    }
  else
    {
      g_dbus_method_invocation_return_error (invocation,
                                            G_DBUS_ERROR,
                                            G_DBUS_ERROR_FAILED,
                                            "Display not available");
    }

  return TRUE;
}

static gboolean
handle_set_zoom_level (MetaDBusZoom          *skeleton,
                       GDBusMethodInvocation *invocation,
                       gdouble                level,
                       gpointer               user_data)
{
  MetaZoomDBus *zoom = user_data;
  if (zoom->display)
    {
      meta_display_set_zoom_level (zoom->display, level);
      meta_dbus_zoom_complete_set_zoom_level (skeleton, invocation);
    }
  else
    {
      g_dbus_method_invocation_return_error (invocation,
                                            G_DBUS_ERROR,
                                            G_DBUS_ERROR_FAILED,
                                            "Display not available");
    }

  return TRUE;
}

static gboolean
handle_get_zoom_level (MetaDBusZoom          *skeleton,
                       GDBusMethodInvocation *invocation,
                       gpointer               user_data)
{
  MetaZoomDBus *zoom = user_data;
  if (zoom->display)
    {
      gdouble level = meta_display_get_zoom_level (zoom->display);
      meta_dbus_zoom_complete_get_zoom_level (skeleton, invocation, level);
    }
  else
    {
      g_dbus_method_invocation_return_error (invocation,
                                            G_DBUS_ERROR,
                                            G_DBUS_ERROR_FAILED,
                                            "Display not available");
    }

  return TRUE;
}

static gboolean
handle_get_zoom_active (MetaDBusZoom          *skeleton,
                        GDBusMethodInvocation *invocation,
                        gpointer               user_data)
{
  MetaZoomDBus *zoom = user_data;
  if (zoom->display)
    {
      gboolean active = meta_display_get_zoom_active (zoom->display);
      meta_dbus_zoom_complete_get_zoom_active (skeleton, invocation, active);
    }
  else
    {
      g_dbus_method_invocation_return_error (invocation,
                                            G_DBUS_ERROR,
                                            G_DBUS_ERROR_FAILED,
                                            "Display not available");
    }

  return TRUE;
}

static gboolean
handle_toggle_zoom_mode (MetaDBusZoom          *skeleton,
                         GDBusMethodInvocation *invocation,
                         gpointer               user_data)
{
  MetaZoomDBus *zoom = user_data;
  if (zoom->display)
    {
      /* For now, just emit a debug message */
      meta_verbose ("Toggle zoom mode via DBus\n");
      meta_dbus_zoom_complete_toggle_zoom_mode (skeleton, invocation);
    }
  else
    {
      g_dbus_method_invocation_return_error (invocation,
                                            G_DBUS_ERROR,
                                            G_DBUS_ERROR_FAILED,
                                            "Display not available");
    }

  return TRUE;
}

static void
meta_zoom_dbus_init (MetaZoomDBus *zoom)
{
  zoom->skeleton = NULL;
  zoom->display = NULL;
}

MetaZoomDBus *
meta_zoom_dbus_new (MetaDisplay *display)
{
  MetaZoomDBus *zoom;

  zoom = g_object_new (META_TYPE_ZOOM_DBUS, NULL);
  zoom->display = display;
  zoom->skeleton = META_DBUS_ZOOM (meta_dbus_zoom_skeleton_new ());

  /* Connect to the D-Bus method calls */
  g_signal_connect (zoom->skeleton, "handle-zoom-in",
                    G_CALLBACK (handle_zoom_in), zoom);
  g_signal_connect (zoom->skeleton, "handle-zoom-out",
                    G_CALLBACK (handle_zoom_out), zoom);
  g_signal_connect (zoom->skeleton, "handle-reset-zoom",
                    G_CALLBACK (handle_reset_zoom), zoom);
  g_signal_connect (zoom->skeleton, "handle-set-zoom-level",
                    G_CALLBACK (handle_set_zoom_level), zoom);
  g_signal_connect (zoom->skeleton, "handle-get-zoom-level",
                    G_CALLBACK (handle_get_zoom_level), zoom);
  g_signal_connect (zoom->skeleton, "handle-get-zoom-active",
                    G_CALLBACK (handle_get_zoom_active), zoom);
  g_signal_connect (zoom->skeleton, "handle-toggle-zoom-mode",
                    G_CALLBACK (handle_toggle_zoom_mode), zoom);

  return zoom;
}

static void
on_bus_acquired (GDBusConnection *connection,
                 const char      *name,
                 gpointer         user_data)
{
  MetaZoomDBus *zoom = user_data;
  GDBusInterfaceSkeleton *interface_skeleton =
    G_DBUS_INTERFACE_SKELETON (zoom->skeleton);
  GError *error = NULL;

  if (!g_dbus_interface_skeleton_export (interface_skeleton,
                                         connection,
                                         META_ZOOM_DBUS_PATH,
                                         &error))
    {
      g_warning ("Failed to export zoom interface: %s\n", error->message);
    }
}

static void
on_name_acquired (GDBusConnection *connection,
                  const char      *name,
                  gpointer         user_data)
{
  meta_verbose ("Acquired name %s\n", name);
}

static void
on_name_lost (GDBusConnection *connection,
              const char      *name,
              gpointer         user_data)
{
  meta_verbose ("Lost or failed to acquire name %s\n", name);
}

static void
meta_zoom_dbus_constructed (GObject *object)
{
  MetaZoomDBus *zoom = META_ZOOM_DBUS (object);

  zoom->dbus_name_id =
    g_bus_own_name (G_BUS_TYPE_SESSION,
                    META_ZOOM_DBUS_SERVICE,
                    G_BUS_NAME_OWNER_FLAGS_ALLOW_REPLACEMENT |
                    (meta_get_replace_current_wm () ?
                     G_BUS_NAME_OWNER_FLAGS_REPLACE : 0),
                    on_bus_acquired,
                    on_name_acquired,
                    on_name_lost,
                    zoom,
                    NULL);

  G_OBJECT_CLASS (meta_zoom_dbus_parent_class)->constructed (object);
}

static void
meta_zoom_dbus_dispose (GObject *object)
{
  MetaZoomDBus *zoom = META_ZOOM_DBUS (object);

  if (zoom->dbus_name_id > 0)
    {
      g_bus_unown_name (zoom->dbus_name_id);
      zoom->dbus_name_id = 0;
    }

  g_clear_object (&zoom->skeleton);

  G_OBJECT_CLASS (meta_zoom_dbus_parent_class)->dispose (object);
}

static void
meta_zoom_dbus_class_init (MetaZoomDBusClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed = meta_zoom_dbus_constructed;
  object_class->dispose = meta_zoom_dbus_dispose;
}

/* Export the skeleton to the D-Bus connection */
void
meta_zoom_dbus_export (MetaZoomDBus *zoom,
                       GDBusConnection *connection)
{
  GError *error = NULL;

  if (!g_dbus_interface_skeleton_export (G_DBUS_INTERFACE_SKELETON (zoom->skeleton),
                                         connection,
                                         META_ZOOM_DBUS_PATH,
                                         &error))
    {
      g_warning ("Failed to export zoom interface: %s", error->message);
      g_error_free (error);
    }
}