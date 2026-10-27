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
    PerformanceMeasurement measurement;

    PerformanceProfiler(const char* name);
    ~PerformanceProfiler();
};

static Profiler GlobalProfiler;

#define CONCAT(a, b) a##b
#define ConcatName(a, b) CONCAT(a, b)
#define TimeFunction PerformanceProfiler ConcatName(fun, __LINE__) = PerformanceProfiler(__func__);
#define TimeBlock(name) PerformanceProfiler ConcatName(block, __LINE__) = PerformanceProfiler(name);

static void BeginProfile(void);
static void EndProfile(void);
static void PrintResults(Profiler &profiler);