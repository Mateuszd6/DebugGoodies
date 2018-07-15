// Define 'DEBUG' to enable:
// * BREAK()
// * PANIC(MSG, ...)
// * ASSERT(EXPR)
// * ALWAYS_ASSERT(EXPR)
// * UNREACHABLE()

// Define 'BENCHMARK' to enable:
// * PUSH_TIMER(NAME)
// * POP_TIMER()
// * RUN_BENCHMARK(NAME, REPEAT, EXPR)
// * BENCHMARK_BEFORE_MAIN(NAME, REPEAT, EXPR)

// Define 'LOGGING' to enable:
// * LOG_TRACE
// * LOG_DEBUG
// * LOG_INFO
// * LOG_WARN
// * LOG_ERROR
// * LOG_FATAL

// Other definable stuff:
// * LOG_GODIES_USE_DATE (To add date & time to your logs)
// * DEBUG_GOODIES_FPRINTF (If you don't want to use fprintf)
// * DEBUG_GODIES_BENCHMARK_FILE (To redirect benchmarks results)
// * DEBUG_GODIES_LOG_FILE (To redirect logs to the file, not stderr)
// * DEBUG_PANIC_BEHAVIOUR(MSG, ...) (Panic logic if defined DEBUG)
// * RELEASE_PANIC_BEHAVIOUR(MSG, ...) (Panic logic if not defined DEBUG)
// * TIME_SCOPE_MSG(NAME, TIME) (Logic when time scope ends)
// * BENCHMARK_MSG(NAME, REPEAT, BEST, WORST, AVG) (Logic after benchmark)

// TODO: Add ability not to use chrono.

#ifndef __DEBUG_GOODIES_H__
#define __DEBUG_GOODIES_H__

// ------------------- AUXILIARY -------------------

#define ___CONCATENATE_IMPL(NAME1, NAME2) NAME1##NAME2
#define ___CONCATENATE(NAME1, NAME2) ___CONCATENATE_IMPL(NAME1, NAME2)

#ifdef __COUNTER__
  #define ANONYMUS_NAME(NAME) ___CONCATENATE(NAME, __COUNTER__)
#else
  #define ANONYMUS_NAME(NAME) ___CONCATENATE(NAME, __LINE__)
#endif

#define ___RUN_BEFORE_MAIN_AUX(NAME, EXPR)                               \
    struct NAME                                                          \
    {                                                                    \
        NAME() { EXPR; }                                                 \
    }; NAME ANONYMUS_NAME(___ANON_VARIABLE_NAME___)

#define RUN_BEFORE_MAIN(EXPR)                                            \
    ___RUN_BEFORE_MAIN_AUX(ANONYMUS_NAME(___ANON_VARIABLE_NAME___), EXPR)

// ------------------- PLATFORM AND DEFAULTS -------------------

// Cross platform debug break.
#if defined(__clang__)
  #include <signal.h>
  #define ___DEBUG_BREAK() raise(SIGTRAP)
#elif defined(__GNUC__) || defined(__GNUG__)
  #include <signal.h>
  #define ___DEBUG_BREAK() raise(SIGTRAP)
#elif defined(_MSC_VER)
  #define ___DEBUG_BREAK() __debugbreak()
#endif

// Define 'DEBUG_GOODIES_FPRINTF' if you dont want to use fprintf.
#ifndef DEBUG_GOODIES_FPRINTF
  #include <cstdio>
  #define DEBUG_GOODIES_FPRINTF fprintf
#endif

// Define 'DEBUG_GODIES_LOG_FILE' for benchmarks output. Default is stdout.
#ifndef DEBUG_GODIES_BENCHMARK_FILE
  #include <cstdio>
  #define DEBUG_GODIES_BENCHMARK_FILE stdout
#endif

// Define 'DEBUG_GODIES_LOG_FILE' to set log output. Default is stderr.
#ifndef DEBUG_GODIES_LOG_FILE
  #include <cstdio>
  #define DEBUG_GODIES_LOG_FILE stderr
#endif

// Define 'LOG_GODIES_USE_DATE' for time info in logs, requires chorno
#ifdef LOG_GODIES_USE_DATE
  #include <chrono>
  #define ____PRINT_DATETIME_TO_FILE(_FILE)                              \
      do {                                                               \
          auto in_time_t =                                               \
              std::chrono::system_clock::to_time_t(                      \
                  std::chrono::system_clock::now());                     \
          auto tm_info = localtime(&in_time_t);                          \
                                                                         \
          DEBUG_GOODIES_FPRINTF(                                         \
              _FILE,                                                     \
              "%02d-%02d-%d %02d:%02d:%02d: ",                           \
              tm_info->tm_year + 1900, tm_info->tm_mon + 1,              \
              tm_info->tm_mday, tm_info->tm_hour,                        \
              tm_info->tm_min, tm_info->tm_sec);                         \
      } while(0)
#else
  #define ____PRINT_DATETIME_TO_FILE(IGNORE) ((void)0)
#endif

#define ___LOG(MSG, LOG_TYPE, ...)                                       \
    do {                                                                 \
        DEBUG_GOODIES_FPRINTF(DEBUG_GODIES_LOG_FILE,                     \
                              "%s:%d: %s: ",                             \
                              __FILE__, __LINE__, #LOG_TYPE);            \
        ____PRINT_DATETIME_TO_FILE(DEBUG_GODIES_LOG_FILE);               \
        DEBUG_GOODIES_FPRINTF(DEBUG_GODIES_LOG_FILE, MSG,                \
                              ##__VA_ARGS__);                            \
        DEBUG_GOODIES_FPRINTF(DEBUG_GODIES_LOG_FILE, "\n");              \
} while(0)

// Default panic behaviour. If either 'DEBUG_PANIC_BEHAVIOUR' or
// 'RELEASE_PANIC_BEHAVIOUR' are not defined this is the default.
#define DEFAULT_PANIC_BEHAVIOUR(MSG, ...)                                \
    do {                                                                 \
        ___LOG(MSG, PANIC, ##__VA_ARGS__);                               \
    } while(0)

// Panic macro defaults.
// These can be redefined to do whatever user wants.
#ifndef DEBUG_PANIC_BEHAVIOUR
  #define DEBUG_PANIC_BEHAVIOUR(MSG, ...)                                \
      DEFAULT_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__)
#endif

#ifndef RELEASE_PANIC_BEHAVIOUR
  #define RELEASE_PANIC_BEHAVIOUR(MSG, ...)                              \
      DEFAULT_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__)
#endif

// Logic that takes place when single 'POP_TIMER' is done.
#ifndef TIME_SCOPE_MSG
  #define TIME_SCOPE_MSG(NAME, TIME)                                     \
      ___LOG("%s - %.3fs", "TIMED: ", NAME, (float)TIME / 1000)
#endif

// Logic that takes place when after the benchmark is done.
#ifndef BENCHMARK_MSG
  #define BENCHMARK_MSG(NAME, REPEAT, BEST, WORST, AVG)                  \
    DEBUG_GOODIES_FPRINTF(DEBUG_GODIES_BENCHMARK_FILE,                   \
        "%s:%d: BENCH: %s(x%d) - AVG: %.3fs, BEST: %.3fs, WORST: %.3fs\n", \
        __FILE__, __LINE__, NAME, REPEAT,                                \
        (float)AVG / 1000, (float)BEST / 1000, (float)WORST / 1000)
#endif

// ------------------- DEBUG BREAK -------------------

#define BREAK() ___DEBUG_BREAK()

// ------------------- ASSERTIONS -------------------

/// PANIC (string formatted).
// This is compiled with a release build and is used to give a
// error feedback to the user. In debug build, its just break.
#ifdef DEBUG
  #define PANIC(MSG, ...)                                                \
      do {                                                               \
          DEBUG_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__);                     \
          BREAK();                                                       \
      } while(0)
#else
  #define PANIC(MSG, ...)                                                \
      do {                                                               \
          RELEASE_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__);                   \
      } while(0)
#endif

/// ASSERT (not string formatted).
// Make a debug break in debug mode, and ignore this statement
// in release mode.
#ifdef DEBUG
  #define ASSERT(EXPR)                                                   \
      do {                                                               \
          if (!(expr))                                                   \
          {                                                              \
              ___LOG("%s", ASSERT_FAIL, #EXPR);                          \
              BREAK();                                                   \
          }                                                              \
      } while(0)
#else
  #define ASSERT(IGNORE) ((void)0)
#endif

/// RUNTIME ASSERT.
// Assertion that is evaluated at runtime. In release mode, if
// expr is false, macro is expanded into PANIC, in debug mode,
// this is just regular asser.
#ifdef DEBUG
  #define ALWAYS_ASSERT(EXPR)                                            \
      do {                                                               \
          ASSERT(EXPR);                                                  \
      } while(0)
#else
  #define ALWAYS_ASSERT(EXPR)                                            \
      do {                                                               \
          if (!(EXPR)) { PANIC("Assertion broken: %s\n", #EXPR); }       \
      } while(0)
#endif

/// UNREACHABLE.
// Marks code as unreachable. In debug mode this causes panic,
// in release marks as unreachable using __builtin_unreachable
#ifdef DEBUG
  #define UNREACHABLE() PANIC("Unreachable code reached!")
#else
  #define UNREACHABLE() __builtin_unreachable()
#endif

// ------------------- BENCHMARK -------------------

// TODO: Add version that does not depend on chrono.
#ifdef BENCHMARK
  #include <chrono>
#endif

// Auxiliary structure for time queries.
#ifdef BENCHMARK
  namespace debug {
      namespace detail {
          struct query_counter
          {
              // Identifier of the query,
              // e.g. name of the function.
              char *name;

              // Start time.
              std::chrono::time_point<std::chrono::system_clock>
              timer;
          };

          int index = 0;
          query_counter counters_stack[32];
      }
  }
#endif

// Start performance timer. Does nothing if 'BENCHMARK'
// is not defined.
#ifdef BENCHMARK
  #define PUSH_TIMER(NAME)                                               \
      do {                                                               \
          debug::detail::counters_stack[debug::detail::index].name=NAME; \
          debug::detail::counters_stack[debug::detail::index++].timer =  \
                std::chrono::system_clock::now();                        \
      } while(0)
#else
  #define PUSH_TIMER(NAME) ((void)0)
#endif

// Stop timer and run the report function.
// Does nothing if 'BENCHMARK' is not defined.
#ifdef BENCHMARK
  #define POP_TIMER()                                                    \
      do {                                                               \
          debug::detail::index--;                                        \
          std::chrono::milliseconds diff =                               \
              std::chrono::duration_cast<std::chrono::milliseconds>(     \
                  std::chrono::system_clock::now() -                     \
                  debug::detail::counters_stack[debug::detail::index]    \
                      .timer);                                           \
          TIME_SCOPE_MSG(                                                \
              debug::detail::counters_stack[debug::detail::index].name,  \
              diff.count());                                             \
      } while(0)

#else
  #define POP_TIMER() ((void)0)
#endif

// Repeat 'REPEAT' times benchark and report results.
// Does nothing if benchmark is not defined.
#ifdef BENCHMARK
  #define RUN_BENCHMARK(NAME, REPEAT, EXPR)                              \
      do {                                                               \
          unsigned long long best = 0, worst = 0, sum = 0;               \
          for (int i = 0; i < REPEAT; ++i)                               \
          {                                                              \
              auto _bench_start_ = std::chrono::system_clock::now();     \
              { EXPR; }                                                  \
              std::chrono::milliseconds _bench_diff_ =                   \
                  std::chrono::duration_cast<std::chrono::milliseconds>( \
                      std::chrono::system_clock::now() - _bench_start_); \
                                                                         \
              unsigned long long _bench_res_ = _bench_diff_.count();     \
              best = _bench_res_ < best || best == 0                     \
                                   ? _bench_res_ : best;                 \
              worst = _bench_res_ > worst ? _bench_res_ : worst;         \
              sum += _bench_res_;                                        \
          }                                                              \
          float avg = sum / REPEAT;                                      \
          BENCHMARK_MSG(NAME, REPEAT, best, worse, avg);                 \
      } while (0);
#else
  #define RUN_BENCHMARK(IGNORE1, IGNORE2, IGNORE3) ((void)0)
#endif

#ifdef BENCHMARK
 #define ___BENCHMARK_BEFORE_MAIN_AUX(NAME, BNAME, REPEAT, EXPR)         \
     struct NAME                                                         \
     {                                                                   \
         NAME()                                                          \
         {                                                               \
             unsigned long long best = 0, worst = 0, sum = 0;            \
             for (int i = 0; i < REPEAT; ++i)                            \
             {                                                           \
                 auto _bench_start_ = std::chrono::system_clock::now();  \
                 { EXPR; }                                               \
                 std::chrono::milliseconds _bench_diff_ =                \
                     std::chrono::duration_cast<                         \
                     std::chrono::milliseconds>(                         \
                         std::chrono::system_clock::now()                \
                         - _bench_start_);                               \
                                                                         \
                 unsigned long long _bench_res_ = _bench_diff_.count();  \
                 best = _bench_res_ < best || best == 0                  \
                                      ? _bench_res_ : best;              \
                 worst = _bench_res_ > worst ? _bench_res_ : worst;      \
                 sum += _bench_res_;                                     \
             }                                                           \
             float avg = sum / REPEAT;                                   \
             BENCHMARK_MSG(BNAME, REPEAT, best, worst, avg);             \
         }                                                               \
     }; NAME ANONYMUS_NAME(___ANON_VARIABLE_NAME___)
#endif

#ifdef BENCHMARK
  #define BENCHMARK_BEFORE_MAIN(NAME, REPEAT, EXPR)                      \
    ___BENCHMARK_BEFORE_MAIN_AUX(                                        \
        ANONYMUS_NAME(___ANON_VARIABLE_NAME___), NAME, REPEAT, EXPR)
#else
  #define BENCHMARK_BEFORE_MAIN(NAME, REPEAT, EXPR) ;
#endif

// ------------------- LOGGING -------------------

// TODO: Refactor to dont use fprintf.
// TODO: Add info about functionality.

#if defined(DEBUG) && defined(LOGGING)
  #define LOG_TRACE(MSG, ...) ___LOG(MSG, TRACE, ##__VA_ARGS__)
  #define LOG_DEBUG(MSG, ...) ___LOG(MSG, DEBUG, ##__VA_ARGS__)
#elif defined(LOGGING)
  #define LOG_TRACE(IGNORE, ...) ((void)0)
  #define LOG_DEBUG(IGNORE, ...) ((void)0)
#else
#endif

#ifdef LOGGING
  #define LOG_INFO(MSG, ...) ___LOG(MSG, INFO, ##__VA_ARGS__)
  #define LOG_WARN(MSG, ...) ___LOG(MSG, WARN, ##__VA_ARGS__)
  #define LOG_ERROR(MSG, ...) ___LOG(MSG, ERROR, ##__VA_ARGS__)
  #define LOG_FATAL(MSG, ...) ___LOG(MSG, FATAL, ##__VA_ARGS__)
#endif

#endif // __DEBUG_GOODIES_H__
