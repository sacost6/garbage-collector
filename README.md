# garbage-collector

## Project Background
Dynamic memory (i.e. memory accessed via the malloc and free family of commands) is essential to many programs in the freedom, control, and interpretability it allows. We’ve talked about 2 different types of management for dynamic memory: implicit allocators and explicit allocators. This assignment focuses on the implicit allocator, perhaps better known as the garbage collector. Intuitively, we want this tool to find the blocks (or chunks as we refer to them) that are not being used and return them to the process as free chunks.

## Project Summary
In this project, we build a basic, conservative garbage collector for C programs. Starting from the set of root pointers present in the stack and global variables, we traverse the “object graph” (in the case of malloc, a chunk graph) to find all chunks reachable from the root. These are marked using the third lowest order bit in the size field of each chunk.

## Some Requirements:
* The program should free any unused chunks of memory by scanning through the stack and heap.
* When a chunk is freed, any additional unused chunks within reach should also be freed.
* Topics Learned Upon Completion
* Basic garbage collecting mark and sweep algorithm
* Memory structures in C
* Applications of bitwise operations
* Pros and Cons of garbage collecting in C

## Note
In order to run, all you have to do is run the makefile on the command line.
