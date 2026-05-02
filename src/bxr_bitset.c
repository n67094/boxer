#include <SDL3/SDL.h>

#include "bxr_bitset.h"
#include "bxr_error.h"
#include "bxr_mem.h"

struct bxr_bitset_s
{
  Uint8 *bits;
  size_t count;
};

bxr_bitset_t *
bxr_bitset_create(size_t bit_count)
{
  bxr_bitset_t *bitset = NULL;
  BXR_NEW(bitset);
  if (!bitset) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    return NULL;
  }

  bitset->count = bit_count;

  size_t byte_count = (bit_count + 7) / 8; // Round up to the nearest byte
  BXR_CALLOC(bitset->bits, byte_count, sizeof(Uint8));
  if (!bitset->bits) {
    bxr_error_set(BXR_ERROR_OUT_OF_MEMORY);
    BXR_FREE(bitset);
    return NULL;
  }

  // Initialize all bits to 0
  BXR_MEMSET(bitset->bits, 0, byte_count * sizeof(Uint8));

  return bitset;
}

void
bxr_bitset_destroy(bxr_bitset_t *bitset)
{
  if (bitset) {
    if (bitset->bits) {
      BXR_FREE(bitset->bits);
    }
    BXR_FREE(bitset);
  }
}

bool
bxr_bitset_is_zero(const bxr_bitset_t *bitset)
{
  SDL_assert(bitset);

  size_t byte_count = (bitset->count + 7) / 8; // Round up to the nearest byte

  // Compare each byte to 0, if any byte is not 0, then the bitset is not zero.
  for (size_t i = 0; i < byte_count; i++) {
    if (bitset->bits[i] != 0) {
      return false;
    }
  }
  return true;
}

bool
bxr_bitset_true(const bxr_bitset_t *bitset)
{
  SDL_assert(bitset);

  size_t byte_count = (bitset->count + 7) / 8; // Round up to the nearest byte

  for (size_t i = 0; i < byte_count; i++) {
    if (bitset->bits[i]) {
      return true;
    }
  }

  return false;
}

bool
bxr_bitset_test(const bxr_bitset_t *bitset, size_t index)
{
  SDL_assert(bitset);
  SDL_assert(index < bitset->count);

  size_t byte_index = index / 8;
  size_t bit_index  = index % 8;

  return (bitset->bits[byte_index] & (1 << bit_index)) != 0;
}

void
bxr_bitset_set(bxr_bitset_t *bitset, size_t index)
{
  SDL_assert(bitset);
  SDL_assert(index < bitset->count);

  size_t byte_index = index / 8;
  size_t bit_index  = index % 8;

  bitset->bits[byte_index] |= (1 << bit_index);
}

void
bxr_bitset_unset(bxr_bitset_t *bitset, size_t index)
{
  SDL_assert(bitset);
  SDL_assert(index < bitset->count);

  size_t byte_index = index / 8;
  size_t bit_index  = index % 8;

  bitset->bits[byte_index] &= ~(1 << bit_index);
}

bxr_bitset_t *
bxr_bitset_and(const bxr_bitset_t *a, const bxr_bitset_t *b)
{
  SDL_assert(a);
  SDL_assert(b);
  SDL_assert(a->count == b->count);

  bxr_bitset_t *result = bxr_bitset_create(a->count);
  if (!result) {
    return NULL;
  }

  size_t byte_count = (a->count + 7) / 8; // Round up to the nearest byte

  for (size_t i = 0; i < byte_count; i++) {
    result->bits[i] = a->bits[i] & b->bits[i];
  }

  return result;
}

bxr_bitset_t *
bxr_bitset_or(const bxr_bitset_t *a, const bxr_bitset_t *b)
{
  SDL_assert(a);
  SDL_assert(b);
  SDL_assert(a->count == b->count);

  bxr_bitset_t *result = bxr_bitset_create(a->count);
  if (!result) {
    return NULL;
  }

  size_t byte_count = (a->count + 7) / 8; // Round up to the nearest byte

  for (size_t i = 0; i < byte_count; i++) {
    result->bits[i] = a->bits[i] | b->bits[i];
  }

  return result;
}

bxr_bitset_t *
bxr_bitset_not(const bxr_bitset_t *bitset)
{
  SDL_assert(bitset);

  bxr_bitset_t *result = bxr_bitset_create(bitset->count);
  if (!result) {
    return NULL;
  }

  size_t byte_count = (bitset->count + 7) / 8; // Round up to the nearest byte

  for (size_t i = 0; i < byte_count; i++) {
    result->bits[i] = ~(bitset->bits[i]);
  }

  return result;
}

bool
bxr_bitset_equal(const bxr_bitset_t *a, const bxr_bitset_t *b)
{
  SDL_assert(a);
  SDL_assert(b);
  SDL_assert(a->count == b->count);

  size_t byte_count = (a->count + 7) / 8; // Round up to the nearest byte

  for (size_t i = 0; i < byte_count; i++) {
    if (a->bits[i] != b->bits[i]) {
      return false;
    }
  }

  return true;
}

void
bxr_bitset_clear(bxr_bitset_t *bitset)
{
  SDL_assert(bitset);

  size_t byte_count = (bitset->count + 7) / 8; // Round up to the nearest byte

  for (size_t i = 0; i < byte_count; i++) {
    bitset->bits[i] = 0;
  }
}
