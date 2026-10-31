#pragma once 

#include "PerformanceUtil.h"

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

 /*
    TODO (Joe): Design the Data Structure to support this instrumentation based profiler
 */