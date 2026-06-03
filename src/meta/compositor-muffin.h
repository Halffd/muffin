/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2008 Matthew Allum
 * Copyright (C) 2007 Iain Holmes
 * Based on xcompmgr - (c) 2003 Keith Packard
 *          xfwm4    - (c) 2005-2007 Olivier Fourdan
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

#ifndef MUFFIN_H_
#define MUFFIN_H_

#include "clutter/clutter.h"
#include "meta/compositor.h"
#include "meta/meta-window-actor.h"
#include "meta/types.h"

/* Forward declarations for opaque types */
typedef struct _MetaLogicalMonitor MetaLogicalMonitor;

/* Public compositor API */
META_EXPORT
ClutterActor *meta_get_stage_for_display            (MetaDisplay *display);

META_EXPORT
GList        *meta_get_window_actors                (MetaDisplay *display);

META_EXPORT
ClutterActor *meta_get_window_group_for_display     (MetaDisplay *display);

META_EXPORT
ClutterActor *meta_get_top_window_group_for_display (MetaDisplay *display);

META_EXPORT
ClutterActor *meta_get_bottom_window_group_for_display   (MetaDisplay *display);

META_EXPORT
ClutterActor *meta_get_feedback_group_for_display   (MetaDisplay *display);

META_EXPORT
void meta_disable_unredirect_for_display (MetaDisplay *display);

META_EXPORT
void meta_enable_unredirect_for_display  (MetaDisplay *display);

META_EXPORT
void meta_focus_stage_window       (MetaDisplay  *display,
                                    guint32       timestamp);

META_EXPORT
gboolean meta_stage_is_focused     (MetaDisplay  *display);

META_EXPORT
ClutterActor *meta_get_x11_background_actor_for_display (MetaDisplay *display);

META_EXPORT
ClutterActor *meta_get_desklet_container_for_display (MetaDisplay *display);

/* Per-view zoom API (backwards compatibility) */
gdouble meta_compositor_get_view_zoom (MetaCompositor   *compositor,
                                        ClutterStageView *view);

void meta_compositor_set_view_zoom (MetaCompositor   *compositor,
                                     ClutterStageView *view,
                                     gdouble           zoom);

gboolean meta_compositor_is_view_zoom_active (MetaCompositor *compositor);

/* Event coordinate transformation for per-view zoom */
void meta_compositor_transform_event_coordinates (MetaCompositor *compositor,
                                                   ClutterEvent   *event);

/* OUTPUT-SCOPED MAGNIFIER API (Stage 1)
 * 
 * New API for per-output zoom instead of per-view zoom.
 * Each output (MetaLogicalMonitor) can be zoomed independently.
 */
gdouble meta_compositor_get_output_zoom (MetaCompositor     *compositor,
                                          MetaLogicalMonitor *output);

void meta_compositor_set_output_zoom (MetaCompositor     *compositor,
                                       MetaLogicalMonitor *output,
                                       gdouble            zoom);

/* Internal helper for mapping views to logical monitors */
META_EXPORT
MetaLogicalMonitor *meta_compositor_get_logical_monitor_for_view (MetaCompositor   *compositor,
                                                                    ClutterStageView *view);

/* STAGE 2: Per-output damage isolation and scaling API
 * 
 * Functions for tracking and scaling damage per-output to avoid redraw storms.
 * When zooming, damage regions need to be scaled to match the zoomed rendering.
 */
void meta_compositor_update_damage_for_output (MetaCompositor     *compositor,
                                               MetaLogicalMonitor *output,
                                               const cairo_region_t *damage);

const cairo_region_t *
meta_compositor_get_damage_for_output (MetaCompositor     *compositor,
                                       MetaLogicalMonitor *output);




#endif

