// -------> Define 'DEBUG' to enable:
// ---> BREAK()
// ---> PANIC(msg, ...)
// ---> ASSERT(expr)
// ---> ALWAYS_ASSERT(expr)
// ---> UNREACHABLE()

// -------> Define 'BENCHMARK' to enable:
// ---> PUSH_TIMER(NAME)
// ---> POP_TIMER()
// ---> RUN_BENCHMARK(NAME, REPEAT, EXPR)
// ---> BENCHMARK_BEFORE_MAIN(NAME, REPEAT, EXPR)

// Cross platform debug break.
#if defined(__clang__)
  #include <signal.h>
  #define ___DEBUG_BREAK() raise(SIGTRAP)
#elif defined(__GNUC__) || defined(__GNUG__)
  #include <signal.h>
  #define ___DEBUG_BREAK() raise(SIGTRAP)
#elif defined(_MSC_VER)
  // TODO:
#endif

// TODO: Add ability not to use chrono.

// NOTE: How to make an anonymus name of the variable.
#define ___CONCATENATE_IMPL(NAME1, NAME2) NAME1##NAME2
#define ___CONCATENATE(NAME1, NAME2) ___CONCATENATE_IMPL(NAME1, NAME2)
#ifdef __COUNTER__
  #define ANONYMUS_NAME(NAME) ___CONCATENATE(NAME, __COUNTER__)
#else
  #define ANONYMUS_NAME(NAME) ___CONCATENATE(NAME, __LINE__)
#endif

#define ___RUN_BEFORE_MAIN_AUX(NAME, EXPR)                      \
    struct NAME                                                 \
    {                                                           \
        NAME() { EXPR; }                                        \
    }; NAME ANONYMUS_NAME(___ANON_VARIABLE_NAME___)

#define RUN_BEFORE_MAIN(EXPR)                                   \
    ___RUN_BEFORE_MAIN_AUX(ANONYMUS_NAME(                       \
                               ___ANON_VARIABLE_NAME___), EXPR)

// ------------------- DEBUG BREAK -------------------
#define BREAK() ___DEBUG_BREAK()

// ------------------- ASSERTIONS -------------------
// Default panic behaviour. If either 'DEBUG_PANIC_BEHAVIOUR' or
// 'RELEASE_PANIC_BEHAVIOUR' are not defined this is the default.
#define DEFAULT_PANIC_BEHAVIOUR(MSG, ...)                       \
    do {                                                        \
        fprintf(stderr, "%s:%d: Panic: ", __FILE__, __LINE__);  \
        fprintf(stderr, MSG, ##__VA_ARGS__);                    \
        fprintf(stderr, "\n");                                  \
    } while(0)

// Panic macro defaults.
// These can be redefined to do whatever user wants.
#ifndef DEBUG_PANIC_BEHAVIOUR
  #define DEBUG_PANIC_BEHAVIOUR(MSG, ...)                       \
      DEFAULT_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__)
#endif

#ifndef RELEASE_PANIC_BEHAVIOUR
  #define RELEASE_PANIC_BEHAVIOUR(MSG, ...)                     \
      DEFAULT_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__)
#endif

/// PANIC (string formatted).
// This is compiled with a release build and is used to give a
// error feedback to the user. In debug build, its just break.
#ifdef DEBUG
#define PANIC(MSG, ...)                                         \
    do {                                                        \
        DEBUG_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__);              \
        BREAK();                                                \
    } while(0)
#else
#define PANIC(MSG, ...)                                         \
    do {                                                        \
        RELEASE_PANIC_BEHAVIOUR(MSG, ##__VA_ARGS__);            \
    } while(0)
#endif

/// ASSERT (not string formatted).
// Make a debug break in debug mode, and ignore this statement
// in release mode.
#ifdef DEBUG
#define ASSERT(EXPR)                                            \
    do {                                                        \
        if (!(expr))                                            \
        {                                                       \
            fprintf(stderr, "%s:%d: Assertion broken: %s\n",    \
                    __FILE__, __LINE__, #EXPR);                 \
                                                                \
            BREAK();                                            \
        }                                                       \
    } while(0)
#else
#define ASSERT(IGNORE) ((void)0)
#endif

/// RUNTIME ASSERT.
// Assertion that is evaluated at runtime. In release mode, if
// expr is false, macro is expanded into PANIC, in debug mode,
// this is just regular asser.
#ifdef DEBUG
  #define ALWAYS_ASSERT(EXPR)                                   \
      do {                                                      \
          ASSERT(EXPR);                                         \
      } while(0)
#else
  #define ALWAYS_ASSERT(EXPR)                                   \
      do {                                                      \
          if (!(EXPR))                                          \
          {                                                     \
              PANIC("Assertion broken: %s\n", #EXPR);           \
          }                                                     \
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
// User can define his own version of this macro.
#ifdef BENCHMARK
  #include <chrono>
#endif

// Logic that takes place when single 'POP_TIMER' is done.
#ifndef TIME_SCOPE_MSG
  #include <iostream>
  #define TIME_SCOPE_MSG(NAME, TIME)                            \
      std::cout << "Time of [" << NAME << "]: "                 \
                << (float)TIME / 1000 << "s\n";
#endif

// Logic that takes place when after the benchmark is done.
#ifndef BENCHMARK_MSG
  #include <iostream>
  #define BENCHMARK_MSG(NAME, REPEAT, BEST, WORSE, AVG)         \
      std::cout << "Benchmark(x" << REPEAT << "): "             \
                << "[" << NAME << "]  "                         \
                << "[BEST: " << (float)best / 1000 << "s]  "    \
                << "[WORSE: " << (float)worst / 1000 << "s]  "  \
                << "[AVG: " << (float)avg / 1000 << "s]\n";
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

#if 0
// Start performance timer. Does nothing if 'BENCHMARK'
// is not defined.
#ifdef BENCHMARK
  #define PUSH_TIMER(NAME)                                      \
      do {                                                      \
          debug::detail::counters_stack[                        \
            debug::detail::index++] = {                         \
              .name = NAME,                                     \
              .timer = std::chrono::system_clock::now()         \
          };                                                    \
      } while(0);
#else
  #define PUSH_TIMER(NAME) ((void)0)
#endif
#endif

// Start performance timer. Does nothing if 'BENCHMARK'
// is not defined.
#ifdef BENCHMARK
  #define PUSH_TIMER(NAME)                                                 \
      do {                                                                 \
          debug::detail::counters_stack[debug::detail::index].name = NAME; \
          debug::detail::counters_stack[debug::detail::index++].timer =    \
                std::chrono::system_clock::now();                          \
      } while(0)
#else
  #define PUSH_TIMER(NAME) ((void)0)
#endif

// Stop timer and run the report function.
// Does nothing if 'BENCHMARK' is not defined.
#ifdef BENCHMARK
  #define POP_TIMER()                                             \
      do {                                                        \
          debug::detail::index--;                                 \
                                                                  \
          std::chrono::milliseconds diff =                        \
            std::chrono::duration_cast<                           \
              std::chrono::milliseconds>(                         \
                  std::chrono::system_clock::now() -              \
                  debug::detail::counters_stack[                  \
                      debug::detail::index].timer);               \
          TIME_SCOPE_MSG(debug::detail::counters_stack[           \
            debug::detail::index].name, diff.count());            \
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

// Auxiliary macro.
#define ___BENCHMARK_BEFORE_MAIN_AUX(NAME, BNAME, REPEAT, EXPR)          \
    struct NAME                                                          \
    {                                                                    \
        NAME()                                                           \
        {                                                                \
            unsigned long long best = 0, worst = 0, sum = 0;             \
            for (int i = 0; i < REPEAT; ++i)                             \
            {                                                            \
                auto _bench_start_ = std::chrono::system_clock::now();   \
                { EXPR; }                                                \
                std::chrono::milliseconds _bench_diff_ =                 \
                    std::chrono::duration_cast<                          \
                    std::chrono::milliseconds>(                          \
                        std::chrono::system_clock::now()                 \
                        - _bench_start_);                                \
                                                                         \
                unsigned long long _bench_res_ = _bench_diff_.count();   \
                best = _bench_res_ < best || best == 0                   \
                                     ? _bench_res_ : best;               \
                worst = _bench_res_ > worst ? _bench_res_ : worst;       \
                sum += _bench_res_;                                      \
            }                                                            \
            float avg = sum / REPEAT;                                    \
            BENCHMARK_MSG(BNAME, REPEAT, best, worse, avg);              \
        }                                                                \
    }; NAME ANONYMUS_NAME(___ANON_VARIABLE_NAME___)

#ifdef BENCHMARK
  #define BENCHMARK_BEFORE_MAIN(NAME, REPEAT, EXPR)                      \
    ___BENCHMARK_BEFORE_MAIN_AUX(                                        \
        ANONYMUS_NAME(___ANON_VARIABLE_NAME___), NAME, REPEAT, EXPR)
#else
  #define BENCHMARK_BEFORE_MAIN(NAME, REPEAT, EXPR) ;
#endif
