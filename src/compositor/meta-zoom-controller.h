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

#ifndef META_ZOOM_CONTROLLER_H
#define META_ZOOM_CONTROLLER_H

#include <glib-object.h>
#include <clutter/clutter.h>
#include "meta/types.h"

/* Forward declaration */
typedef struct _MetaLogicalMonitor MetaLogicalMonitor;

#define META_TYPE_ZOOM_CONTROLLER (meta_zoom_controller_get_type ())
G_DECLARE_FINAL_TYPE (MetaZoomController, meta_zoom_controller,
                      META, ZOOM_CONTROLLER, GObject)

MetaZoomController *meta_zoom_controller_new (MetaCompositor *compositor);

void meta_zoom_controller_set_zoom_level (MetaZoomController *controller,
                                         MetaLogicalMonitor *output,
                                         gdouble             zoom_level,
                                         guint               duration_ms);

void meta_zoom_controller_set_zoom_level_immediate (MetaZoomController *controller,
                                                   MetaLogicalMonitor *output,
                                                   gdouble             zoom_level);

gdouble meta_zoom_controller_get_zoom_level (MetaZoomController *controller,
                                            MetaLogicalMonitor *output);

void meta_zoom_controller_reset_zoom (MetaZoomController *controller,
                                     MetaLogicalMonitor *output,
                                     guint               duration_ms);

void meta_zoom_controller_reset_zoom_all (MetaZoomController *controller,
                                         guint               duration_ms);

#endif /* META_ZOOM_CONTROLLER_H */