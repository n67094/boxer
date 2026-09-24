---
name: documentation
description: Code documentation for boxer (C99/SDL3 game library)
argument-hint: [header.h]
arguments: [header]
disable-model-invocation: true
context: fork
allowed-tools: Bash(gh:*), Read, Grep, Glob
---

# SKILL.md: documentation

Add or improve doc comments for every public function declared in one boxer header.

## 1. Input

- `$header` find in `include` with it's source in `src` if available find them using glob.
- If `$header` is missing, or no matching header exists, stop and tell the user the file was not found.

## 2. Purpose

Callers of boxer should be able to use any public function correctly by reading its header alone:

- What it does.
- What each parameter means.
- What it returns.
- Who owns returned memory.
- How errors are reported.

## 3. Workflow

1. Locate files
2. Read implementation
3. Prepare a branch named `doc-<file>` to merge into the current branch.
4. Write the documentation
5. Commit and ask validation by the user.

## 4. Rules

- Change comments only. Never modify code, signatures, or formatting outside comments, even if you spot a bug.
- Document public functions in the header. Do not document static functions in .c files.
- Keep existing correct comments; rewrite them only to match the format or fix inaccuracies.
- Be concise: one short sentence for the summary, details only when they add information.
- Don't restate the function name or the obvious ("width the width").
- If you spot a bug, an ambiguous behavior, over-complexity, or similar issue add a `FIXME` with an explaination on what to do.

## 5. Format

File-level comment (top of header, once):

```
/**
 * File `filename.h`.
 *
 * One-line summary of what this module provides.
 *
 * Optional extra context: main types, typical usage order.
 *
 * Copyright (c) 2026 nsix. All rights reserved.
 */
```

Function comment:

```
/**
 * Short description of the function.
 *
 * Optional extra details (behavior, side effects, preconditions).
 *
 * `param_name` what it is, valid range, whether NULL is allowed.
 * `other_param` what it is.
 *
 * `return` what it returns on success and on failure.
 *
 * **Note**: the caller must free the result with `bx_free_thing()`.
 * **Note**: on failure, call `SDL_GetError()` for details.
 */
```
