#ifndef _SERVER_
#define _SERVER_

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

#define MSG 'm'
#define CMD 'c'
#define HEART 'h'
#define REG 'r'
#define LOGIN 'l'
#define LOGOUT 'o'
#define BUFFER_SIZE 1024
#define PORT 8888
#define LISTEN_NUM 5
#define LOG "server.log"
#define MAXSIZE 10

int server();
void write_log(char *data);	//write information
int process_cmd(int client_sock, char *buff);

#endif
