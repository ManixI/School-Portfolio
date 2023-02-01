#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/limits.h>

#include "myftp.h"

#define DEBUG 0

int main (int argc, char *argv[]) {

	if (argc < 2) {
		printf("Error: no host specified for server\n");
		return -1;
	}

	if(DEBUG) {printf("connecting to host [%s]\n", argv[1]);}

	// setup command socket
    int cfd, sockid;
    struct sockaddr_in serv_addr;
    if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
 
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) < 0) {
        perror("inet error");
        return -1;
    }
 
    if ((cfd = connect(sockid, (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        perror("connection error");
        return -1;
    }

    if(DEBUG) {
	    printf("connection successfull\n");
	    printf("%d\n", sockid);
    }

	// possible commands for execvp:
	char *ls[] = {"ls","-l", NULL};
	char *more_1[] = {"more", "-20", ".tmp", NULL};		// use one of these, delete the other

	if(DEBUG) {
		printf("entering main loop\n");
	}

	// buffer for reading user input
	char buffer[256];
	char *cmd;

	// main loop
	while(1) {

		// empty buffer
		// get user input
		printf("MFTP-> ");
		scanf("%[^\n]%*c", buffer);
		if(DEBUG) {printf("buffer: [%s]\n",buffer);}

		// if exit command
		if (strcmp(buffer, "exit") == 0) {
			if(DEBUG) {printf("exit\n");}
			// send exit to server
			send_command_to_server(sockid, "Q", NULL, NULL);			
			break;
		}
//------------------------------------------------------------------------------------------------
		// ls command
		else if (strcmp(buffer, "ls") == 0) {

			if(DEBUG) {printf("ls\n");}

			FILE *fp = fopen(".tmp", "w+");

			int fd[2];
			pipe(fd);

			// write ls -l to file
			if (fork() != 0) {
				// parent
				// write ls contents to tmp file
				close(fd[1]);
				int in = dup(fd[0]);
				close(fd[0]);

				// read pipe to file
				char buff[2] = {0};
				while (read(in, buff, 1)) {
					fputs(buff, fp);					
				} 
			} else {
				// child
				close(fd[0]);
				close(STDOUT_FILENO);
				int out = dup(fd[1]);
				close(fd[1]);

				// call ls, write output to parrent
				if(execvp(ls[0], ls) == -1) {
					char *er = strerror(errno);
		            write(out,er,strlen(er));
		            write(out,"\n",1);
		            exit(-1);
				}
			}

			fclose(fp);
			int pid;

			// child calls more on .tmp file, pauses parent until ends
			if ((pid = fork()) != 0) {
				// parent
				if(DEBUG) {printf("waiting on children to end\n");}
				waitpid(pid, NULL, 0);
				remove(".tmp");
			} else {
				// child
				if (execvp(more_1[0], more_1) == -1) {
					perror("More error: ");
					exit(-1);
				}
			}

		}
//--------------------------------------------------------------------------------------------------
		// ls remote command
		else if (strcmp(buffer, "rls") == 0) {
			if(DEBUG) {printf("remote ls\n");}

			int data_sockid = client_get_data_port(sockid, argv[1]);
			if (data_sockid < 0) {
				// error print statement already in func
				continue;
			}

			// send command to server
			send_command_to_server(sockid, "L", NULL, NULL);

			FILE *fd = fopen(".tmp", "w+");

			if (fork() != 0) {
				// parent
				// just wait for child to end then cleanup
				wait(NULL);
				close(fileno(fd));
				remove(".tmp");
				close(data_sockid);
			} else {
				// child
				// execute more in child fork
				read_from_port(data_sockid, fileno(fd));

				if(execvp(more_1[0], more_1)) {
					char *err = "error executing more\n";
					write(STDOUT_FILENO, err, strlen(err));
					exit(-1);
				} 
			}
		}
//--------------------------------------------------------------------------------------------------------
		// don't split buffer until all single word commands have been checked
		cmd =  strtok(buffer, " ");
		// cd command
		if (strcmp(buffer, "cd") == 0) {

			if (DEBUG) {printf("cd\n");}

			cmd = strtok(NULL, " ");
			// various error checking
			if (!cmd) {
				printf("%s is not a valid command\n", cmd);
				continue;
			} 
			if (access(cmd, F_OK) != 0) {
				printf("%s dose not exist\n", cmd);
				continue;
			} 
			if (access(cmd, R_OK) == -1) {
				printf("could not access %s\n", cmd);
				continue;
			} 

			if (strlen(cmd) > PATH_MAX) {
				printf("%s is too long, cannot access dirrectory\n", cmd);
				continue;
			}
			if (chdir(cmd) != 0) {
				perror("change dir error: ");
				continue;
			}			
		}
//-----------------------------------------------------------------------------------------------------
		// cd remote command
		else if (strcmp(cmd, "rcd") == 0) {

			if (DEBUG) {printf("remote cd\n");}

			cmd = strtok(NULL, " ");
			if (!cmd) {
				printf("%s is not a valid command\n", buffer);
				continue;
			} 

			// send cd command and target to server
			int tmp = send_command_to_server(sockid, "C", cmd, NULL);
			if (tmp != 0) {
				// perror comes from send cmd func
				continue;
			}
		}
//-------------------------------------------------------------------------------------------------------
		// get command
		else if (strcmp(buffer, "get") == 0) {
			if (DEBUG) {printf("get\n");}

			cmd = strtok(NULL, " ");
			if (!cmd) {
				printf("%s is not a valid command\n", buffer);
				continue;
			}
			// get filename from user input
			char *rest = cmd;
			char *filename = strtok_r(cmd, "/", &rest);
			FILE *fp = fopen(filename, "w");				
			if (!fp) {
				perror("open file error: ");
				continue;
			}

			int data_sockid = client_get_data_port(sockid, argv[1]);
			if (data_sockid < 0) {
				// error print statement in get data port func
				continue;
			}

			int val = send_command_to_server(sockid, "G", cmd, NULL);
			if (val) {
				continue;
			}

			// read data from server to file
			if (read_from_port(data_sockid, fileno(fp)) != 0) {
				printf("Error reading data from server");
				close(data_sockid);
				continue;
				// placeholder for error code
			}

			close(data_sockid);
		}
//-----------------------------------------------------------------------------------------------
		// show command
		// displays file
		else if (strcmp(buffer, "show") == 0) {

			if (DEBUG) {printf("show\n");}

			cmd = strtok(NULL, " ");
			if (!cmd) {
				printf("%s is not a valid command\n", buffer);
				continue;
			}

			int data_sockid = client_get_data_port(sockid, argv[1]);
			if (data_sockid < 0) {
				continue;
			}
			if (DEBUG) {printf("established data connection\n");}

			int val = send_command_to_server(sockid, "G", cmd, NULL);

			if (val) {
				printf("error sending command\n");
				close(data_sockid);
				continue;
			}
			if (data_sockid < 0) {
				printf("Error");
				continue;
			}

			// open temp file to read and write to
			FILE *fd = fopen(".tmp", "w+");

			if (fork() != 0) {
				// parent

				if (DEBUG){printf("waiting on children\n");}
				wait(NULL);
				if (DEBUG){printf("children have ended\n");}
				close(fileno(fd));
				remove(".tmp");
				close(data_sockid);
			} else {
				read_from_port(data_sockid, fileno(fd));

				if(execvp(more_1[0], more_1)) {
					char *err = "error executing more\n";
					write(STDOUT_FILENO, err, strlen(err));
					exit(-1);
				} 
			}
		}
//----------------------------------------------------------------------------------------------------
		// put command
		else if (strcmp(buffer, "put") == 0) {
			if (DEBUG) {printf("put\n");}

			cmd = strtok(NULL, " ");
			if (!cmd) {
				printf("%s is not a valid command\n", buffer);
				continue;
			}

			// get file name from user input
			char *rest = cmd;
			char *filename = strtok_r(cmd, "/", &rest);
			if(DEBUG) {printf("sending file %s\n", filename);}
			FILE *fp = fopen(filename, "r");
			if (!fp) {
				perror("open file error");
				break;
			}

			int data_sockid = client_get_data_port(sockid, argv[1]);
			if (data_sockid < 0) {
				continue;
			}

			if (send_command_to_server(sockid, "P", cmd, NULL) != 0) {
				printf("Error reading data from server");
				close(data_sockid);
				continue;
			}
			
			write_to_port(data_sockid, fileno(fp));

			close(fileno(fp));
			close(data_sockid);
		}
		// error
		else {
			printf("[%s] is not a valid command\n", buffer);
		}
	}
	if (DEBUG) {printf("Ending cliend\n");}
	// cleanup memory
	// close connection
	close(cfd);

	return 0; 
}

//-----------------------------------------------------------------------------------------------

/**
 * send a command to the server
 * 
 * @param {int} sockid for server
 * @param {char*} command char to send
 * @param {char*} command test to append to char
 * @param {char*} buffer to save server response to
 * 
 * @return {int} 0 if exits properly
 */
int client_get_data_port(int sockid, char *host) {
	if (DEBUG){printf("client get data port\n");}

	// send request to server
	char *reciept = calloc(sizeof(char), 256);
	int val = send_command_to_server(sockid, "D", NULL, reciept);
	if (DEBUG){printf("got data ack: %s\n",reciept);}
	if (val) {
		if (DEBUG){printf("error from server getting dataport\n");}
		return -1;
	}

	// drop newline and command chars
	drop_first(reciept);
	drop_newline(reciept);
	if (DEBUG){printf("data socket: %s\n", reciept);}
	
	// convert string to int
	int dataport = atoi(reciept);
	if (DEBUG){printf("data int %d\n", dataport);}

	int sockid_d[2];
    struct sockaddr_in serv_addr;
    if ((sockid_d[0] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return -1;
    }
 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(dataport);
 
    if (inet_pton(AF_INET, host, &serv_addr.sin_addr) < 0) {
        perror("inet error");
        return -1;
    }
 
    if ((sockid_d[1] = connect(sockid_d[0], (struct sockaddr*)&serv_addr,sizeof(serv_addr))) < 0) {
        perror("connection error");
        return -1;
    }

	return sockid_d[0];

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
	if (DEBUG){printf("reading from port %d\n", sockid);}

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
 * @param {int} port to read from
 * @param {int} file descriptor for output
 * 
 * @return {int} error code, 0 if exist properly
 */int write_to_port(int sockid, int fd_in) {
	if (DEBUG){printf("writing to port %d\n", sockid);}

	char buf[2] = {0};
	while(read(fd_in, buf, 1)) {
		//printf("%s",buf);
		send(sockid, buf, 1, 0);
		}

	if (DEBUG){printf("sent everything\n");}
	// send null terminator just to be sure
	return 0;
}

/**
 * send a command to the server
 * 
 * @param {int} sockid for server
 * @param {char*} command char to send
 * @param {char*} command test to append to char
 * @param {char*} buffer to save server response to
 * 
 * @return {int} 0 if exits properly
 */
int send_command_to_server(int sockid, char *cmd, char *data, char *message) {
	// send command to server
	if (DEBUG){printf("sending command to server\n");}

	int len = strlen(cmd) + 3;
	if (data) {
		len += strlen(data);
	}

	// build command string to send
	char *command = calloc(sizeof(char), len);
	strcpy(command, cmd);
	if (data) {strcat(command, data);}
	strcat(command, "\n");
	if (DEBUG){printf("sent: %s", command);}

	// send string
	send(sockid, command, strlen(command), 0);
	free(command);
	char buf[256] = {0};
	char tmp[2] = {0};

	// wait for ack
	if (DEBUG){printf("waiting on ack\n");}

	while(read(sockid, tmp, 1)) {
		strcat(buf, tmp);
		//printf("[%s]\n",tmp);
		if (tmp[0] == '\n' || tmp[0] == '\0') {
			break;
		}
	}

	// check for error
	drop_newline(buf);
	if (DEBUG){printf("server sent: [%s]\n", buf);}
	if (buf[0] != 'A') {
		printf("server error: %s\n", buf);
		return -1;
	}
	if (message) {strcpy(message, buf);}
	return 0;
}
//--------------------------------------------------------------------------------------------------------
// take a string and replace the first \n with a \0
// for dropping \n on server acks
void drop_newline(char *buf) {
	for (int i=0; i<strlen(buf);i++) {
		if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		}
	}
}

// remove the first char of the string
void drop_first(char *str) {
	memmove(str, str+1, strlen(str));
	if (DEBUG){printf("string after drop first: [%s]\n", str);}
}