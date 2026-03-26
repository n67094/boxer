#ifndef EXAMPLE_BLEND_H_
#define EXAMPLE_BLEND_H_

#include "bxr.h"

void example_blend_setup(void);

void example_blend_update(Uint64 delta_time_ms);

void example_blend_render(Uint64 alpha_time_ms);

void example_blend_shutdown(void);

#endif // EXAMPLE_BLEND_H_
