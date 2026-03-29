#include <SDL3/SDL.h>

#include "bxr_atom.h"
#include "bxr_config.h"
#include "bxr_mem.h"

typedef struct _bxr_atom_s _bxr_atom_t;

struct _bxr_atom_s
{
  _bxr_atom_t *link;
  size_t length; // str length
  char *str;
};

static _bxr_atom_t *_buckets[BXR_ATOM_MAX];

static size_t
_bxr_atom_hash(const char *str, size_t len)
{
  size_t hash = 5381;
  for (size_t i = 0; i < len; i++) {
    unsigned char c = (unsigned char)str[i];
    hash            = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  SDL_assert(BXR_ATOM_MAX > 0);

  return hash % BXR_ATOM_MAX;
}

const char *
bxr_atom_make(const char *str)
{
  SDL_assert(str);
  return bxr_atom_make_mem(str, SDL_strlen(str));
}

const char *
bxr_atom_make_mem(const char *str, size_t len)
{
  SDL_assert(str);
  SDL_assert(len >= 0);

  size_t hash = _bxr_atom_hash(str, len);

  // Try to find the string in the atom table
  // If found return it.
  _bxr_atom_t *atom = _buckets[hash];
  while (atom) {
    if (len == atom->length) {
      if (SDL_memcmp(str, atom->str, len) == 0) {
        return atom->str;
      }
    }

    atom = atom->link;
  }

  BXR_NEW(atom);

  atom->length = len;

  BXR_ALLOC(atom->str, len + 1);
  SDL_memcpy(atom->str, str, len);
  atom->str[len] = '\0';

  atom->link = _buckets[hash];

  _buckets[hash] = atom;

  return atom->str;
}

size_t
bxr_atom_length(const char *str)
{
  SDL_assert(str);

  size_t len  = SDL_strlen(str);
  size_t hash = _bxr_atom_hash(str, len);

  _bxr_atom_t *atom = _buckets[hash];
  while (atom) {
    if (atom->str == str)
      return atom->length;
    atom = atom->link;
  }

  // Fallback: full scan (handles embedded NULs/collisions)
  for (int i = 0; i < BXR_ATOM_MAX; ++i) {
    for (atom = _buckets[i]; atom; atom = atom->link) {
      if (atom->str == str)
        return atom->length;
    }
  }

  return 0;
}
