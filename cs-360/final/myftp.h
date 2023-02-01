#ifndef MYFTP_H
#define MYFTP_H

#define PORT 49999


// sighandler to prevent spawning of daemons
void sighandler(int signum);

// take a string and replace the first \n with a \0
// for dropping \n on server acks
void drop_newline(char *buf);


// send ack from server to client
void send_ack(int sockid);


// send err from server to client
void send_err(int sockid, char *one, char* two, char* three);


// drop first char of string
void drop_first(char *str);


/** 
 * client side function, get data port for read/write
 * 
 * @param {int} sockid for communication
 * @param {char*} command to send to server (should be G or P)
 * 
 * @return {int} sockid for data
 */
int client_get_data_port(int sockid, char *host);


/** 
 * read server output and write it to file
 * 
 * @param {int} port to write to
 * @param {int} file descriptor to write to
 * 
 * @return {int} error code, 0 if exist properly
 */
int read_from_port(int sockid, int fd_out);


/** 
 * same as above but writes instead
 * NOTE: input is expected to come from pipe, call in fork
 * 
 * @param {int} port to read from
 * @param {int} file descriptor for output
 * 
 * @return {int} error code, 0 if exist properly
 */
int write_to_port(int sockid, int fd_in);

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
int send_command_to_server(int sockid, char *cmd, char *data, char *message);

#endif