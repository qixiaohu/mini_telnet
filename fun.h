#ifndef FUN_H
#define FUN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define LEN sizeof(struct info)
#define PATH "usr_info.txt"
#define MAXSIZE 10

typedef struct info
{
	char name[MAXSIZE];
	char passwd[MAXSIZE];
	struct info *next;
}INFO_NODE;

int get_info(char *name, char *passwd, char *data);	//get username and password from the packet
INFO_NODE *load_info_lib();	//load username and passwd
int insert_node(INFO_NODE *head, char *name, char *passwd);	//insert a new node info to the chain table
int free_chain(INFO_NODE *head);	//free the table chain
int save_info(char *name, char *passwd);	//save client info to local file
int check_info(INFO_NODE *inf_head, char *name, char *passwd, int option);	//check client information
#endif
