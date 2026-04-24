/**
 * File `bxr_ecs.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_ECS_H_
#define BXR_ECS_H_

#include <SDL3/SDL.h>

typedef Uint32 bxr_ecs_entity_t;
typedef Uint32 bxr_ecs_component_t;
typedef Uint32 bxr_ecs_system_t;
typedef Uint64 bxr_ecs_mask_t;

typedef struct bxr_ecs_desc_s
{
  Uint32 max_entity;
  Uint32 max_component;
  Uint32 max_system;
} bxr_ecs_desc_t;

typedef struct bxr_ecs_s bxr_ecs_t;

bxr_ecs_t *bxr_ecs_make(const bxr_ecs_desc_t *desc);

void bxr_ecs_destroy(bxr_ecs_t *ecs);

void bxr_ecs_reset(bxr_ecs_t *ecs);

typedef void (*bxr_ecs_component_make_cb)(bxr_ecs_t *ecs,
                                          bxr_ecs_entity_t entity,
                                          void *component_ptr,
                                          void *args);

typedef void (*bxr_ecs_component_destroy_cb)(bxr_ecs_t *ecs,
                                             bxr_ecs_entity_t entity,
                                             void *component_ptr);

bxr_ecs_component_t
bxr_ecs_register_component(bxr_ecs_t *ecs,
                           bxr_ecs_component_make_cb make_cb,
                           bxr_ecs_component_destroy_cb destroy_cb);

typedef int (*bxr_ecs_system_cb)(bxr_ecs_t *ecs,
                                 bxr_ecs_entity_t *entities,
                                 size_t entity_count,
                                 void *udata);

typedef void (*bxr_ecs_system_added_cb)(bxr_ecs_t *ecs,
                                        bxr_ecs_entity_t entity,
                                        void *udata);

typedef void (*bxr_ecs_system_removed_cb)(bxr_ecs_t *ecs,
                                          bxr_ecs_entity_t entity,
                                          void *udata);

bxr_ecs_system_t bxr_ecs_register_system(bxr_ecs_t *ecs,
                                         bxr_ecs_system_cb system_cb,
                                         bxr_ecs_system_added_cb added_cb,
                                         bxr_ecs_system_removed_cb removed_cb,
                                         void *udata);

void bxr_ecs_require_component_for_system(bxr_ecs_t *ecs,
                                          bxr_ecs_system_t sys,
                                          bxr_ecs_component_t comp);

void bxr_ecs_exclude_component_for_system(bxr_ecs_t *ecs,
                                          bxr_ecs_system_t sys,
                                          bxr_ecs_component_t comp);

void bxr_ecs_enable_system(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

void bxr_ecs_disable_system(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

void bxr_ecs_set_system_callback(bxr_ecs_t *ecs,
                                 bxr_ecs_system_t sys,
                                 bxr_ecs_system_cb system_cb,
                                 bxr_ecs_system_added_cb added_cb,
                                 bxr_ecs_system_removed_cb removed_cb);

void
bxr_ecs_set_system_userdata(bxr_ecs_t *ecs, bxr_ecs_system_t sys, void *udata);

void *bxr_ecs_get_system_userdata(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

void bxr_ecs_set_system_mask(bxr_ecs_t *ecs,
                             bxr_ecs_system_t sys,
                             bxr_ecs_mask_t mask);

bxr_ecs_mask_t bxr_ecs_get_system_mask(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

size_t bxr_ecs_get_system_entity_count(bxr_ecs_t *ecs, bxr_ecs_system_t sys);

bxr_ecs_entity_t *bxr_ecs_make_entity(bxr_ecs_t *ecs);

bool bxr_ecs_is_entity_ready(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity);

bool bxr_ecs_has_entity(const bxr_ecs_t *ecs, bxr_ecs_entity_t entity);

void bxr_ecs_destroy_entity(bxr_ecs_t *ecs, bxr_ecs_entity_t entity);

void *bxr_ecs_add_component_to_entity(bxr_ecs_t *ecs,
                                      bxr_ecs_entity_t entity,
                                      bxr_ecs_component_t component,
                                      void *args);

void *bxr_ecs_get_component_from_entity(const bxr_ecs_t *ecs,
                                        bxr_ecs_entity_t entity,
                                        bxr_ecs_component_t component);

void bxr_ecs_remove_component_from_entity(bxr_ecs_t *ecs,
                                          bxr_ecs_entity_t entity,
                                          bxr_ecs_component_t component);

int
bxr_ecs_run_system(bxr_ecs_t *ecs, bxr_ecs_system_t sys, bxr_ecs_mask_t mask);

int ecs_run_systems(bxr_ecs_t *ecs, bxr_ecs_mask_t mask);

#endif // BXR_ECS_H_
