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

#pragma once

#include <glib-object.h>
#include "meta/types.h"
#include "meta-dbus-zoom.h"

G_BEGIN_DECLS

typedef struct _MetaZoomDBus MetaZoomDBus;
typedef struct _MetaZoomDBusClass MetaZoomDBusClass;

#define META_TYPE_ZOOM_DBUS (meta_zoom_dbus_get_type ())
#define META_ZOOM_DBUS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), META_TYPE_ZOOM_DBUS, MetaZoomDBus))
#define META_ZOOM_DBUS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), META_TYPE_ZOOM_DBUS, MetaZoomDBusClass))
#define META_IS_ZOOM_DBUS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), META_TYPE_ZOOM_DBUS))
#define META_IS_ZOOM_DBUS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), META_TYPE_ZOOM_DBUS))
#define META_ZOOM_DBUS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), META_TYPE_ZOOM_DBUS, MetaZoomDBusClass))

struct _MetaZoomDBus
{
  GObject parent;

  MetaDisplay *display;
  MetaDBusZoom *skeleton;
  int dbus_name_id;
};

struct _MetaZoomDBusClass
{
  GObjectClass parent_class;
};

GType meta_zoom_dbus_get_type (void);

MetaZoomDBus * meta_zoom_dbus_new (MetaDisplay *display);

void meta_zoom_dbus_export (MetaZoomDBus *zoom,
                            GDBusConnection *connection);

G_END_DECLS