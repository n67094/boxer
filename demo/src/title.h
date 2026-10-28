/**
 * @file title.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TITLE_H_
#define TITLE_H_

#include "scene.h"

void title_setup(void);

void title_update(double delta_time_ms);

void title_render(double alpha);

void title_teardown(void);

scene_t SCENE_TITLE = {
  .setup    = title_setup,
  .update   = title_update,
  .render   = title_render,
  .teardown = title_teardown,
};

#endif // TITLE_H_
