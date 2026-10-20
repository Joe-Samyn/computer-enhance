#pragma once 

#include "PerformanceUtil.h"

#include <stdio.h>
#include <unordered_map>

class PerformanceProfiler;

#define CONCAT_HIDDEN(a, b) a##b
#define CONCAT(a, b) CONCAT_HIDDEN(a, b)
#define TimeFunction PerformanceProfiler CONCAT(fun_, __LINE__) = PerformanceProfiler(__func__)
#define TimeBlock(name) PerformanceProfiler CONCAT(block_, __LIN__) = PerformanceProfiler(name)
#define BeginProfile PerformanceProfiler::GlobalInstance().StartProfiling()
#define EndProfile PerformanceProfiler::GlobalInstance().EndProfiling()

class PerformanceProfiler
{
private:
    const char* name;
    unsigned long long start;
    unsigned long long end;
    unsigned long long elapsed;

public:
    std::unordered_map<const char*, unsigned long long> measurments;

    PerformanceProfiler() = default;
    PerformanceProfiler(const char* name);
    ~PerformanceProfiler();

    static PerformanceProfiler& GlobalInstance();

    /**
     * Captures current timestamp counter as start time
     */
    void StartProfiling();

    /**
     * Captures current timestamp counter value as end time
     */
    void EndProfiling();

    /**
     * Display performance results in console
     */
    void PrintPerformanceResults();
};