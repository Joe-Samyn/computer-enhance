#include "Instrumentation.h"

/* Performance Profiler Class Implementation */
PerformanceProfiler::PerformanceProfiler(const char* name)
{
    measurement = PerformanceMeasurement();
    measurement.name = name;
    start = GetCPUTime();
}

PerformanceProfiler::~PerformanceProfiler()
{
    unsigned long long end = GetCPUTime();
    measurement.elapsed = start - end;
    GlobalProfiler.measurements[__COUNTER__] = measurement;
}

static void BeginProfile(void)
{
    GlobalProfiler = Profiler();
    GlobalProfiler.name = "Main";
    GlobalProfiler.start = GetCPUTime();
}

static void EndProfile(void)
{
    GlobalProfiler.end = GetCPUTime();
    GlobalProfiler.elapsed = GlobalProfiler.end - GlobalProfiler.start;

    PrintResults(GlobalProfiler);
}

static void PrintResults(Profiler &profiler)
{
    printf("Performance Results\n");
    printf("%s Elapsed: %llu\n", profiler.name, profiler.elapsed);
}