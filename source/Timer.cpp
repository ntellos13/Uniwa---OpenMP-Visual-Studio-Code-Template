#include <omp.h>

#include "Timer.hpp"

Timer::Timer()
{
    start = omp_get_wtime(); 
}

double Timer::Elapsed()
{
    auto end = omp_get_wtime();

    return (end - start) * 1000;
}