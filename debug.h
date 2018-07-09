#include <signal.h>

#define DEFAULT_PANIC_BEHAVIOUR(msg, ...)                       \
    do {                                                        \
        fprintf(stderr, "%s:%d: Panic: ", __FILE__, __LINE__);  \
        fprintf(stderr, msg, ##__VA_ARGS__);                    \
        fprintf(stderr, "\n");                                  \
    } while(0)

// Panic macro defaults. Can be redefined to do whatever user wants.
#ifndef DEBUG_PANIC_BEHAVIOUR
#define DEBUG_PANIC_BEHAVIOUR(msg, ...)                         \
    DEFAULT_PANIC_BEHAVIOUR(msg, ##__VA_ARGS__)
#endif

#ifndef RELEASE_PANIC_BEHAVIOUR
#define RELEASE_PANIC_BEHAVIOUR(msg, ...)                       \
    DEFAULT_PANIC_BEHAVIOUR(msg, ##__VA_ARGS__)
#endif

/// PANIC (string formatted).
// This is compiled with a release build and is used to give a
// error feedback to the user. In debug build, its just break.
#ifdef DEBUG
#define panic(msg, ...)                                         \
    do {                                                        \
        DEBUG_PANIC_BEHAVIOUR(msg, ##__VA_ARGS__);              \
        raise(SIGTRAP);                                         \
    } while(0)
#else
#define panic(msg, ...)                                         \
    do {                                                        \
        RELEASE_PANIC_BEHAVIOUR(msg, ##__VA_ARGS__);            \
    } while(0)
#endif

/// ASSERT (not string formatted).
// Make a debug break in debug mode, and ignore this statement
// in release mode.
#ifdef DEBUG
#define assert(expr)                                            \
    do {                                                        \
        if (!(expr))                                            \
        {                                                       \
            fprintf(stderr, "%s:%d: Assertion broken: %s\n",    \
                    __FILE__, __LINE__, #expr);                 \
                                                                \
            raise(SIGTRAP);                                     \
        }                                                       \
    } while(0)
#else
#define assert(ignore) ((void)0)
#endif

/// RUNTIME ASSERT.
// Assertion that is evaluated at runtime. In release mode, if
// expr is false, macro is expanded into panic, in debug mode,
// this is just regular asser.
#ifdef DEBUG
#define always_assert(expr)                                     \
    do {                                                        \
        assert(expr);                                           \
    } while(0)
#else
#define always_assert(expr)                                     \
    do {                                                        \
        if (!(expr))                                            \
        {                                                       \
            panic("Assertion broken: %s\n", #expr);             \
        }                                                       \
    } while(0)
#endif
