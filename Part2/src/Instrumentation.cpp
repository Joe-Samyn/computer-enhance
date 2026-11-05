#include "Instrumentation.h"
#include "PerformanceUtil.h"

#include <stdint.h>
#include <inttypes.h>

InvocationPerformanceMeasurement::InvocationPerformanceMeasurement(const char* label, unsigned int index)
{    

    GlobalPerformanceMeasurement *globalMeasurement = Profiler.globalMeasurements + index;
    // If no global function measurement exists, create one
    if (globalMeasurement->label == 0x00)
    {
        globalMeasurement->label = label;
    }

    this->start = GetCPUTime();
    this->index = index;
    this->childInclusiveElapsed = 0;
    this->parent = CurrentParent;
    CurrentParent = this;
    globalMeasurement->activeInvocations++;
}

InvocationPerformanceMeasurement::~InvocationPerformanceMeasurement()
{
    this->end = GetCPUTime(); // TODO: This could go away as we only care about elapsed time
    unsigned long long inclusiveElapsed = end - start;

    // Set global exclusive 
    GlobalPerformanceMeasurement *globalMeasurement = Profiler.globalMeasurements + this->index;
    globalMeasurement->exclusiveElapsed += inclusiveElapsed - this->childInclusiveElapsed;
    globalMeasurement->activeInvocations--;
    globalMeasurement->invocationCount++;
    
    // Check if this is root and no parent function exists
    if(globalMeasurement->activeInvocations == 0)
    {
        globalMeasurement->totalInclusiveElapsed += inclusiveElapsed;
    }

    if (parent != 0x0)
    {
        // Add inclusive to parents->childInclusiveElapsed
        parent->childInclusiveElapsed += inclusiveElapsed;

        // Set this parent back to CurrentParent
        CurrentParent = parent;
    }
}

void BeginProfile()
{
    Profiler.start = GetCPUTime();
}

void EndProfile()
{
    Profiler.end = GetCPUTime();
    PrintResults();
}

void PrintMeasurementHeader(void)
{
    printf("%-24s %12s %16s %16s %14s %14s\n",
           "Function",
           "Calls",
           "Inclusive",
           "Exclusive",
           "Exclusive %",
           "Inclusive %");

    printf("------------------------------------------------------------------------------------------------------\n");
}


static void PrintMeasurement(
    const char *label,
    uint64_t calls,
    uint64_t inclusive,
    uint64_t exclusive,
    double exclusivePercent,
    double inclusivePercent)
{
    char exclusiveText[32];
    char inclusiveText[32];

    snprintf(exclusiveText, sizeof(exclusiveText),
             "%.2f%%", exclusivePercent);

    snprintf(inclusiveText, sizeof(inclusiveText),
             "%.2f%%", inclusivePercent);

    printf("%-24s %12" PRIu64 " %16" PRIu64 " %16" PRIu64 " %14s %14s\n",
           label,
           calls,
           inclusive,
           exclusive,
           exclusiveText,
           inclusiveText);
}

void PrintResults()
{
    PrintMeasurementHeader();

    unsigned long long globalElapsed = Profiler.end - Profiler.start;
    int length = ArrayCount(Profiler.globalMeasurements);
    for (int i = 0; i < length; i++)
    {
        GlobalPerformanceMeasurement m = Profiler.globalMeasurements[i];
        if (m.label == 0x00)
            continue;
        
        PrintMeasurement(m.label, m.invocationCount, m.totalInclusiveElapsed, m.exclusiveElapsed, ((double)m.exclusiveElapsed / (double)globalElapsed) * 100, ((double)m.totalInclusiveElapsed / (double)globalElapsed) * 100);
    }
}