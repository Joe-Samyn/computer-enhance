#pragma once 

#ifndef PROFILE
#define PROFILE 0
#endif 

/*
    -- Specifications for Instrumentation Based Profiler --

     This is a instrumentation based performance profiler that allows functions and blocks of code to be 
     measured for performance analysis, optimization, and troubleshooting purposes. 

     REQUIREMENTS
        - Track global metrics for a function being measured
            - Metrics to be tracked at the global level
                * Global inclusive runtime
                * Global exclusive runtime
                * Active function invocations (think ARC)
                * Total invocation count 
        - Track invocation level metrics
            - Metrics to be tracked 
                * Inclusive runtime (including child inclusive runtime)
                * Exclusive runtime (excluding child runtime)
        - Output clean, easy to read metrics in format below

            Function   Calls   Inclusive   Exclusive   Exclusive %  Inclusive %
            --------------------------------------------------------------------
            A          2       3000        1900        63.33%       63.33%
            B          2       1100        1100        36.67%       36.67%      

    NOTES
        - Active Invocation Count measures the number of currently active invocations of a function/block, including recursion 
            * Example: Active Invocations of A() = 2
                main()
                        |
                        +-- A(2)    300 inclusive cycles
                                |
                                +-- A(1)    100 inclusive cycles
        - Total Invocation Count measures the number of total invocations of a function/block.
            * Example: Total Invocations of A() = 3
                main()
                    A(2) begins
                        A(1) begins
                        A(1) ends
                    A(2) ends

                    A(3) begins
                    A(3) ends

        - Global Inclusive Runtime measures the outermost inclusive runtime of a function/block. 
            * Example: Global Inclusive Runtime = 300 cycles 
                * Example: 
                    main()
                        |
                        +-- A(2)    300 inclusive cycles
                                |
                                +-- A(1)    100 inclusive cycles
        - Global Exclusive Runtime measures the aggregated exclusive runtime of all calls to a particular function.
            * Example: Global Exclusive Runtime of A() = 200 + 100 = 300 
                 main()
                        |
                        +-- A(2)    300 inclusive cycles
                                |
                                +-- A(1)    100 Exclusive cycles

        - Invocation Inclusive runtime measures current function/block + child inclusive 
            * Example: 
                A(2)    300 inclusive cycles
                    |
                    +-- A(1)    100 inclusive cycles

        - Invocation Exclusive runtime measures a functions runtime excluding elapsed child inclusive runtime. 
            * Example: A(2) Exclusive Runtime = 300 - 100 = 200 cycles
                main()
                    |
                    +-- A(2)    300 cycles, 200 exclusive cycles
                            |
                            +-- A(1)    100 cycles

        - Exclusive % = Function Total Exclusive Runtime / Program Total Runtime
            * Note - This will exclude calls to nonrecursive nested functions, resulting in a measurement that defines strictly how much 
                time was spent exclusively in this function. 
        - Inclusive % = Function Invlusive Runtime / Program Total Runtime
 */

 #define ArrayCount(array) sizeof(array) / sizeof(array[0]);

 /**
  * Global metrics for an instrumented function or block
  */
 struct GlobalPerformanceMeasurement {

    /**
     * Name or label for the function being measured. 
     */
    const char* label;

    /**
     * Total exclusive cycles spent in a function. 
     */
    unsigned long long exclusiveElapsed;

    /**
     * Total cycles spent in function, including all time spent in nested/child function calls. 
     */
    unsigned long long totalInclusiveElapsed; 

    /**
     * Total number of times a function/block has been invoked. 
     */
    unsigned long long invocationCount;

    /**
     * The number of currently active invocations. 
     * @note If 0, there are no current active invocations running.
     */
    unsigned long long activeInvocations;

    /**
     * Total amount of bytes processed by the function or block
     */
    unsigned int totalBytesProcessed;
 };

 /**
  * Invocation based metrics for a function or block
  */
 struct InvocationPerformanceMeasurement {
     
    /**
     * Cycles elapsed at start of measurement. 
     */
    unsigned long long start; 

    /**
     * Cycles elapsed at end of measurement;
     */
    unsigned long long end; 

    /**
     * Inded of the global measurement for the function 
     */
    unsigned int index;

    /**
     * The total cycles spent in nested or child function calls. 
     * @note This is used to determine a function or blocks exclusive cycle time. 
     */
    unsigned long long childInclusiveElapsed;

    /**
     * Parent measurement for current invocation
     */
    InvocationPerformanceMeasurement *parent;

    InvocationPerformanceMeasurement(const char* label, unsigned int index, unsigned int bytes = 0);
    ~InvocationPerformanceMeasurement();

 };

 /**
  * Global profiler data structure managing all performance metrics for instrumented function, blocks, and the program. 
  */
 struct GlobalProfiler {

    /**
     * Cycles elapsed at start of global profiler measurement. 
     */
    unsigned long long start; 

    /**
     * Cycles elapsed at end of global profiler measurement. 
     */
    unsigned long long end; 

    /**
     * OS time captured at start of global profiler measurement
     */
    unsigned long long startTime;
    
    /**
     * OS time captured at end of global profiler measurement
     */
    unsigned long long endTime;

    /**
     * Global level metrics for all profiled/instrumented functions/blocks. 
     * @note This supports up to 4096 function/blocks being instrumented. This is mainly an arbitrary number chosen. It can be increased later if needed. 
     */
    GlobalPerformanceMeasurement globalMeasurements[4096];

 };

 static GlobalProfiler Profiler;
 static InvocationPerformanceMeasurement *CurrentParent;

 #if PROFILE
 
 #define CONCAT(a, b) a##b
 #define ConcatName(a, b) CONCAT(a, b)
 #define TimeFunction InvocationPerformanceMeasurement ConcatName(fun_, __LINE__) = InvocationPerformanceMeasurement(__func__, __COUNTER__);
 #define TimeBlock(label) InvocationPerformanceMeasurement ConcatName(block_, __LINE__) = InvocationPerformanceMeasurement(label, __COUNTER__);
 #define TimeDataThroughput(label, size) InvocationPerformanceMeasurement ConcatName(block, __LINE__) = InvocationPerformanceMeasurement(label, __COUNTER__, size);
 
 #else

 #define TimeFunction
 #define TimeBlock(...)
 #define TimeDataThroughput(...)

 #endif

 void BeginProfile();
 void EndProfile();
 void PrintResults();
 void PrintGlobalMetrics(unsigned long long globalElapsed, unsigned long long globalElapsedTime);