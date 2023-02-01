#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/limits.h>

#include "myftp.h"

#define DEBUG 0

// TODO: test to fix any points where prog will hang because client closed unexpectedly

// basic semaphore for data channel
int new_dat;

// counts num children for exit
int num_children;

int main (void) {

	if(DEBUG) {printf("setting socket\n");}

	// setup command socket
	int sockid;
	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		return -1;
	}
	if (setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0 ) {
		perror("setsockopt");
		return -1;
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(PORT);

	if (bind(sockid, (const struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind error");
		return -1;
	} 

	if(DEBUG) {printf("socket set, listining for connection\n");}

	int adr_len = sizeof(address);

	int new;
	int dataflag = 0; 		// flag for data channel, 0 if open, otherwise closed
	char buf[256];
	char tmp[2];
	char *port_str;
	FILE *fp;

	// setup shutdown signal
	num_children = 0;
	signal(SIGINT, sighandler);

	char *ls[3] = {"ls", "-l", NULL};

	// main loop
	while(1) {
		// max backlog of 4
		if (listen(sockid, 4) != 0) {
			perror("listin error");
			continue;
		}
		if(DEBUG) {printf("listining for new connection\n");}

		// accept request from que
		if ((new = accept(sockid, (struct sockaddr*)&address, (socklen_t *)&adr_len)) < 0) {
	    	perror("accept error");
	    	continue;
    	}
    	// fork off child on accept
    	if (fork() == 0) {
    		num_children++;
	    	if(DEBUG) {printf("accepted new connection, waiting on inital read\n");}


	    	while (1) {
	    		// while client is writing to server
	    		memset(buf, 0, 256);
	    		if(DEBUG) {printf("reading command from client\n");}
	    		while (read(new, tmp, 1)) { 
	    			strcat(buf, tmp);
	    			if(tmp[0] == '\n') {
	    				break;
	    			}
	    			if(tmp[0] == '\0') {
	    				break;
	    			} 

	    		}
	    		// drop \n if in buf
	    		for (int i=0; i<strlen(buf);i++) {
	    			if (buf[i] == '\n') {
	    				buf[i] = '\0';
	    				if(DEBUG) {printf("found linebreak in command line\n");}
	    				break;
	    			}
	    		}
	    		if(DEBUG) {printf("recieved: [%s]\n", buf);}
	    		switch(buf[0]) {
	    			case 'D':
	    				if(DEBUG) {printf("receved 'D'\n");}
	    				// open data connection
						
						if (dataflag) {
							send_err(new, "data port already open\n", NULL, NULL);
							continue;
						}
						dataflag = 1;
					
						// open port for data
						int data_sockid;
				    	if ((data_sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				    		send_err(new, "socket error: ",strerror(errno), "\n");
				    		break;
				    	}
				    	if (setsockopt(data_sockid, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) != 0 ) {
				    		send_err(new, "set socket error: ",strerror(errno),"\n");
				    		break;
				    	}

				    	struct sockaddr_in adr;
				    	memset(&adr, 0, sizeof(adr));
				    	adr.sin_family = AF_INET;
				    	adr.sin_addr.s_addr = (50000+num_children);	// to help prevent conflicting ports

				    	if (bind(data_sockid, (const struct sockaddr *)&adr, sizeof(adr)) != 0) {
				    		send_err(new, "bind error: ",strerror(errno), "\n");
				    		break;
				    	}

				    	port_str = calloc(sizeof(char),15);
				    	char tmp_str[25];
				    	strcat(port_str, "A");
				    	sprintf(tmp_str, "%d", 50000+num_children);
				    	strcat(port_str, tmp_str);
				    	strcat(port_str, "\n");

				    	if(DEBUG) {printf("sending %s",port_str);}
				    	// send and cleanup
				    	send(new, port_str, strlen(port_str), 0); 

			    		// start listening to port 49998
			    		listen(data_sockid, 1);

			    		if ((new_dat = accept(data_sockid, (struct sockaddr*)&adr, (socklen_t *)&adr_len)) < 0) {
					    	perror("accept error");
					    	close(data_sockid);
				    	}

	    				break;
	//-------------------------------------------------------------------------------------------------------
	    			case 'C':
	    				if(DEBUG) {printf("recieved 'C'\n");}
	    				// cd command
	    				drop_first(buf);

	    				if (access(buf, F_OK) != 0) {
	    					send_err(new, "dir", " dose not exist\n", NULL);
							break;
						} 
						if (access(buf, R_OK) == -1) {
							send_err(new, "could not access dir: ", buf, "\n");
							break;
						} 

						if (strlen(buf) > PATH_MAX) {
							send_err(new, buf, " is too long, cannot access dirrectory\n", NULL);
							break;
						}
						if (chdir(buf) != 0) {
							send_err(new, "chdir error: ",strerror(errno), "\n");
							break;
						}

						send_ack(new);

	    				break;
	//----------------------------------------------------------------------------------------------------
	    			case 'L':
	    				if(DEBUG) {printf("recieved 'L'\n");}
	    				// ls command
	    				
	    				if (!dataflag) {
	    					send_err(sockid, "Error: data channle not open", NULL,NULL);
	    					break;
	    				}
	    				send_ack(new);
	    				if (fork() != 0) {
	    					// parent
	    					wait(NULL);
	    					wait(NULL);
	    					if(DEBUG) {printf("done waiting for children\n");}
	    				} else {
	    					if(DEBUG) {printf("forking children\n");}
	    					// child
	    					int fd1[2];
	    					pipe(fd1);
	    					if (fork() != 0) {
	    						// parent
	    						close(fd1[1]);
	    						close(STDIN_FILENO);
	    						int in = dup(fd1[0]);
	    						close(fd1[0]);
	    						write_to_port(new_dat, in);
	    						
	    						close(in);
	    						exit(0);                
	    					} else {
	    						// child
	    						//close(new_dat);
	    						close(fd1[0]);
						        close(STDOUT_FILENO);
						        //int out1 = dup(fd1[1]);
						        close(fd1[1]);

						        if (execvp(ls[0], ls) == -1) {
						        	send_err(new, "ls error: ",strerror(errno), "\n");
						        	exit(-1);
						        }
	    					}
	    					exit(-1);
	    				} 

	    				close(new_dat);
	    				close(data_sockid);
	    				dataflag = 0;
	    				break;
	//---------------------------------------------------------------------------------------------------    				
	    			case 'G':
	    				if(DEBUG) {printf("recieved 'G'\n");}
	    				// get command
	    				if (!dataflag) {
	    					send_err(new, "Error: data channle not open", NULL,NULL);
	    					break;
	    				}
	    				drop_first(buf);

	    				if(DEBUG) {printf("getting: %s\n", buf);}

	    				fp = fopen(buf, "r");
	    				if (!fp) {
	    					send_err(new, "could not open file: ",strerror(errno), "\n");
	    					close(new_dat);
	    					close(data_sockid);
	    					dataflag = 0;
	    					break;
	    				}
	    				send_ack(new);

	    				write_to_port(new_dat, fileno(fp));

	    				close(fileno(fp));
	    				close(new_dat);
	    				close(data_sockid);
	    				dataflag = 0;
	    				break;
	//--------------------------------------------------------------------------------------------------------
	    			case 'P':
	    				// TODO: Handles text but not binary
	    				if(DEBUG) {printf("recieved 'P'\n");}
	    				// put command
	    				if (!dataflag) {
	    					send_err(new, "Error: data channle not open", NULL,NULL);
	    					break;
	    				}
	    				drop_first(buf);

	    				if (access(buf, F_OK) == 0) {
	    					send_err(new, "Error: ",buf," already exists\n");
	    				}

	    				fp = fopen(buf, "w");
	    				if (!fp) {
	    					send_err(new, "could not create file: ",strerror(errno), "\n");
	    					close(new_dat);
	    					close(data_sockid);
	    					dataflag = 1;
	    					break;
	    				}
	    				send_ack(new);

	    				read_from_port(new_dat, fileno(fp));

	    				fclose(fp);
	    				close(new_dat);
	    				close(data_sockid);
	    				dataflag = 0;
	    				break;
	//---------------------------------------------------------------------------------------------------------
	    			case 'Q':
	    				if(DEBUG) {printf("recieved 'Q'\n");}
	    				// exit command
	    				send_ack(new);
	    				if (dataflag) {
	    					close(new_dat);
	    					close(data_sockid);
	    				}
	    				num_children--;
	    				exit(0);
	//--------------------------------------------------------------------------------------------------------
	    			default:
	    				// invalid command
	    				send_err(new, "Error: bad command\n", NULL, NULL);

	    		}
    		}
    	}
	}

}

// sighandler to prevent spawning of daemons
void sighandler(int signum) {
	printf("recieved ^C, waiting for all children to end before closeing\n");
	while(num_children) {
		// hold instead of just looping endlessly
		wait(NULL);
	}
	exit(0);
}

// send ack from server to client
void send_ack(int sockid) {
	if(DEBUG) {printf("sending ack\n");}
	send(sockid, "A\n", strlen("A\n"), 0);
	
}

// send err from server to client
void send_err(int sockid, char *one, char* two, char* three) {
	if(DEBUG) {printf("sending error\n");}
	char buf[512] = {0};
	strcat(buf, "E");
	if (one) {strcat(buf, one);}
	if(two) {strcat(buf, two); }
	if(three) {strcat(buf, three);}

	if(DEBUG) {printf("%s", buf);}

	send(sockid, buf, strlen(buf), 0);
	if(DEBUG) {printf("send error: %s", buf);}
}

// remove the first char of the string
void drop_first(char *str) {
	memmove(str, str+1, strlen(str));
	if(DEBUG) {printf("string after drop first: [%s]\n", str);}
}

/** 
 * read server output and write it to file
 * 
 * @param {int} port to write to
 * @param {int} file descriptor to write to
 * 
 * @return {int} error code, 0 if exist properly
 */
int read_from_port(int sockid, int fd_out) {
	if(DEBUG) {printf("reading from port %d\n", sockid);}

	char buf[1];
	// read data from server
	while(read(sockid, buf, 1)) {
		// write output to fd
		write(fd_out, buf, 1);

		if (buf[0] == '\0') {
			break;
		}
	}
	return 0;
}

/** 
 * same as above but writes instead
 * NOTE: input is expected to come from pipe, call in fork
 * 
 * @param {int} port to write to
 * @param {int} file descriptor for input
 * @param {int} file descriptor for output
 * 
 * @return {int} error code, 0 if exist properly
 */
int write_to_port(int sockid, int fd_in) {
	if(DEBUG) {printf("writing to port %d\n", sockid);}

	char buf[1];
	while(read(fd_in, buf, 1)) {
		send(sockid, buf, 1, 0);
		}
	if(DEBUG) {printf("sent everything\n");}
	// send null terminator just to be sure
	send(sockid, "\0", 2, 0);

	return 0;
}