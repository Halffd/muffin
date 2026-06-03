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

#include "meta-zoom-controller.h"
#include "compositor-private.h"
#include "meta/meta-backend.h"
#include "backends/meta-logical-monitor.h"

typedef struct _MetaZoomAnimation
{
  MetaLogicalMonitor *output;
  gdouble start_zoom;
  gdouble target_zoom;
  gdouble current_zoom;
  guint timeline_id;
  gboolean is_active;
} MetaZoomAnimation;

struct _MetaZoomController
{
  GObject parent;

  MetaCompositor *compositor;
  GHashTable *active_animations;  /* Key: MetaLogicalMonitor*, Value: MetaZoomAnimation* */
};

G_DEFINE_TYPE (MetaZoomController, meta_zoom_controller, G_TYPE_OBJECT)

static void
meta_zoom_animation_free (gpointer data)
{
  MetaZoomAnimation *animation = (MetaZoomAnimation *) data;

  if (animation)
    {
      /* The timeout is automatically removed when the callback returns G_SOURCE_REMOVE */
      animation->timeline_id = 0;
      g_free (animation);
    }
}

static void
meta_zoom_controller_finalize (GObject *object)
{
  MetaZoomController *controller = META_ZOOM_CONTROLLER (object);

  g_clear_pointer (&controller->active_animations, g_hash_table_unref);
  g_clear_object (&controller->compositor);

  G_OBJECT_CLASS (meta_zoom_controller_parent_class)->finalize (object);
}

static void
meta_zoom_controller_init (MetaZoomController *controller)
{
  controller->active_animations = g_hash_table_new_full (g_direct_hash,
                                                         g_direct_equal,
                                                         NULL,
                                                         meta_zoom_animation_free);
}

static void
meta_zoom_controller_class_init (MetaZoomControllerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = meta_zoom_controller_finalize;
}

MetaZoomController *
meta_zoom_controller_new (MetaCompositor *compositor)
{
  MetaZoomController *controller;

  g_return_val_if_fail (META_IS_COMPOSITOR (compositor), NULL);

  controller = g_object_new (META_TYPE_ZOOM_CONTROLLER, NULL);
  controller->compositor = g_object_ref (compositor);

  return controller;
}

typedef struct _AnimationStepData
{
  MetaZoomController *controller;
  MetaLogicalMonitor *output;
  guint timer_id;
} AnimationStepData;

static gboolean
zoom_animation_step (gpointer user_data)
{
  AnimationStepData *step_data = (AnimationStepData *) user_data;
  MetaZoomController *controller = step_data->controller;
  MetaLogicalMonitor *output = step_data->output;
  MetaZoomAnimation *animation;

  if (!controller || !output)
    {
      g_free (step_data);
      return G_SOURCE_REMOVE;
    }

  animation = g_hash_table_lookup (controller->active_animations, output);
  if (!animation || !animation->is_active)
    {
      g_free (step_data);
      return G_SOURCE_REMOVE;
    }

  // For this simplified implementation, we'll just complete the animation
  // In a real implementation, we would interpolate gradually over time
  meta_compositor_set_output_zoom (controller->compositor, output, animation->target_zoom);

  // Mark animation as complete
  animation->is_active = FALSE;
  animation->timeline_id = 0;

  g_free (step_data);
  return G_SOURCE_REMOVE;
}

static void
start_zoom_animation (MetaZoomController *controller,
                      MetaLogicalMonitor *output,
                      gdouble             start_zoom,
                      gdouble             target_zoom,
                      guint               duration_ms)
{
  MetaZoomAnimation *animation;
  AnimationStepData *step_data;

  // Get or create animation data
  animation = g_hash_table_lookup (controller->active_animations, output);
  if (!animation)
    {
      animation = g_new0 (MetaZoomAnimation, 1);
      animation->output = output;
      g_hash_table_insert (controller->active_animations, output, animation);
    }

  animation->start_zoom = start_zoom;
  animation->target_zoom = target_zoom;
  animation->current_zoom = start_zoom;
  animation->is_active = TRUE;

  // Create step data to pass to the timeout function
  step_data = g_new0 (AnimationStepData, 1);
  step_data->controller = controller;
  step_data->output = output;

  // Schedule the animation completion
  // In a real implementation, we would schedule multiple steps for smooth interpolation
  animation->timeline_id = clutter_threads_add_timeout (duration_ms,
                                                      zoom_animation_step,
                                                      step_data);
}

void
meta_zoom_controller_set_zoom_level (MetaZoomController *controller,
                                   MetaLogicalMonitor *output,
                                   gdouble             zoom_level,
                                   guint               duration_ms)
{
  gdouble current_level;

  g_return_if_fail (META_IS_ZOOM_CONTROLLER (controller));
  g_return_if_fail (META_IS_LOGICAL_MONITOR (output));
  g_return_if_fail (zoom_level >= 1.0 && zoom_level <= 16.0);

  // Get current zoom level for this output
  current_level = meta_compositor_get_output_zoom (controller->compositor, output);

  // Cancel any existing animation for this output
  MetaZoomAnimation *animation = g_hash_table_lookup (controller->active_animations, output);
  if (animation && animation->timeline_id > 0)
    {
      /* The timeout will be automatically removed when the callback returns G_SOURCE_REMOVE */
      animation->timeline_id = 0;
    }

  // If duration is 0, set zoom immediately
  if (duration_ms == 0)
    {
      meta_compositor_set_output_zoom (controller->compositor, output, zoom_level);
      return;
    }

  // Start the animated transition
  start_zoom_animation (controller, output, current_level, zoom_level, duration_ms);
}

void
meta_zoom_controller_set_zoom_level_immediate (MetaZoomController *controller,
                                             MetaLogicalMonitor *output,
                                             gdouble             zoom_level)
{
  g_return_if_fail (META_IS_ZOOM_CONTROLLER (controller));
  g_return_if_fail (META_IS_LOGICAL_MONITOR (output));

  // Cancel any animation for this output
  MetaZoomAnimation *animation = g_hash_table_lookup (controller->active_animations, output);
  if (animation && animation->timeline_id > 0)
    {
      /* The timeout will be automatically removed when the callback returns G_SOURCE_REMOVE */
      animation->timeline_id = 0;
    }

  // Set zoom immediately
  meta_compositor_set_output_zoom (controller->compositor, output, zoom_level);
}

gdouble
meta_zoom_controller_get_zoom_level (MetaZoomController *controller,
                                   MetaLogicalMonitor *output)
{
  g_return_val_if_fail (META_IS_ZOOM_CONTROLLER (controller), 1.0);
  g_return_val_if_fail (META_IS_LOGICAL_MONITOR (output), 1.0);

  return meta_compositor_get_output_zoom (controller->compositor, output);
}

void
meta_zoom_controller_reset_zoom (MetaZoomController *controller,
                               MetaLogicalMonitor *output,
                               guint               duration_ms)
{
  meta_zoom_controller_set_zoom_level (controller, output, 1.0, duration_ms);
}

void
meta_zoom_controller_reset_zoom_all (MetaZoomController *controller,
                                   guint               duration_ms)
{
  MetaBackend *backend;
  MetaMonitorManager *monitor_manager;
  GList *logical_monitors, *l;

  g_return_if_fail (META_IS_ZOOM_CONTROLLER (controller));

  backend = meta_get_backend ();
  monitor_manager = meta_backend_get_monitor_manager (backend);
  logical_monitors = meta_monitor_manager_get_logical_monitors (monitor_manager);

  for (l = logical_monitors; l; l = l->next)
    {
      MetaLogicalMonitor *logical_monitor = l->data;
      meta_zoom_controller_reset_zoom (controller, logical_monitor, duration_ms);
    }
}