#include "Instrumentation.h"

/* Performance Profiler Class Implementation */
PerformanceProfiler::PerformanceProfiler(const char* name, int counter)
{   
    measurementIndex = counter;
    measurement = PerformanceMeasurement();
    measurement.name = name;
    start = GetCPUTime();
}

PerformanceProfiler::~PerformanceProfiler()
{
    unsigned long long end = GetCPUTime();
    measurement.elapsed = end - start;
    GlobalProfiler.measurements[measurementIndex] = measurement;
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

    for (int i = 0; i < ArrayCount(profiler.measurements); i++)
    {
        PerformanceMeasurement measurement = profiler.measurements[i];
        if (measurement.elapsed)
        {
            printf("%s: %llu (%0.2f%%)\n", measurement.name, measurement.elapsed, ((double)measurement.elapsed / (double)profiler.elapsed) * 100);
        }
    }
}