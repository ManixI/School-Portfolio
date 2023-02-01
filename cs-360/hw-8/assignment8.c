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
	if (root == NULL) {
		// first node of linked list
		root = (struct node *) malloc(sizeof(struct node));
		root->next = NULL;
		root->count = 1;
		root->host = (char *) calloc(NI_MAXHOST, sizeof(char));
		strcpy(root->host, hostid);
		return 1;
	}
	if (strcmp(root->host, hostid) == 0) {
		// increment count
		root->count++;
		return root->count;
	}
	return insert_host(hostid, root->next);
}


int main(int argc, char const *argv[]){
    //You know the drill :)
    if (argc == 2) {
    	// start server
    	int sockid;
    	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) != 0) {
    		perror("socket error");
    		exit(2);
    	}
    	if (setsockopt(sockid, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) != 0 ) {
    		perror("setsockopt");
    		exit(6);
    	}

    	struct sockaddr_in address;
    	memset(&address, 0, sizeof(address));
    	address.sin_family = AF_INET;
    	address.sin_addr.s_addr = htonl(INADDR_ANY);
    	address.sin_port = htons(PORT);

    	if (bind(sockid, (struct sockadder *)&address, sizeof(address)) != 0) {
    		perror("bind error");
    		exit(3);
    	} 
    	//struct node *root;
    	//root->host = (char *) calloc(NI_MAXHOST, sizeof(char));
    	//root->next = NULL;
    	root = NULL;

    	int new, val, status;
    	int adr_len = sizeof(address);
    	pid_t tmp_pid;
    	struct sockaddr_in client;
    	char host_buf[NI_MAXHOST];
    	int host_entry;
    	struct node cursor;
    	while(1) {
    		if (listen(sockid, 11) != 0) {
    	    		perror("listen error");
    	    		exit(4);
    	    }
    	    if (new = accept(sockid, (struct sockaddr*)&address, &adr_len) < 0) {
    	    	perror("accept error");
    	    	exit(5);
    	    }

    	    // get host info
    	    if ((host_entry = getnameinfo(
    	    	(struct sockaddr*)&client, sizeof(client), host_buf, 
    	    	sizeof(host_buf), NULL, 0, NI_NUMERICSERV)
    	    	) != 0) {
    	    	fprintf(stderr, "%s\n", gai_strerror(host_entry));
    	    }
    	    // fork off proccess to talk to client
    	    if (fork() == 0) {
    	    	// child
    	    	val = read(new, host_buf, NI_MAXHOST);
    	    	while (val != 0) {
    	    		val = read(new, host_buf, NI_MAXHOST);
    	    	}
    	    	int count = insert_host(host_buf, root);
    	    	printf("%s %i\n", host_buf, count);
    	    	exit(0);
    	    } else {
    	    	// parent
    	    	// wait until child ends
    	    	// can remove if semaphore added for list insert
    	    	wait(&status);
    	    }
    	}

//----------------------------------------------------------------------------------------------------------

    } else if (argc == 3) {
    	// connect to server
    	char buffer[19] = {0};
    	char name[NI_MAXHOST] = {0};

    	if (gethostname(name, 256) != 0) {
    		perror("host name");
    	}
    	int sockid;
    	if ((sockid = socket(AF_INET, SOCK_STREAM, 0)) !=0) {
    		perror("socket error");
    	}

    	struct sockaddr_in server;
    	server.sin_family = AF_INET;
    	server.sin_port = htons(PORT);

    	if (inet_pton(AF_INET, argv[2], &server.sin_addr) !=0) {
    		perror("connection error");
    	}
    	send(sockid, name, strlen(name), 0);
    	int valread = read(sockid, buffer, 19);
    	while (valread != 0) {
    		valread = read(sockid, buffer, 19);
    	}

    } else {
    	// else error
    	printf("ERROR: wrong number of arguments");
    	return 1;
    }

    return 0;
}