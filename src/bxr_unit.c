#include "bxr_unit.h"

#define BXR_UNIT_TERM_COLOR_CODE 0x1B
#define BXR_UNIT_TERM_COLOR_RED "[1;31m"
#define BXR_UNIT_TERM_COLOR_GREEN "[1;32m"
#define BXR_UNIT_TERM_COLOR_BOLD "[1m"
#define BXR_UNIT_TERM_COLOR_RESET "[0m"

static unsigned _asserts_count = 0;
static unsigned _passed_count  = 0;
static unsigned _failed_count  = 0;
static unsigned _suites_count  = 0;

static bool _has_colors = false;

static bxr_unit_setup_cb _setup_cb    = NULL;
static bxr_unit_setup_cb _teardown_cb = NULL;

void
bxr_unit_setup(bxr_unit_setup_cb setup_cb, bxr_unit_setup_cb teardown_cb)
{
  _setup_cb    = setup_cb;
  _teardown_cb = teardown_cb;
}

void
bxr_unit_clear_setup(void)
{
  _setup_cb    = NULL;
  _teardown_cb = NULL;
}

void
bxr_unit_enable_colors(bool enabled)
{
  _has_colors = enabled;
}

bool
bxr_unit_failed(void)
{
  return (_failed_count != 0);
}

bool
bxr_unit_assert(bool passed,
                const char *const expr,
                const char *const file,
                int line)
{
  _asserts_count++;

  if (passed) {
    return true;
  }

  if (_has_colors) {
    SDL_Log("(%c%sFAILED%c%s: %s (%d): %s)\n",
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RED,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET,
            file,
            line,
            expr);
  } else {
    SDL_Log("(FAILED: %s (%d): %s)\n", file, line, expr);
  }

  return false;
}

void
bxr_unit_run_case(const char *const name, bxr_unit_case_cb case_cb)
{
  if (NULL != _setup_cb) {
    _setup_cb();
  }

  SDL_Log("Running: %s ", name);

  if (!case_cb()) {
    _failed_count++;

    if (NULL != _teardown_cb) {
      _teardown_cb();
    }

    return;
  }

  if (_has_colors) {
    SDL_Log("(%c%sOK%c%s)",
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_GREEN,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET);
  } else {
    SDL_Log("(OK)");
  }

  SDL_Log("\n");

  _passed_count++;

  if (NULL != _teardown_cb) {
    _teardown_cb();
  }
}

void
bxr_unit_run_suite(const char *const name, bxr_unit_suite_cb suite_cb)
{
  SDL_Log("===============================================================\n");

  if (_has_colors) {
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

  _suites_count++;
}

void
bxr_unit_report(void)
{
  SDL_Log("===============================================================\n");

  if (_has_colors) {
    SDL_Log("Summary: Passed: %c%s%u%c%s "
            "Failed: %c%s%u%c%s "
            "Total: %u Suites: %u "
            "Asserts: %u\n",
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_GREEN,
            _passed_count,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RED,
            _failed_count,
            BXR_UNIT_TERM_COLOR_CODE,
            BXR_UNIT_TERM_COLOR_RESET,
            _passed_count + _failed_count,
            _suites_count,
            _asserts_count);
  } else {
    SDL_Log("Summary: Passed: %u "
            "Failed: %u "
            "Total: %u Suites: %u "
            "Asserts: %u\n",
            _passed_count,
            _failed_count,
            _passed_count + _failed_count,
            _suites_count,
            _asserts_count);
  }
}
