#include "Instrumentation.h"

/* Performance Profiler Class Implementation */
PerformanceProfiler::PerformanceProfiler(const char* name, int counter)
{   
    measurementIndex = counter;
    parentIndex = CurrentProfilerIndex;
    CurrentProfilerIndex = measurementIndex;
    GlobalProfiler.measurements[measurementIndex].name = name;
    start = GetCPUTime();
}

PerformanceProfiler::~PerformanceProfiler()
{
    unsigned long long elapsed = GetCPUTime() - start;

    PerformanceMeasurement* parent = GlobalProfiler.measurements + parentIndex;
    PerformanceMeasurement* current = GlobalProfiler.measurements + measurementIndex;

    CurrentProfilerIndex = parentIndex;
    current->elapsed = elapsed;

    if (parent != current)
        parent->childrenElapsed += elapsed;
}

static void BeginProfile(void)
{
    GlobalProfiler = Profiler();
    GlobalProfiler.start = GetCPUTime();
}

static void EndProfile(void)
{
    GlobalProfiler.end = GetCPUTime();

    PrintResults(GlobalProfiler);
}

static void PrintResults(Profiler &profiler)
{
    unsigned long long globalElapsed = profiler.end - profiler.start;
    printf("Performance Results\n");
    printf("%s Elapsed: %llu\n", "Main", globalElapsed);

    for (int i = 0; i < ArrayCount(profiler.measurements); i++)
    {
        PerformanceMeasurement measurement = profiler.measurements[i];
        if (measurement.elapsed)
        {
            if (measurement.childrenElapsed)
            {
                printf("%s: %llu (%0.2f%%, %0.2f%% w/ Children)\n", measurement.name, measurement.elapsed, ((double)(measurement.elapsed - measurement.childrenElapsed) / (double)globalElapsed) * 100, (double)measurement.elapsed / (double)globalElapsed * 100);
            }
            else 
            {
                printf("%s: %llu (%0.2f%%)\n", measurement.name, measurement.elapsed, ((double)measurement.elapsed / (double)globalElapsed) * 100);
            }
        }
    }
}