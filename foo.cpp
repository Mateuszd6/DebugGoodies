#include <iostream>
#include <string>
#include <ctime>

#define DEBUG
#define BENCHMARK
#include "debug.h"

int main(void)
{
    PUSH_TIMER("TEST1");
    for (int i = 0; i < 0x1FFFFFFF; ++i)
    {
        auto j = i % 777;
        if (j > 0)
            j -= 3;
    }
    POP_TIMER();

    return 0;
}
