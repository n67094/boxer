#include "bxr_unit.h"

#define BXR_UNIT_TERM_COLOR_CODE 0x1B
#define BXR_UNIT_TERM_COLOR_RED "[1;31m"
#define BXR_UNIT_TERM_COLOR_GREEN "[1;32m"
#define BXR_UNIT_TERM_COLOR_BOLD "[1m"
#define BXR_UNIT_TERM_COLOR_RESET "[0m"

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
    SDL_Log("\t(%c%sFAILED%c%s: %s (%d): %s)\n",
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RED,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET,
            file,
            line,
            expr);
  } else {
    SDL_Log("\t(FAILED: %s (%d): %s)\n", file, line, expr);
  }

  return false;
}

void
bxr_unit_run_case(const char *const name, bxr_unit_case_cb case_cb)
{
  if (NULL != setup_cb_) {
    setup_cb_();
  }

  SDL_Log("Running: %s ", name);

  if (!case_cb()) {
    failed_count_++;

    if (NULL != teardown_cb_) {
      teardown_cb_();
    }

    return;
  }

  if (has_colors_) {
    SDL_Log("\t(%c%sOK%c%s)",
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_GREEN,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET);
  } else {
    SDL_Log("\t(OK)");
  }

  passed_count_++;

  if (NULL != teardown_cb_) {
    teardown_cb_();
  }
}

void
bxr_unit_run_suite(const char *const name, bxr_unit_suite_cb suite_cb)
{
  SDL_Log("===============================================================\n");

  if (has_colors_) {
    SDL_Log("%c%sRunning: %s%c%s\n",
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_BOLD,
            name,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET);
  } else {
    SDL_Log("Running: %s\n", name);
  }

  SDL_Log("---------------------------------------------------------------\n");
  suite_cb();

  suites_count_++;
}

void
bxr_unit_report(void)
{
  SDL_Log("===============================================================\n");

  if (has_colors_) {
    SDL_Log("Summary: Passed: %c%s%u%c%s "
            "Failed: %c%s%u%c%s "
            "Total: %u Suites: %u "
            "Asserts: %u\n",
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_GREEN,
            passed_count_,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RED,
            failed_count_,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET,
            passed_count_ + failed_count_,
            suites_count_,
            asserts_count_);
  } else {
    SDL_Log("Summary: Passed: %u "
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
