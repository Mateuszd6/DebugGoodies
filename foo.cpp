#define DEBUG

#include <signal.h>
#include <cstdio>
#include <cstdlib>

// TODO: Inform user.
#define DEBUG_RELEASE_PANIC(msg, ...)                           \
    do                                                          \
    {                                                           \
        fprintf(stderr, "%s:%d: Panic: ", __FILE__, __LINE__);  \
        fprintf(stderr, msg, ##__VA_ARGS__);                    \
        fprintf(stderr, "\n");                                  \
        std::exit(1);                                           \
    }                                                           \
    while(0)


/// PANIC (string formatted).
// This is compiled with a release build and is used to give a
// error feedback to the user. In debug build, its just break.
#ifdef DEBUG
#define PANIC(msg, ...)                                         \
    do                                                          \
    {                                                           \
        fprintf(stderr, "%s:%d: Panic: ", __FILE__, __LINE__);  \
        fprintf(stderr, msg, ##__VA_ARGS__);                    \
        fprintf(stderr, "\n");                                  \
        raise(SIGTRAP);                                         \
    }                                                           \
    while(0)
#else
#define PANIC(msg, ...)                                         \
    do                                                          \
    {                                                           \
        DEBUG_RELEASE_PANIC(msg, ##__VA_ARGS__);                \
    }                                                           \
    while(0)
#endif

/// ASSERT. (not string formatted).
// Make a debug break in debug mode, and ignore this statement
// in release mode.
#ifdef DEBUG
#define ASSERT(expr)                                            \
    do                                                          \
    {                                                           \
        if (!(expr))                                            \
        {                                                       \
            fprintf(stderr, "%s:%d: Assertion broken: %s\n",    \
                    __FILE__, __LINE__, #expr);                 \
                                                                \
            raise(SIGTRAP);                                     \
        }                                                       \
    }                                                           \
    while(0)
#else
#define ASSERT(ignore) ((void)0)
#endif

struct student
{
    int age, year;
    char *name;
};

int main()
{
    DEBUG_RELEASE_PANIC("THIS CANNOT HAPPEN");
    printf("Hello world!\n");

    return 0;
}
