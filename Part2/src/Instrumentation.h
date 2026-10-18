#pragma once 

#include "PerformanceUtil.h"

#include <stdio.h>

class PerformanceTimer;

#define CONCAT_HIDDEN(a, b) a##b
#define CONCAT(a, b) CONCAT_HIDDEN(a, b)
#define TimeFunction PerformanceTimer CONCAT(v, __LINE__) = PerformanceTimer(__func__)
#define TimeBlock(name) PerformanceTimer(name)


class PerformanceTimer 
{
private:
    const char* name;
    unsigned long long start;
    unsigned long long end;
public:
    PerformanceTimer(const char* name);
    ~PerformanceTimer();
};