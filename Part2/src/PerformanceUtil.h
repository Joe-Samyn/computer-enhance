
#pragma once 

/**
 * Get the current timestamp (ticks) computed by the OS. 
 */
unsigned long long GetOSTime();

/**
 * Get the number of ticks per second set by the OS. 
 */
unsigned long long GetOSFrequency();

/**
 * Get the approximate CPU time (cycles) via RDTSC.
 */
unsigned long long GetCPUTime();

/**
 * Estimate the CPU cycles based on OS elapsed time and CPU elapsed time
 */
unsigned long long EstimateCPUFrequency(unsigned long long osElapsedTime, unsigned long long cpuElapsedCycles);