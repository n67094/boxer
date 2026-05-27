#include "bxr_unit.h"
#include "bxr_defs.h"
#include "bxr_log.h"

static unsigned asserts_count_ = 0;
static unsigned passed_count_  = 0;
static unsigned failed_count_  = 0;
static unsigned suites_count_  = 0;

static bool has_colors_ = false;

static bxr_unit_setup_cb setup_cb_    = NULL;
static bxr_unit_setup_cb teardown_cb_ = NULL;

void
bxr_unit_setup(bxr_unit_setup_cb setup_cb, bxr_unit_setup_cb teardown_cb)
{
  setup_cb_    = setup_cb;
  teardown_cb_ = teardown_cb;
}

void
bxr_unit_clear_setup(void)
{
  setup_cb_    = NULL;
  teardown_cb_ = NULL;
}

void
bxr_unit_enable_colors(bool enabled)
{
  has_colors_ = enabled;
}

bool
bxr_unit_failed(void)
{
  return (failed_count_ != 0);
}

bool
bxr_unit_assert(bool passed,
                const char *const expr,
                const char *const file,
                int line)
{
  asserts_count_++;

  if (passed) {
    return true;
  }

  if (has_colors_) {
    BXR_LOG_INFO("\t(%sFAILED%s: %s (%d): %s)\n",
                 BXR_TERM_COLOR_RED,
                 BXR_TERM_RESET,
                 file,
                 line,
                 expr);
  } else {
    BXR_LOG_INFO("\t(FAILED: %s (%d): %s)\n", file, line, expr);
  }

  return false;
}

void
bxr_unit_run_case(const char *const name, bxr_unit_case_cb case_cb)
{
  if (NULL != setup_cb_) {
    setup_cb_();
  }

  BXR_LOG_INFO("Running: %s ", name);

  if (!case_cb()) {
    failed_count_++;

    if (NULL != teardown_cb_) {
      teardown_cb_();
    }

    return;
  }

  if (has_colors_) {
    BXR_LOG_INFO("\t(%sOK%s)", BXR_TERM_COLOR_GREEN, BXR_TERM_RESET);
  } else {
    BXR_LOG_INFO("\t(OK)");
  }

  passed_count_++;

  if (NULL != teardown_cb_) {
    teardown_cb_();
  }
}

void
bxr_unit_run_suite(const char *const name, bxr_unit_suite_cb suite_cb)
{
  BXR_LOG_INFO(
      "===============================================================\n");

  if (has_colors_) {
    BXR_LOG_INFO("%sRunning: %s%s\n", BXR_TERM_BOLD, name, BXR_TERM_RESET);
  } else {
    BXR_LOG_INFO("Running: %s\n", name);
  }

  BXR_LOG_INFO(
      "---------------------------------------------------------------\n");
  suite_cb();

  suites_count_++;
}

void
bxr_unit_report(void)
{
  BXR_LOG_INFO(
      "===============================================================\n");

  if (has_colors_) {
    BXR_LOG_INFO("Summary: Passed: %s%u%s "
                 "Failed: %s%u%s "
                 "Total: %u Suites: %u "
                 "Asserts: %u\n",
                 BXR_TERM_COLOR_GREEN,
                 passed_count_,
                 BXR_TERM_RESET,
                 BXR_TERM_COLOR_RED,
                 failed_count_,
                 BXR_TERM_RESET,
                 passed_count_ + failed_count_,
                 suites_count_,
                 asserts_count_);
  } else {
    BXR_LOG_INFO("Summary: Passed: %u "
                 "Failed: %u "
                 "Total: %u Suites: %u "
                 "Asserts: %u\n",
                 passed_count_,
                 failed_count_,
                 passed_count_ + failed_count_,
                 suites_count_,
                 asserts_count_);
  }
}
