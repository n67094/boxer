/**
 * @file title.h
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef TITLE_H_
#define TITLE_H_

void title_setup(void);

void title_update(double delta_time_ms);

void title_render(double alpha);

void title_teardown(void);

#endif // TITLE_H_
