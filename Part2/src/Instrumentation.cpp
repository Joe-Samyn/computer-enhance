#include "Instrumentation.h"


PerformanceTimer::PerformanceTimer(const char* name) 
{
    this->start = GetCPUTime();
    this->name = name;
}

PerformanceTimer::~PerformanceTimer()
{
    this->end = GetCPUTime();

    printf("%s Cycles: %llu\n", this->name, (this->end - this->start));
}