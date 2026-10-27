/**
 * @file menu.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef MENU_H_
#define MENU_H_

#include "scene.h"

void menu_setup(void);

void menu_update(double delta_time_ms);

void menu_render(double alpha);

void menu_teardown(void);

scene_t menu_scene = {
  .setup    = menu_setup,
  .update   = menu_update,
  .render   = menu_render,
  .teardown = menu_teardown,
};

#endif // MENU_H_
