#include "server.h"
#include "fun.h"

int server()
{
	struct sockaddr_in server_add, client_add;
	int server_sock, client_sock;
	
	int add_len;
	int ret;
	int ret_c=-1;
	int quit=1;
	pid_t pid;
	char pack_type;
	char *ip, buff[BUFFER_SIZE], data[BUFFER_SIZE];
	char name[MAXSIZE], passwd[MAXSIZE];
	INFO_NODE *info_h;
	unsigned value=1;
	
    //读取配置文件，验证用户身份
	info_h=load_info_lib();	//load client information
	/* create a socket */
	server_sock=socket(AF_INET,SOCK_STREAM,0);
	if(server_sock==-1)
	{
		perror("socket");
		return -1;
	}
	
    //端口复用
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(value));
	/* initialize the server_addr */
	bzero(&server_add, sizeof(server_add));
	server_add.sin_family=AF_INET;	//IPV4
	server_add.sin_addr.s_addr=htonl(INADDR_ANY);	//any client is permited
	server_add.sin_port=htons(PORT);

	/* bind the socket */
	if(bind(server_sock, (struct sockaddr *)&server_add, sizeof(server_add))==-1)
	{
		perror("bind");
		return -1;
	}
	
	/* listen */
	if(listen(server_sock,LISTEN_NUM)==-1)
	{
		perror("listen");
		return -1;
	}
	
	while(1)
	{
		/* accept */
		add_len=sizeof(struct sockaddr_in);
		client_sock=accept(server_sock, (struct sockaddr *)&client_add, &add_len);
		if(client_sock==-1)
		{
			perror("accept failed");
			return -1;
		}
		
        //将网络IP字节序转化为点分十进制IP
		ip=inet_ntoa(client_add.sin_addr);	//get client IP
		printf("server get connect with %s.\n",ip);
		
		/* write information of client*/

		memset(buff,'\0', BUFFER_SIZE);
		strcpy(buff,"server get connect with ");
		strcat(buff, ip);
        //Write log
		write_log(buff);

		signal(SIGCHLD,SIG_IGN);
        //fork a child process to deal with the client.
		if((pid=fork())==0)	
		{
			while(quit)
			{
				memset(buff, '\0', BUFFER_SIZE);
				recv(client_sock, buff, BUFFER_SIZE, 0);
				pack_type=buff[0];
				strcpy(data,buff+1);
				switch (pack_type)
				{
					case CMD:		//a command packet
						ret=process_cmd(client_sock, data);
						if(ret==1)	//client quit
						{
							printf("client has quited. Server is unlinking.\n");
							memset(buff,'\0', sizeof(buff));
							strcpy(buff, "client disconnect with server. Unlink with ");
							strcat(buff, ip);
							write_log(buff);
							
							free_chain(info_h);
							close(client_sock);
							quit=0;
						}
						break;
					case MSG:		//a message packet
						memset(buff,'\0',sizeof(buff));
						strcpy(buff,"Message from client:  ");
						strcat(buff, data);
						write_log(buff);
						break;
					case HEART:		//a heart beating packet
						
						break;
					case REG:		//new client register
						memset(name, '\0', MAXSIZE);
						memset(passwd, '\0', MAXSIZE);
						get_info(name, passwd, data);
						ret_c=check_info(info_h, name, passwd, 0);
						
						if(ret_c==0)	//save info when register success
						{
							insert_node(info_h, name, passwd);
							save_info(name, passwd);
						}
						memset(buff, '\0', sizeof(buff));
						snprintf(buff, BUFFER_SIZE, "%d", ret_c);
						send(client_sock, buff, BUFFER_SIZE, 0);  //send result to client
						break;
					case LOGIN:		//user login
						get_info(name, passwd, data);
						ret_c=check_info(info_h, name, passwd, 1);
						/*send check result to client*/
						memset(buff,'\0',sizeof(buff));
						snprintf(buff, BUFFER_SIZE, "%d", ret_c);
						send(client_sock, buff, BUFFER_SIZE, 0);
						break;
				//	case LOGOUT:
				//		break;
					default: NULL;
				}	//end of switch
				
			}	//end of while
			exit(0);
		}
	}
	return 0;
}

/*
** Name: write_log
** Function: record information when client login and logout.
** Parameter: data
** Return Value: void
*/
void write_log(char *data)
{
	int fd;
	time_t loctime;
	char *ptr;
	
	loctime=time(NULL);
	ptr=ctime(&loctime);

	fd=open(LOG,O_CREAT|O_WRONLY|O_APPEND,0600);
	write(fd, "\n\n",2);
	write(fd, ptr, strlen(ptr));	//write time
	write(fd, data, strlen(data));	//write information

	close(fd);	
}

/*
** Name: process_cmd
** Function: deal with the shell command which receive from client
** Parameter: client, buff
** Return value: -1, failed; 0, success; 1, the "exit" command.
*/
int process_cmd(int client_sock, char *buff)
{
	FILE *fp=NULL;
	char tmp[BUFFER_SIZE];
    //if client typed in "exit", then return
	if(bcmp(buff, "exit", 4)==0)
	{
		return 1;
	}

    //execute the shell command
    //popen()函数通过创建一个管道，调用fork()产生一个子进程
    //执行一个shell以运行命令来开启一个进程
	fp=popen(buff, "r");
	if(fp==NULL)
	{
		perror("popen");
		return -1;
	}
	memset(tmp, '\0', BUFFER_SIZE);
	while(fread(tmp, sizeof(char), BUFFER_SIZE, fp)!=0)	//read the result from the stream
	{
		if(send(client_sock, tmp, BUFFER_SIZE, 0)==-1)	//send result to client
		{
			perror("send_packet error");
			return -1;
		}
		memset(tmp, '\0', BUFFER_SIZE);
	}
	send(client_sock, "SEND_END", 8, 0);
	pclose(fp);
}

