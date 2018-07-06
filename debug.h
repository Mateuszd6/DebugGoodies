#include <signal.h>

#ifndef RELEASE_PANIC_BEHAVIOUR
// This is what will happen in the panic macro by default
#define RELEASE_PANIC_BEHAVIOUR(msg, ...)                       \
    do {                                                        \
        fprintf(stderr, "%s:%d: Panic: ", __FILE__, __LINE__);  \
        fprintf(stderr, msg, ##__VA_ARGS__);                    \
        fprintf(stderr, "\n");                                  \
        std::exit(1);                                           \
    } while(0)
#endif

/// PANIC (string formatted).
// This is compiled with a release build and is used to give a
// error feedback to the user. In debug build, its just break.
#ifdef DEBUG
#define PANIC(msg, ...)                                         \
    do {                                                        \
        fprintf(stderr, "%s:%d: _Panic: ", __FILE__, __LINE__); \
        fprintf(stderr, msg, ##__VA_ARGS__);                    \
        fprintf(stderr, "\n");                                  \
        raise(SIGTRAP);                                         \
    } while(0)
#else
#define PANIC(msg, ...)                                         \
    do {                                                        \
        RELEASE_PANIC_BEHAVIOUR(msg, ##__VA_ARGS__);            \
    } while(0)
#endif

/// ASSERT (not string formatted).
// Make a debug break in debug mode, and ignore this statement
// in release mode.
#ifdef DEBUG
#define ASSERT(expr)                                            \
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
#define ASSERT(ignore) ((void)0)
#endif

/// RUNTIME ASSERT.
// Assertion that is evaluated at runtime. In release mode, if
// expr is false, macro is expanded into panic, in debug mode,
// this is just regular asser.
#ifdef DEBUG
#define RUNTIME_ASSERT(expr)                                    \
    do {                                                        \
        ASSERT(expr);                                           \
    } while(0)
#else
#define RUNTIME_ASSERT(expr)                                    \
    do {                                                        \
        if (!(expr))                                            \
        {                                                       \
            PANIC("Assertion broken: %s\n", expr);              \
        }                                                       \
    } while(0)
#endif
