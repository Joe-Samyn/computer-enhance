#pragma once 

#include "PerformanceUtil.h"

struct PerformanceMeasurement 
{
    const char* name;
    unsigned long long elapsed;
};

struct Profiler
{
    const char* name;
    unsigned long long start;
    unsigned long long end;
    unsigned long long elapsed;

    PerformanceMeasurement measurements[4096];
};

struct PerformanceProfiler
{
    unsigned long long start;
    int measurementIndex;
    PerformanceMeasurement measurement;

    PerformanceProfiler(const char* name, int counter);
    ~PerformanceProfiler();
};

static Profiler GlobalProfiler;

#define CONCAT(a, b) a##b
#define ConcatName(a, b) CONCAT(a, b)
#define TimeFunction PerformanceProfiler ConcatName(fun, __LINE__) = PerformanceProfiler(__func__, __COUNTER__);
#define TimeBlock(name) PerformanceProfiler ConcatName(block, __LINE__) = PerformanceProfiler(name, __COUNTER__);

#define ArrayCount(array) sizeof(array) / sizeof(array[0])

static void BeginProfile(void);
static void EndProfile(void);
static void PrintResults(Profiler &profiler);