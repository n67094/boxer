#include "bxr_ecs.h"

#include <SDL3/SDL.h>

struct bxr_ecs_s
{
  //
};

typedef struct _bxr_ecs_sparse_set_s
{
  Uint32 *sparse;
  Uint32 *dense;
  size_t count;
  size_t capacity;
} _bxr_ecs_sparse_set_t;

/**
 * Sparse set allow insertion, deletion and lookup of elements in O(1) time
 * complexity.
 */
static _bxr_ecs_sparse_set_t *
bxr_ecs_sparse_set_make(size_t capacity)
{
  // TODO
}

static void
bxr_ecs_sparse_set_destroy(_bxr_ecs_sparse_set_t *set)
{
  // TODO
}

static bool
bxr_ecs_sparse_set_insert(_bxr_ecs_sparse_set_t *set, Uint32 element)
{
  // TODO
  return false;
}

static bool
bxr_ecs_sparse_set_remove(_bxr_ecs_sparse_set_t *set, Uint32 element)
{
  // TODO
  return false;
}

static bool
bxr_ecs_sparse_set_contains(const _bxr_ecs_sparse_set_t *set, Uint32 element)
{
  // TODO
  return false;
}
