#ifndef H_APPLICATION_RUN_CONTEXT
#define H_APPLICATION_RUN_CONTEXT

/**
 * Free of locks on purpose, for the time the use here is
 * interrupt-driven and single-threaded.
 */

struct ApplicationRunContext {
  bool isTickingEnabled;
  bool isProcessingWindows;
  bool isFeasibleToCount;
  uint64_t genProcessCounter;
  uint64_t killProcessCounter;
  uint32_t upperBoundExponent;
  uint32_t factFactor;

  ApplicationRunContext ()
    : isTickingEnabled(false)
    , isProcessingWindows(false)
    , isFeasibleToCount(true)
    , genProcessCounter(0)
    , killProcessCounter(0)
    , upperBoundExponent(0)
    , factFactor(0)
  {}
};

#endif
