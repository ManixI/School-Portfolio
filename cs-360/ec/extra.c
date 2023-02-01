#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DEBUG 1

char *client_str;
// global so sighadler can write to pipe
int alarm_time, in, sem;

// signal handler to print string on every SIGALARM
void print_on_alarm(int sig) {
	if(client_str == NULL) {
		return;
	}
	if(DEBUG) {printf("printing string\n");}
	printf("%s\n", client_str);
	fflush(stdout);
	alarm(alarm_time);
}

// signal handler to get string from user on SIGINT
void get_string_on_alarm(int sig) {
	if (client_str != NULL) {
		memset(client_str,0,sizeof(client_str));
	}
	
	char tmp[256];
	printf("Interrupt received, enter new message: ");
	fflush(stdout);
	scanf("%[^\n]",tmp);
	if(write(in, tmp, strlen(tmp)) < 0) {
		perror("Write error");
		exit(-2);
	}
	if(write(in, "\0", 1) < 0) {
		perror("Write error");
		exit(-2);
	}
	//if(DEBUG) {printf("wrote string %s\n",tmp);}

	// if command was exit, close after write
	if(strcmp(tmp, "exit") == 0) {
		if(DEBUG) {printf("exit sent\n");}
		close(in);
		// wait for child before ending
		wait(NULL);
		exit(0);
	}
	return;
}

void pipe_sig(int sig) {
	return;
}

int main(int argc, char *argv[]) {

	int fd[2];
	if(pipe(fd) < 0) {
		perror("Pipe error");
		return -1;
	}

	client_str = NULL;
	int pid;
	if((pid = fork())!=0)  {
		if(DEBUG) {printf("child pid: %i\n", pid);}
		// parrent
		close(fd[0]);
		//close(STDOUT_FILENO);
		in = dup(fd[1]);
		close(fd[1]);

		//wait(NULL);
		// write to child through pipe
		signal(SIGINT, get_string_on_alarm);
		while(1) {
			// wait until SIGINT, then write to pipe and send SIGFPE
			if(DEBUG) {printf("waiting on pause\n");}
			// wait for user input
			pause();
			/*
			char tmp[256];
			//printf("Interrupt received, enter new message: ");
			//fflush(stdout);
			scanf("Interrupt received, enter new message: %s",tmp);
			if(write(in, tmp, strlen(tmp)) < 0) {
				perror("Write error");
				exit(-2);
			}

			// if command was exit, close after write
			if(strcmp(tmp, "exit") == 0) {
				if(DEBUG) {printf("exit sent\n");}
				close(in);
				exit(0);
			}
			if(DEBUG) {printf("sending SIGFPE to child\n");}
			*/

			// sigal child to wake up
			if(DEBUG) {printf("sent sig to child\n");}
			if(kill(pid, SIGFPE)!=0) {
				perror("kill error");
			}
		}
		// I don't think you can actually get here
		printf("how did I get here?\n");
		close(in);
		return 1;

	} else {
		if(DEBUG) {printf("child\n");}
		// child
		close(fd[1]);
		//close(STDIN_FILENO);
		int out = dup(fd[0]);
		close(fd[0]);

		// ignore interups on child
		signal(SIGINT, SIG_IGN);
		signal(SIGFPE, pipe_sig);
		signal(SIGALRM, print_on_alarm);

		char char_buf[1];
		char tmp_char;
		char buf[256];
		char buf2[256];
		char *ptr;
		int tmp;
		char *token = "N/A";

		while(1) {
			if(DEBUG) {
				printf("child waiting on signle\n");
				fflush(stdout);
			}
			// wait for user input
			pause();
			//if(DEBUG) {printf("passed first pause\n");}
			// wait for pipe sig
			//pause();
			if(DEBUG) {printf("child recieved signle\n");}
			// read stirng from pipe after signle
			while(tmp = read(out, char_buf, 1)) {
				if (tmp < 0) {
					perror("Read error");
					close(out);
					exit(-3);
				}
				if(DEBUG) {printf("read %s\n",char_buf);}
				tmp_char = char_buf[0];
				strcat(buf, &tmp_char);

				if(DEBUG) {printf("string buffer %s\n",buf);}
				if(strcmp(char_buf, "\0") == 0) {
					if(DEBUG) {printf("read end of string from pipe\n");}
					// read end of string
					break;
				}
				char_buf[0] = 0;

			}
			// check first char of buffer
			//token = strtok(buf, " ");
			/*;
			for (int i = 1; i < strlen(token)-1; i++)
				token[i-1] = token[i];
				*/
			//token[strlen(token)-1] = '\0';
			if(DEBUG) {printf("token: %s len: %li\n","n/a",strlen(token));}
			int num = (int)strtol(&buf[1], &ptr , 10);
			//printf("%s\n",&ptr[1]);


			//printf("%i, %s, %i\n", num, ptr, atoi(ptr));
			
			//int t = atoi("8");
			//printf("test int %i\n",t);
			//token[strlen(token)-1] = '\0';
			//int num = atoi(token);
			/*
			int len = strlen(buf);
			int num_flag
			for (int i = 0; i < len; i++) {
				if (buf[i] == ' ')
			}
			*/
			if(DEBUG) {printf("num %i\n",num);}
			if(num == 0) {
				if(DEBUG) {printf("num is 0\n");}
				// no leading character
				strcpy(buf2, ptr);
				ptr = "\0";
				// set alarm
				if(DEBUG) {printf("revieved %s\n",buf2);}
				alarm_time = 5;

			} else {
				if(DEBUG) {printf("found leading char\n");}
				// leading character
				//token = strtok(NULL, " ");
				//printf("%s\n", token);
				if(DEBUG) {printf("token %s\n",token);}
				// rebuild string without leadinc num
				strcat(buf2, &ptr[1]);
				//strcat(buf2, " ");

				// set alarm based on leading num
				if(num <= 0) {
					printf("Error: alarm time of %i\n is invalid, time must be > 0\n",num);
					printf("Using default alarm schedule of 5 sec\n");
					alarm_time = 5;
				} else {
					alarm_time = num;
				}
				
			}
			strcat(buf2, "\0");
			free(client_str);
			if(strcmp(buf2, "exit") == 0) {
				// end if exit command recieved
				printf("Recieved exit command\n");
				fflush(stdout);
				break;
			}
			alarm(alarm_time);
			// copy string to global for sig handler to print
			client_str = (char *) calloc(strlen(buf2)+1, sizeof(char));
			strcpy(client_str, buf2);

			// wipe buffers so strings don't get carries over
			memset(buf,0,sizeof(buf));
			memset(buf2,0,sizeof(buf2));
			memset(char_buf,0,sizeof(char_buf));
			memset(ptr,0,sizeof(ptr));
		}

		
		close(out);
		exit(0);
	}
}