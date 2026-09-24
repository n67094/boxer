---
name: review
description: Code review for boxer (C99/SDL3 game library)
argument-hint: [header.h]
arguments: [header]
disable-model-invocation: true
context: fork
allowed-tools: Bash(git diff:_), Bash(make -n:_), Bash(cc:*), Read, Grep, Glob
---

# SKILL.md: review

Review C code in the boxer `src/` and `include/` directories, plus the `Makefile`.

## 1. Input

- `$header` find in `include` with it's source in `src` if available find them using glob.
- If `$header` is missing, review all changes in the current branch against `main`.
- If `$header` don't exists, stop and tell the user that the file don't exist.

## 2. Purpose

Review code changes for correctness, C safety, C99 conformance, project conventions, style and simplification.

- Point to each issue by file and line, and name what kind of issue it is.
- Explain clearly what is wrong and why it matters.
- Suggest a concrete fix.

Never modify files. Do not edit sources, do not run make targets that write build output into the tree, and do not run formatters in place. Only report findings with suggested fixes.

## 3. Workflow

1. Locate files
2. Read implementation
3. Reports findings

## 4. Checklist

- `BXR_ALLOC`, `BXR_NEW`, `BXR_CALLOC`, `BXR_RESIZE` must be followed by a NULL check.
- `BXR_FREE` must be used to free memory allocated by the above macros.
- `BXR_ASSERT` must be used to validate handles and module state.
- `BXR_LOG_ERROR` must be used for SDL failures, `BXR_LOG_WARN` for bad user input.
- Memory leaks and overflows.
- Use-after-free, double free and dangling pointers.

## 5. Output format

Group findings by severity, most severe first:

- **Critical:** crashes, memory corruption, undefined behavior, security issues.
- **Major:** resource leaks, wrong results, SDL3 misuse, broken builds, API breakage.
- **Minor:** portability, conformance, missing checks unlikely to trigger.
- **Nit:** style, naming, simplification.

For each finding format the output like this:

```
[Severity] path/to/file.c:LINE — Category

Problem: what is wrong.

Why: the consequence (what can happen at runtime or build time).

Fix:
    suggested code or diff
```
