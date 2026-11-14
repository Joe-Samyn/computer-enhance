# Analysis of Two Instrumentation Profiler Approaches 

## Approach #1 -> My Method

## Approach #2 -> Casey's Method 

### Assembly Notes 
- In C++, the first argument to a construction and function is `this`, a pointer to the object itself to reference instance data and functions. 

#### Constructor 
- Moving parameters into general purpose registers for use
- The index is shifted left by 5 (multiplied by 32) because each element in the global Profiler.globalMeasurements is 32 bytes
- Check if Profiler.globalMeasurements[index * 32 + 16] == 0x00
    - Why + 16? The array of global measurements is 16 bytes into the GlobalProfiler struct 
- Use rdtsc to get timestamp counter cycles for `start` property 
- Set this->index  `mov     DWORD PTR [rdi+8], esi`
- Set this->start to 64 bit value retrieved from rtdsc `mov     QWORD PTR [rdi], rax`
- Get the current global parent index `mov     eax, DWORD PTR "GlobalParentIndex"[rip]`
    - Don't be confused by this notation, think of this as `mov     eax, DWORD PTR [rip + <displacement of GlobalParentIndex>]`
- Store this->index as new GlobalParent `mov     DWORD PTR "GlobalParentIndex"[rip], esi`
- Capture this invocations parent index `mov     DWORD PTR [rdi+12], eax`
- Capture globalInclusive time from global Profiler `mov     rax, QWORD PTR "Profiler"[rcx+32]`
- Store this->rootInclusiveSnapshot as captured globalInclusive `QWORD PTR [rdi+16], rax`

#### Constructor Assembly Analysis 
- Memory is accessed 8 times in the constructor between:
    - `this`
    - GlobalParentIndex and instruction pointer relative addressing
    - Profiler and global static variable access
- 4 registers used (RAX, RDX, RSI, RCX)
- Memory Access


