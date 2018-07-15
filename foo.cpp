#include <iostream>
#include <string>
#include <ctime>

#define DEBUG
#define BENCHMARK
#define LOGGING

#include "debug_goodies.h"

void slow()
{
    for (int i = 0; i < 0x1FFFFFFF; ++i)
    {
        auto j = i % 777;
        if (j > 0)
            j -= 3;
    }
}

// BENCHMARK_BEFORE_MAIN("Foobar", 3, slow());

int main(void)
{
    char const* foobar = "Mateusz D.";

    // LOG_INFO("This is info");
    PANIC("This is panic with bad arg");


    PUSH_TIMER("TEST1");
    for (int i = 0; i < 0x1FFFFFFF; ++i)
    {
        auto j = i % 777;
        if (j > 0)
            j -= 3;
    }
    POP_TIMER();


    // LOG_FATAL("This is info");

    return 0;
}
