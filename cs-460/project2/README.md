# Project 2 - Process Scheduling
## Ian Manix

## Compile Options
 - to comile run `make`
 - to clean run `make clean`
 - to compile and run the test case with the RR alg, run `make test`

## To Run
 - command to run is `./project2 -alg [scheduler] -input [filename] -quant [quant size]`
 - quant size defaults to 10 if not provided
 - behavior is undefined if passed an unknown scheduler alg
 - Accepted scheduler options are:
    - FIFO: first in first out
    - FCFS: first come first serve, same as FIFO
    - SRTF: N/A
    - SJF:  shortest job first
    - RR:   round robin
    - PR:   highest priority first, 10 before 1


## Files
 - data/
    - test data
 - docs/
    - project requs
 - src/
    - main.c:              entry point to program
    - pcb.[c/h]            pcb struct to pass between threads
    - queue.[c/h]          priority queue
    - schedulers.[c/h]     scheduler thread
    - io.[c/h]             io thread
    - global.h             declares global flags used to track thread progress

 - bin/
    - holds .o files