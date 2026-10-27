#ifndef SCENE_H_
#define SCENE_H_

typedef struct scene_s
{
  void (*setup)(void);
  void (*update)(double delta_ms);
  void (*render)(double alpha_ms);
  void (*teardown)(void);
} scene_t;

extern scene_t SCENE_MENU, SCENE_LEVEL;

void scene_setup(void);

void scene_update(double delta_ms);

void scene_render(double alpha_ms);

void scene_teardown(void);

void scene_go_to(scene_t next);

#endif // SCENE_H_
