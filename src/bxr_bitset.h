/**
 * File `bxr_bitset.h`.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 *
 * # Bitset
 *
 * A bitset is a data structure that compactly stores a fixed number of bits,
 * where each bit can be either 0 or 1. Bitsets are often used for efficient
 * storage and manipulation of sets of boolean values, such as flags, feature
 * sets, or membership in a group.
 */

#ifndef BXR_BITSET_H_
#define BXR_BITSET_H_

#include <SDL3/SDL.h>

typedef struct bxr_bitset_s bxr_bitset_t;

/**
 * Create a new bitset with the specified number of bits.
 *
 * `bit_count` The number of bits in the bitset.
 *
 * `return` A pointer to the newly created bitset, or `NULL` if an error
 * occurs (e.g., if memory allocation fails).
 *
 * The caller is responsible for destroying the returned bitset using
 * `bxr_bitset_destroy` when it is no longer needed.
 */
bxr_bitset_t *bxr_bitset_make(size_t bit_count);

/**
 * Destroy a bitset and free its associated memory.
 *
 * `bitset` The bitset to destroy.
 */
void bxr_bitset_destroy(bxr_bitset_t *bitset);

/**
 * `return` `true` if all bits in the bitset are 0, `false` otherwise.
 */
bool bxr_bitset_is_zero(const bxr_bitset_t *bitset);

/**
 * `return` `true` if at least one bit in the bitset is 1, `false` otherwise.
 */
bool bxr_bitset_true(const bxr_bitset_t *bitset);

/**
 * `return` `true` if the bit at the specified index is 1, `false` otherwise.
 *
 * `index` The index of the bit to test (0-based).
 */
bool bxr_bitset_test(const bxr_bitset_t *bitset, size_t index);

/**
 * Set the bit at the specified index to 1.
 *
 * `index` The index of the bit to set (0-based).
 */
void bxr_bitset_set(bxr_bitset_t *bitset, size_t index);

/**
 * Set the bit at the specified index to 0.
 *
 * `index` The index of the bit to unset (0-based).
 */
void bxr_bitset_unset(bxr_bitset_t *bitset, size_t index);

/**
 * Perform a bitwise AND operation between two bitsets and return a new bitset
 * containing the result.
 *
 * `a` The first bitset.
 * `b` The second bitset.
 *
 * `return` A new bitset that is the result of the AND operation, or `NULL` if
 * an error occurs (e.g., if the bitsets have different sizes).
 *
 * The caller is responsible for destroying the returned bitset using
 * `bxr_bitset_destroy` when it is no longer needed.
 */
bxr_bitset_t *bxr_bitset_and(const bxr_bitset_t *a, const bxr_bitset_t *b);

/**
 * Perform a bitwise OR operation between two bitsets and return a new bitset
 * containing the result.
 *
 * `a` The first bitset.
 * `b` The second bitset.
 *
 * `return` A new bitset that is the result of the OR operation, or `NULL` if
 * an error occurs (e.g., if the bitsets have different sizes).
 *
 * The caller is responsible for destroying the returned bitset using
 * `bxr_bitset_destroy` when it is no longer needed.
 */
bxr_bitset_t *bxr_bitset_or(const bxr_bitset_t *a, const bxr_bitset_t *b);

/**
 * Perform a bitwise NOT operation on a bitset and return a new bitset
 * containing the result.
 *
 * `bitset` The bitset to negate.
 *
 * `return` A new bitset that is the result of the NOT operation, or `NULL` if
 * an error occurs.
 *
 * The caller is responsible for destroying the returned bitset using
 * `bxr_bitset_destroy` when it is no longer needed.
 */
bxr_bitset_t *bxr_bitset_not(const bxr_bitset_t *bitset);

/**
 * `return` `true` if two bitsets are equal (i.e., have the same bits set to 1),
 * `false` otherwise.
 *
 * `a` The first bitset.
 * `b` The second bitset.
 */
bool bxr_bitset_equals(const bxr_bitset_t *a, const bxr_bitset_t *b);

#endif // BXR_BITSET_H_
