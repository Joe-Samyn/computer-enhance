#include "Instrumentation.h"




/* Performance Profiler Class Implementation */
PerformanceProfiler::PerformanceProfiler(const char* name)
{
    this->name = name;
    this->start = GetCPUTime();
}

PerformanceProfiler::~PerformanceProfiler()
{
    this->end = GetCPUTime();
    PerformanceProfiler::GlobalInstance().measurments.emplace(this->name, this->end - this->start);
}

PerformanceProfiler& PerformanceProfiler::GlobalInstance()
{
    static PerformanceProfiler profiler = {};
    return profiler;
}

void PerformanceProfiler::StartProfiling()
{
    this->start = GetCPUTime();
}


void PerformanceProfiler::EndProfiling()
{
    this->end = GetCPUTime();
    this->elapsed = this->end - this->start;
    PrintPerformanceResults();
}

void PerformanceProfiler::PrintPerformanceResults()
{

    printf("Performance Profiling Results\n");
    printf("Program Cycles: %llu cycles\n", this->elapsed);
    for (const auto& [blockName, measurement] : measurments)
    {
        double percent = ((double)measurement / (double)this->elapsed) * 100;
        printf("%s: %llu cycles (%.2f%%)\n", blockName, measurement, percent);
    }
}