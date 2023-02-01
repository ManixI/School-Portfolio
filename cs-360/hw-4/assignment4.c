/***********************************************************************
name:
    assignment4 -- acts as a pipe using ":" to seperate programs.
description:    
    See CS 360 Processes and Exec/Pipes lecture for helpful tips.
***********************************************************************/

/* Includes and definitions */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

/**********************************************************************
./assignment4 <arg1> : <arg2>

    Where: <arg1> and <arg2> are optional parameters that specify the programs
    to be run. If <arg1> is specified but <arg2> is not, then <arg1> should be
    run as though there was not a colon. Same for if <arg2> is specified but
    <arg1> is not.
**********************************************************************/

// heavy refrence taken from: https://gist.github.com/a-cordier/33211eda92b8084a9e7e


int main(int argc, char *argv[]){
    if (argc == 1) {
        fprintf(stderr,"%s: no arguments. propper format is:\n %s <arg1> <[flags]> : <arg2> <[flags]> \n", argv[0], argv[0]);
        return 1;
    }
    int fd[2];
    pipe(fd);
    int mid = 1;
    int colon = 0;
    // find dividing line
    while (mid < argc) {
        if (strcmp(argv[mid], ":")==0) {
            colon = 1;
            break;
        }
        mid++;
    }

    if (fork()) {
        // parent
        // output
        close(fd[1]);
        close(STDIN_FILENO);
        int in = dup(fd[0]);
        close(fd[0]);
        // ensure child has head start, may not be necessary
        int w = 1;
        wait(&w);

        if ((mid+1 != argc) && (colon == 1)){
            // if no right arg
            char **bffr = (char **) malloc(sizeof(char *) * (argc - mid));
            for (int i = mid+1; i < argc; i++) {
                bffr[i-mid-1] = malloc((strlen(argv[i])+1)*sizeof(char));
                strcpy(bffr[i-mid-1], argv[i]);
            }
            
            // print error if exec fails;
            if(execvp(bffr[0], bffr) == -1) {
                perror("");
            }
        } else {
            // else write right arg
            char buf[1];
            while(read(in,buf,1)) {
                // writes one char at a time, writing more then one results in 
                // garbage data where the buffer extends past last char in pipe
                if(write(STDOUT_FILENO, buf, 1)==-1){
                    perror("");
                    close(in);
                    exit(-1);
                }
            }
        }
        close(in);
        exit(0);
    } else {
        // child
        // input
        close(fd[0]);
        close(STDOUT_FILENO);
        int out = dup(fd[1]);
        close(fd[1]);

        char **bffr = (char **) malloc(sizeof(char *) * (mid));

        for (int i = 1; i < mid; i++) {
            bffr[i-1] = malloc((strlen(argv[i])+1)*sizeof(char));
            strcpy(bffr[i-1], argv[i]);
        }
        // make last element of buffer null so it works with execvp
        bffr[mid] = (char *) NULL;

        // print error if exec fails
        if(execvp(bffr[0], bffr) == -1) {
            char *er = strerror(errno);
            //printf("error: %s\n", er);
            write(out,er,strlen(er));
            write(out,"\n",1);
        }
        close(out);
        exit(0);
    }  
}
