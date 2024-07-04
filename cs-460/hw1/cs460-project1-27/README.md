Ian Manix

# Part 1
 - the command to toggle the trace is `toggletrace`
 - syscall_trace.c replaces sysproc.c, sysproc.c has the toggletrace as well but it is disabled by default
 - The other modified files are:
 	- Makefile
 	- syscall.h
 	- syscall.c
 	- usys.S
 	- toggletrace.c
 	- user.h

 # Part 2
  - the command is 'ps'
  - the modified files are:
  	- Makefile
 	- syscall.h
 	- sysproc.c
 	- usys.S
 	- uproc.h
 	- ps.c
