#include <SDL3/SDL.h>

#include "bxr_ecs.h"
#include "bxr_mem.h"

typedef Uint32 _bxr_ecs_id_t;

typedef struct _bxr_ecs_id_array_s
{
  size_t length;
  size_t count;
  _bxr_ecs_id_t *ids;
} _bxr_ecs_id_array_t;

typedef struct _bxr_ecs_component_array_s
{
  size_t length;
  size_t size; // Size of each component in bytes.
  void *components;
} _bxr_ecs_component_array_t;

typedef struct _bxr_ecs_entity_data_s
{

} _bxr_ecs_entity_data_t;

typedef struct _bxr_ecs_component_data_s
{

} _bxr_ecs_component_data_t;

typedef struct _bxr_ecs_system_data_s
{

} _bxr_ecs_system_data_t;

struct bxr_ecs_s
{
  _bxr_ecs_id_t *entity_free_stack;
  _bxr_ecs_id_t *entity_add_stack;
  _bxr_ecs_id_t *entity_remove_stack;
  _bxr_ecs_id_t *entity_destroy_stack;

  size_t entity_count;
  size_t next_entity_id;

  // TODO component data
  // TODO component arrays
  size_t component_count;

  // TODO system data
  size_t system_count;
};
