<a name="line-1"></a>
File `bxr_sparse_set.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Sparse Set Data Structure

Sparse set allow insertion, deletion and lookup of hanldes (e.g. entity IDs)
in O(1) time complexity.

The sparse set is implemented as two arrays: a sparse array and a dense
array:

- `Sparse array`: store the indices of handles in the dense array.
- `Dense array`: store the actuale handles in a contiguous manner.

<a name="line-25"></a>
## 🔹 bxr_sparse_set_t *bxr_sparse_set_create(size_t default_capacity)

```cpp
bxr_sparse_set_t *bxr_sparse_set_create(size_t default_capacity)
```

Create a new sparse set with the given default capacity.

The sparse set will automatically resize if the handles inserted exceed the
current capacity.

`default_capacity` is the initial capacity of the sparse set (the max number
of handles that can be stored in the sparse set without resizing).

`return` a pointer to the created sparse set, or NULL if an error occurred.
Use `bxr_error_get` to get more information about the error.

The caller is responsible for destroying the returned sparse set using
`bxr_sparse_set_destroy` when it is no longer needed.

<a name="line-42"></a>
## 🔹 void bxr_sparse_set_destroy(bxr_sparse_set_t *set)

```cpp
void bxr_sparse_set_destroy(bxr_sparse_set_t *set)
```

Destroy the given sparse set and free its memory.

<a name="line-47"></a>
## 🔹 bool bxr_sparse_set_insert(bxr_sparse_set_t *set, size_t handle)

```cpp
bool bxr_sparse_set_insert(bxr_sparse_set_t *set, size_t handle)
```

Insert an handle into the sparse set.

`set` is the sparse set to insert into.

`handle` is the handle to insert into the sparse set.

`return` `true` if the element was successfully inserted, `false` if the
element already exists in the set or if an error occurred.

<a name="line-59"></a>
## 🔹 bool bxr_sparse_set_remove(bxr_sparse_set_t *set, size_t handle)

```cpp
bool bxr_sparse_set_remove(bxr_sparse_set_t *set, size_t handle)
```

Remove an element from the sparse set.

`set` is the sparse set to remove from.

'handle` is the handle to remove from the sparse set.

`return` `true` if the element was successfully removed, `false` if the
element does not exist in the set or if an error occurred.

<a name="line-71"></a>
## 🔹 bool bxr_sparse_set_contains(const bxr_sparse_set_t *set,

```cpp
bool bxr_sparse_set_contains(const bxr_sparse_set_t *set,
```

Check if an element exists in the sparse set.

`set` is the sparse set to check.

`handle` is the handle to check for existence in the sparse set.

`found` is an optional output parameter that will be set to the index of the
handle. Or UINT32_MAX if the handle does not exist in the set.

`return` `true` if the element exists in the set, `false` otherwise.

<a name="line-87"></a>
## 🔹 size_t bxr_sparse_set_capacity(const bxr_sparse_set_t *set)

```cpp
size_t bxr_sparse_set_capacity(const bxr_sparse_set_t *set)
```


`return` the capacity of the sparse set (the max number of handles that can
be stored in the sparse set without resizing).

<a name="line-94"></a>
## 🔹 size_t bxr_sparse_set_count(const bxr_sparse_set_t *set)

```cpp
size_t bxr_sparse_set_count(const bxr_sparse_set_t *set)
```

`return` the number of handles stored in the sparse set.

<a name="line-99"></a>
## 🔹 void bxr_sparse_set_clear(bxr_sparse_set_t *set)

```cpp
void bxr_sparse_set_clear(bxr_sparse_set_t *set)
```

Clear all handles from the sparse set (but keep the allocated memory for
future insertions).

<a name="line-105"></a>
## 🔹 size_t *bxr_sparse_set_get_dense(const bxr_sparse_set_t *set)

```cpp
size_t *bxr_sparse_set_get_dense(const bxr_sparse_set_t *set)
```

`return` a pointer to the dense array of the sparse set.
