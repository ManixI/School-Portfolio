#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/wait.h>

#define PORT 49999
#define DEBUG 0

// struct for linked list
struct node {
	char *host;
	int count;
	struct node *next;
} *root;

// free nodes in linked list recursivly
void free_nodes(struct node *root) {
	if (root->next != NULL) {
		free_nodes(root->next);
	}
	free(root->host);
	free(root);
}

// walk list recursivly until match for host id is found or end of list is reached
int insert_host(char *hostid, struct node *root) {
	if (root->host == NULL) {
		if (DEBUG) {printf("inserting first node\n");}
		// first node of linked list
		//root = (struct node *) malloc(sizeof(struct node));
		//root->next = NULL;
		root->next = (struct node *) malloc(sizeof(struct node));
		root->next->count = 0;
		root->next->next = NULL;
		root->next->host = NULL;
		root->count = 1;
		root->host = (char *) calloc(NI_MAXHOST, sizeof(char));
		strcpy(root->host, hostid);
		if (DEBUG) {printf("node: %s\n",root->host);}
		return 1;
	}
	if (strcmp(root->host, hostid) == 0) {
		// increment count
		root->count++;
		return root->count;
	}
	if (DEBUG) {printf("going deeper\n");}
	return insert_host(hostid, root->next);
}


int main(int argc, char const *argv[]){
    //You know the drill :)
    if (argc == 2) {
    	if (strcmp(argv[1], "server") != 0) {
    		fprintf(stderr, "command: %s not found, expected command: server or client <server-address>\n", argv[1]);
    		exit(5);
    	}
    	// start server
    	int sockid;
    	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    		perror("socket error");
    		exit(2);
    	}
    	if (DEBUG) {printf("got sockid\n");}
    	if (setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) != 0 ) {
    		perror("setsockopt");
    		exit(6);
    	}
    	if (DEBUG) {printf("set socket\n");}

    	struct sockaddr_in address;
    	memset(&address, 0, sizeof(address));
    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = htonl(INADDR_ANY);
    	address.sin_port = htons(PORT);

    	int tmp;
    	if ((tmp = bind(sockid, (struct sockadder *)&address, sizeof(address))) < 0) {
    		printf("%i\n",tmp);
    		perror("bind error");
    		exit(3);
    	} 
    	if(DEBUG) {printf("bound socket\n");}
    	root = (struct node *) malloc(sizeof(struct node));
    	root->next = NULL;
    	root->count = 0;
    	root->host = NULL;

    	if (DEBUG) {
    		char hostname[NI_MAXHOST];
    		int hostid = (getnameinfo(
    			(struct sockaddr*)&address, sizeof(address),hostname, 
    			sizeof(hostname), NULL, 0, NI_NUMERICSERV));
    		printf("%s\n", hostname);
    	}

    	int newfd, val, status;
    	int adr_len = sizeof(address);
    	pid_t tmp_pid;
    	struct sockaddr_in client;
    	char host_buf[NI_MAXHOST];
    	int host_entry;
    	struct node cursor;
    	if (listen(sockid, 1) != 0) {
   	    		perror("listen error");
   	    		exit(4);
   	    }
   	    if (DEBUG) {printf("listening\n");}
    	while(1) {
    		
    	    if ((newfd = accept(sockid, (struct sockaddr*)&address, &adr_len)) < 0) {
    	    	perror("accept error");
    	    	exit(5);
    	    }
    	    if (DEBUG) {printf("connected\n");}

    	    // get host info
    	    if ((host_entry = getnameinfo(
    	    	(struct sockaddr*)&address, sizeof(address), host_buf, 
    	    	sizeof(host_buf), NULL, 0, NI_NUMERICSERV)
    	    	) != 0) {
    	    	fprintf(stderr, "%s\n", gai_strerror(host_entry));
    	    }
    	    int count = insert_host(host_buf, root);
    	    if (DEBUG) {printf("node after insert: %s\n",root->host);}
    	    printf("%s %i\n", host_buf, count);
    	    fflush(stdout);
    	    if (DEBUG) {printf("got host info\n");}
    	    if (DEBUG) {printf("connected to %s\n", host_buf);}
    	    // fork off proccess to talk to client
    	    if (tmp_pid = fork()) {
    	    	//close(sockid);
    	    	// parent
    	    	// wait until child ends
    	    	// can remove if semaphore added for list insert
    	    	if (DEBUG) {printf("waiting on child\n");}
    	    	wait(NULL);
    	    } else {
    	    	if (DEBUG) {printf("forked child\n");}
    	    	// child
    	    	// get client name
    	    	if (0) {
	    	    	char *hi = "hi";
	    	    	int tmp = write(sockid, hi, strlen(hi));
    	    		printf("wrote hi to client\n");
    	    		close(sockid);
    	    	}

    	    	// send time to client
    	    	time_t t;
    	    	time(&t);
    	    	if (DEBUG) {printf("%s\n", ctime(&t));}
    	    	int tim;
    	    	char *time_str = ctime(&t);
    	    	if (tim = write(newfd, time_str, 18) < 0) {
    	    		perror("writing time error");
    	    		exit(1);
    	    	}
    	    	if (DEBUG) {printf("wrote first thing\n");}
    	    	if (tim != 18) {
    	    		//fprintf(stderr,"error: only wrote %i/18 bytes of time\n",tim);

    	    	}
    	    	if (tim = write(newfd, "\n", 1) < 0) {
    	    		perror("write error");
    	    		exit(1);
    	    	}
    	    	if (tim != 1) {
    	    		//fprintf(stderr, "didn't write \\n\n");
    	    	}
    	    	close(newfd);
    	    
    	    	exit(0);
    	    }
    	    
    	}

//----------------------------------------------------------------------------------------------------------

    } else if (argc == 3) {
    	if (strcmp(argv[1], "client") != 0) {
    		fprintf(stderr, "command: %s not found, expected command: server\n", argv[1]);
    		exit(5);
    	}
    	// connect to server
    	char buffer[19];
    	char name[NI_MAXHOST] = {0};

    	if (gethostname(name, 256) != 0) {
    		perror("host name");
    	}

    	struct sockaddr_in server;
    	server.sin_family = AF_INET;
    	server.sin_port = htons(PORT);

    	int sockid, err;
    	struct addrinfo hints, *actualdata;
    	memset(&hints, 0, sizeof(hints));

    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_family = AF_INET;

    	err = getaddrinfo(argv[2], "49999", &hints, &actualdata);
    	if (err != 0) {
    		fprintf(stderr, "%s\n", gai_strerror(err));
    		exit(1);
    	}
    	if (DEBUG) {printf("got addr info\n");}

    	if ((sockid = socket(actualdata->ai_family, actualdata->ai_socktype, 0)) < 0) {
    		perror("socket error");
    	}
    	if (DEBUG) {printf("got sockid\n");}
    	/*
    	if (inet_pton(AF_INET, argv[2], &server.sin_addr) !=0) {
    		perror("connection error");
    	}
    	*/
    	if (connect(sockid, actualdata->ai_addr, actualdata->ai_addrlen) < 0) {
    		perror("connect error ");
    		exit(2);
    	}
    	if (DEBUG) {printf("connected to server\n");}
    	// send name to server
    	//send(sockid, name, strlen(name), 0);

    	// read time from server
    	int valread;
    	if (valread = read(sockid, buffer, 19) < 0) {
    		perror("error reading socket");
    		exit(7);
    	}
    	if (DEBUG) {printf("read something\n");}
    	printf("%s\n",buffer);
    	while (valread != 0) {
    		if (valread = read(sockid, buffer, 19) < 0) {
    			perror("error reading socket");
    			exit(7);
    		}
    		
    		//fflush(stdout);
    	}
    	char to_print[19];
    	printf("%s", to_print);
    	close(sockid);

    } else {
    	// else error
    	printf("ERROR: wrong number of arguments");
    	return 1;
    }

    return 0;
}