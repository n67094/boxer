<a name="line-1"></a>
File `bxr_unit.h`.

Copyright (c) 2026 nsix. All rights reserved.

# Simple Unit Testing API

Example usage:

BXR_UNIT_CASE(test_case) { // Define case
  BXR_UNIT_ASSERT(1 + 1 == 2) // Assert something
  BXR_UNIT_ASSERT(...)
}

BXR_UNIT_SUITE(test_suite) { // Define suite
  BXR_UNIT_RUN_CASE(test_case) // Run case
  BXR_UNIT_RUN_CASE(...)
}

BXR_UNIT_RUN_SUITE(test_suite) // Run suite

<a name="line-28"></a>
Internal API

<a name="line-35"></a>
Asserts that an expression is true and reports the result to the console.

`passed` indicates whether the assertion passed or failed.

`expr` is the string representation of the expression being asserted.

`file` is the name of the source file where the assertion is located.

`line` is the line number in the source file where the assertion is located.

`return` true if the assertion passed, false if it failed.

```cpp
bool bxr_unit_assert(bool passed,
```

---

<a name="line-53"></a>
Runs a test case function and reports the result to the console.

`name` is the name of the test case function being run.

`case_cb` is the test case callback function to execute.

```cpp
void bxr_unit_run_case(const char *const name, bxr_unit_case_cb case_cb)
```

---

<a name="line-62"></a>
Runs a test suite function and reports the results to the console.

`name` is the name of the test suite function being run.

`suite_cb` is the test suite callback function to run.

```cpp
void bxr_unit_run_suite(const char *const name, bxr_unit_suite_cb suite_cb)
```

---

<a name="line-71"></a>
Public API

<a name="line-75"></a>
Runs a test suite function and reports the results to the console.

`suite_cb` is the test suite callback function to run.

```cpp
#define BXR_UNIT_RUN_SUITE(suite_cb) bxr_unit_run_suite(#suite_cb, suite_cb)
```

---

<a name="line-82"></a>
Defines a test suite function.

`name` is the name of the test suite function.

```cpp
#define BXR_UNIT_SUITE(name) void name(void)
```

---

<a name="line-89"></a>
Runs a single unit test case and reports the result to the console.

`test_cb` is the test case callback function to execute.

```cpp
#define BXR_UNIT_RUN_CASE(test_cb) (bxr_unit_run_case(#test_cb, test_cb))
```

---

<a name="line-96"></a>
Defines a test case function.

`name` is the name of the test case function.

`return` `true` if the test case passed, `false` if it failed.

```cpp
#define BXR_UNIT_CASE(name) static bool name(void)
```

---

<a name="line-105"></a>
Asserts that an expression is true.

`expr` is the expression to assert.

`return` `true` if the assertion passed, `false` if it failed.

```cpp
#define BXR_UNIT_ASSERT(expr)                                                  \
```

---

<a name="line-118"></a>
Functions that are run before and/or after a number of unit tests execute.

```cpp
typedef void (*bxr_unit_setup_cb)(void)
```

---

<a name="line-123"></a>
Sets the current setup and teardown functions.

The setup function is called prior to each unit test case and the teardown
function after. Either of these functions can be `NULL`.

`setup_cb` The setup callback function.

`teardown_cb` The teardown callback function.

```cpp
void bxr_unit_setup(bxr_unit_setup_cb setup_cb, bxr_unit_setup_cb teardown_cb)
```

---

<a name="line-135"></a>
Disables the setup and teardown functions by setting them to `NULL`.

```cpp
void bxr_unit_clear_setup(void)
```

---

<a name="line-140"></a>
Enables or disables colored output in the console.

`enabled` `true` to enable colored output, `false` to disable it.

```cpp
void bxr_unit_enable_colors(bool enabled)
```

---

<a name="line-147"></a>
Reports the results of the unit tests to the console.

```cpp
void bxr_unit_report(void)
```

---

<a name="line-152"></a>
Check whether at least one test failed.

`return` `true` if any test failed, `false` if they all passed

```cpp
bool bxr_unit_failed(void)
```

---
