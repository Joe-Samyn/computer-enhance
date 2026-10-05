
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