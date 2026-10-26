/**
 * @file level.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef LEVEL_H_
#define LEVEL_H_

void level_setup(void);

void level_update(double delta_time_ms);

void level_render(double alpha);

void level_teardown(void);

#endif // LEVEL_H_
