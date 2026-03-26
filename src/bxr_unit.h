/**
 * @file bxr_unit.h
 *
 * Thanks to empyreanx (pico_headers) for the unit testing API inspiration.
 *
 * @brief A simple unit testing API.
 *
 * @copyright Copyright (c) 2026 nsix. All rights reserved.
 */

#ifndef BXR_UNIT_H
#define BXR_UNIT_H

#include <SDL3/SDL.h>

// Internal API
// -----------------------------------------------------------------

typedef bool (*bxr_unit_case_cb)(void);
typedef void (*bxr_unit_suite_cb)(void);

bool bxr_unit_assert(bool passed,
                     const char *const expr,
                     const char *const file,
                     int line);

void bxr_unit_run_case(const char *const name, bxr_unit_case_cb case_cb);

void bxr_unit_run_suite(const char *const name, bxr_unit_suite_cb suite_cb);

// Public API
// -----------------------------------------------------------------

/**
 * @brief Defines a test case.
 *
 * @param name The name of the test function.
 */
#define BXR_UNIT_CASE(name) static bool name(void)

/**
 * @brief Asserts that a condition is true
 *
 * @param expr The expression to evaluate.
 */
#define BXR_UNIT_ASSERT(expr)                                                  \
  do {                                                                         \
    if (!bxr_unit_assert((expr) ? true : false, (#expr), __FILE__, __LINE__))  \
      return false;                                                            \
  } while (false)

/**
 * @brief Runs a test case function.
 *
 * @param test_cb The test callback function to execute
 */
#define BXR_UNIT_RUN_CASE(test_cb) (bxr_unit_run_case(#test_cb, test_cb))

/**
 * @brief Declares a test suite
 *
 * @param name The name of the test suite function.
 */
#define BXR_UNIT_SUITE(name) void name(void)

/**
 * @brief Runs a series of unit tests.
 *
 * @param suite_cb The test suite callback function to run
 */
#define BXR_UNIT_RUN_SUITE(suite_cb) bxr_unit_run_suite(#suite_cb, suite_cb)

/**
 * @brief Functions that are run before and/or after a number of unit
 * tests execute.
 */
typedef void (*bxr_unit_setup_cb)(void);

/**
 * @brief Sets the current setup and teardown functions.
 *
 * Sets the current setup and teardown functions.
 *
 * The setup function is called prior to each unit test and the teardown
 * function after. Either of these functions can be `NULL`.
 *
 * @param setup_cb The setup callback function
 * @param teardown_cb The teardown callback function
 */
void bxr_unit_setup(bxr_unit_setup_cb setup_cb, bxr_unit_setup_cb teardown_cb);

/**
 * @brief Disables the setup and teardown functions by setting them to `NULL`.
 */
void bxr_unit_clear_setup(void);

/**
 * @brief Enables or disables colored output in the console.
 */
void bxr_unit_enable_colors(bool enabled);

/**
 * @brief Reports the results of the unit tests to the console.
 */
void bxr_unit_report(void);

/**
 * @brief Check whether at least one test failed.
 *
 * @return true if any test failed, false if they all passed
 */
bool bxr_unit_failed(void);

#endif // BXR_UNIT_H
