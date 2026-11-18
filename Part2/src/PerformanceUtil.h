
#pragma once 

/**
 * Get the current timestamp (ticks) computed by the OS. 
 */
unsigned long long GetOSTime(void);

/**
 * Get the current timestamp (ticks) computed by the OS in milliseconds
 */
unsigned long long GetOSTimeMS(void);

/**
 * Get the number of ticks per second set by the OS. 
 */
unsigned long long GetOSFrequency(void);

/**
 * Get the approximate CPU time (cycles) via RDTSC.
 */
unsigned long long GetCPUTime(void);

/**
 * Estimate the CPU cycles based on OS elapsed time and CPU elapsed time
 */
unsigned long long EstimateCPUFrequency(unsigned long long osElapsedTime, unsigned long long cpuElapsedCycles);

/**
 * Estimate the frequency of CPU timer counts (rdtsc cycles) per second
*/
unsigned long long EstimateCPUTimerFrequency(void);

/**
 * Convert RDTSC value to milliseconds 
 */
double CPUTimeToMs(unsigned long long cpuTime, unsigned long long cpuFreq);