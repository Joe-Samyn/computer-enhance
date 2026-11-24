#include "PerformanceUtil.h"

#include <cstdio>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

unsigned long long GetOSTime(void)
{
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<unsigned long long>(counter.QuadPart);
}

unsigned long long GetOSFrequency(void)
{
    static const unsigned long long frequency = [] {
        LARGE_INTEGER countsPerSecond;
        QueryPerformanceFrequency(&countsPerSecond);
        return static_cast<unsigned long long>(countsPerSecond.QuadPart);
    }();
    return frequency;
}

#else
#include <time.h>

/*
    NOTE: On Linux, we just return 1,000,000,000 to convert to nanoseconds as this is the units used by Linux. 
*/
unsigned long long GetOSFrequency(void)
{
    return 1000000000; 
}

unsigned long long GetOSTime(void)
{
    struct timespec ts;
    int timeResult = clock_gettime(CLOCK_MONOTONIC, &ts);
    
    // TODO: validate we successfully recieved time

    long timestamp = GetOSFrequency()*ts.tv_sec + ts.tv_nsec;
    return timestamp;
}

#endif

unsigned long long GetOSTimeMS(void)
{
    unsigned long long ticks = GetOSTime();

    unsigned long long ms = 1000 * ticks / GetOSFrequency();

    return ms;
}

unsigned long long GetCPUTime(void)
{
    return __rdtsc();
}

unsigned long long EstimateCPUFrequency(unsigned long long osElapsedTime, unsigned long long cpuElapsedCycles)
{
    return GetOSFrequency() * cpuElapsedCycles / osElapsedTime;
}

unsigned long long EstimateCPUTimerFrequency()
{

    unsigned int timeToWaitMs = 100;
    unsigned long long timeToWaitTicks = timeToWaitMs * GetOSFrequency() / 1000;

    unsigned long long startOS = GetOSTime();
    unsigned long long endOS = GetOSTime();

    unsigned long long startCPU = GetCPUTime();

    while(endOS - startOS < timeToWaitTicks) {
        endOS = GetOSTime();
    }

    unsigned long long elapsedCPU = GetCPUTime() - startCPU;
    unsigned long long elapsedOS = endOS - startOS;

    unsigned long long cpuFrequency = GetOSFrequency() * elapsedCPU / elapsedOS;

    return cpuFrequency;

}

double CPUTimeToMs(unsigned long long cpuTime, unsigned long long cpuFreq)
{
    return 1000 * (double)cpuTime / (double)cpuFreq;
}
