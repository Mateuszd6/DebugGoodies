#include <iostream>
#include <string>
#include <ctime>

#define DEBUG
// #define BENCHMARK
#define LOGGING
#define DG_LOG_LVL DG_LOG_LVL_WARN

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
    LOG_WARN("This (s), is message!");
    return 0;
}
