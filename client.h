#ifndef _CLIENT_
#define _CLIENT_

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
#include <termios.h>
#include <signal.h>

#define ECHOFLAGS (ECHO|ECHOE|ECHOK|ECHONL)	//used in set_disp_mode
#define MAXSIZE 10	//the length of user name and password.
#define MSG "m"
#define CMD "c"
#define HEART "h"
#define REG "r"
#define LOGIN "l"
#define LOGOUT "o"
#define BUFFER_SIZE 1024
#define PORT 8888
#define LISTEN_NUM 5

int client_sock;

int menu();	//display the main menu
int get_passwd(char *passwd);	//get password from stdin
int login();	//user login the server.
int set_disp_mode(int fd,int option);	//to control the ECHO
int get_name(char *name);	//get user name
int user_menu();	//the menu for user to operate when login successfully
int shell_cmd();	//deal with the shell command that user want to send.
int regist();	//new user register
void sig_handler(int signo); 	//send heart packet to server
#endif
