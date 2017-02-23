#include "client.h"
//#define IP 192.168.1.120

int main(int argc,char **argv)
{
	
	int ret;
	struct sockaddr_in server_add,client_add;
	struct hostent *host_info;
	char buff[10];
	if(argc!=2)
	{
		printf("No host name or IP address found.\n");
		exit(-1);
	}

	/*find the server*/
    //gethostbyname()返回对应于给定主机名的包含主机名字和地址信息的hostent结构指针结构的声明与gethostbyaddr()中一致
	if((host_info=gethostbyname(argv[1]))==NULL)
	{
		printf("server not found.\n");
		exit(-1);
	}

	/*create the socket*/
	if((client_sock=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		perror("socket");
		exit(-1);
	}
	
	/*fill the server data*/
	bzero(&server_add,sizeof(server_add));
	server_add.sin_family=AF_INET;
	server_add.sin_port=htons(PORT);
	server_add.sin_addr=*((struct in_addr *)host_info->h_addr);

	/*connect*/
	if(connect(client_sock,(struct sockaddr *)&server_add,sizeof(struct sockaddr))==-1)
	{
		perror("conncet");
		exit(-1);
	}

    //三秒发送一个心跳包
	signal(SIGALRM, sig_handler);
	alarm(3);

	while(1)
	{
		ret=menu();
		if(ret==-1)	//quit the client.
		{
			memset(buff, '\0', 10);
			strcat(buff, CMD);
			strcat(buff, "exit");
			send(client_sock, buff, 10, 0);	//break up the connect
			close(client_sock);
			printf("\nclient quit successfully.\n");
			break;
		}
	}
	return 0;
}

/*
** Name: sig_handler
** Function: send heart packet to server
** Parameter: signo, signal number
** Return value: void
*/
void sig_handler(int signo)
{
	if(signo==SIGALRM)
	{
		send(client_sock, HEART, 1, 0);
	}
	alarm(3);
}

/*
** Name: menu.
** Function: display the main list for choosing.
** Parameter: void.
** Return Value: if -1, quit the client. if 0, success.
*/
int menu()
{
	int choose;
	system("clear");

	printf("\n\n   ****** The remote terminal ******\n");
	printf("\n    1. Login.");
	printf("\n    2. Regist.");
	printf("\n    3. Quit.");
	printf("\n\nPlease choose you choice-->");
	scanf("%d",&choose);
//	fflush(stdin);

	switch(choose)
	{
		case 1:
			login();
			break;
		case 2:
			regist();
			break;
		case 3:
			return -1;
		default:
			printf("\nWrong Selection!!!\n");
	}
	return 0;
}

/*
** Name: get_passwd.
** Function: get the password from stdin.
** Parameter: passwd.
** Return Value: if success, the length of passwd.
** PS:
**    the length of password should more than 6 and less than 10.
*/
int get_passwd(char *passwd)
{
	int c;
	int n=0;
	printf("passwd -->");
	do
	{
		c=getchar();
		if(c!='\n')
		{
			passwd[n++]=c;
		}
	}while(c!='\n' && n<MAXSIZE-1);
	passwd[n]='\0';
	return n;
}

/*
** Name: set_disp_mode
** Function: control the ECHO.
** Parameter: fd; 
**           option: 0, close echo; 1,open echo
** return value: 0, success. -1, failed.
*/
int set_disp_mode(int fd,int option)
{
	int err;
	struct termios term;
    //tcgetattr()函数用于获取与终端相关的参数，参数fd为终端的文件描述符，返回的结果过保存在termios结构体中
	if(tcgetattr(fd,&term)==-1)
	{
		perror("Cannot get the attribution of the terminal\n");
		return -1;
	}
	
	if(option)
		term.c_lflag|=ECHOFLAGS;
	else
		term.c_lflag&=~ECHOFLAGS;

    //设置终端参数的函数
	err=tcsetattr(fd,TCSAFLUSH,&term);
	if(err==-1&&err==EINTR)
	{
		perror("Cannot set the attribution of the terimnal\n");
		return -1;
	}	

	return 0;
}

/*
** Name: login
** Function: check user name and password when user login
** Parameter: void
** return value: 0, success. -1, failed.
*/
int login()
{
	char name[MAXSIZE];
	char passwd[MAXSIZE];
	char buff[BUFFER_SIZE];
	int ret;
	char data[BUFFER_SIZE];
	
	system("clear");
	printf("\n\n   ****** User Login ******\n\n");

	memset(name, '\0', MAXSIZE);
	get_name(name);

	while(1)	//get password.
	{
		set_disp_mode(STDIN_FILENO, 0);	//close echo
		memset(passwd, '\0', MAXSIZE);
		ret=get_passwd(passwd);		//get password
		set_disp_mode(STDIN_FILENO, 1);	//open echo
		if(ret<6)
		{
			printf("Illegal password, too short! Please Retry.\n");
			continue;
		}
		else
		{
			break;
		}
	}	//end of get password.
	
	/* send info to the server */
	memset(buff,'\0',BUFFER_SIZE);
	strcat(buff,LOGIN);
	strcat(buff,name);
	strcat(buff,",");
	strcat(buff,passwd);
	if(send(client_sock,buff, BUFFER_SIZE, 0)==-1)	//send packet
	{
		printf("\nSend error\n");
		return -1;
	}

	/* get result from server*/
	memset(buff, '\0', BUFFER_SIZE);
	recv(client_sock, buff, BUFFER_SIZE, 0);
	switch(buff[0])
	{
		case '0':
			printf("\n--> User name not found, please login again or register <--\n");
			printf("-->Press Enter to continue...");
			getchar();
			break;
		case '1':
			printf("\n-->Password is not correct, please try again <--\n");
			printf("-->Press Enter to continue...");
			getchar();
			break;
		case '2':
			printf("\n--> Login Successfully! <--\n");
			printf("-->Press Enter to continue...");
			getchar();
			user_menu();
			break;
		default: NULL;
	}
//	free(data);
	return 0;
	
}

/*
** Name: get_name
** Function: get user name from stdin.
** Parameter: name, a pointer point to the place to store the name
** Return Value: 0, success
*/
int get_name(char *name)
{
	while(1)	//get user name.
	{
		printf("username-->");
		scanf("%s",name);
		while(getchar()!='\n');
	
		if(strlen(name)>(MAXSIZE-1))
		{
			printf("\nIllegal user name, please retry.\n");
			continue;
		}
		else
		{
			break;
		}
	}	//end of while
	return 0;
}

/*
** Name: user_menu
** Function: the menu for user to operate when login successed
** Parameter: none
** Return Value: 
*/
int user_menu()
{	
	char buff[BUFFER_SIZE];
	char tmp[BUFFER_SIZE];
	int n, quit;
	
	quit=1;
	system("clear");
	while(quit)
	{
		
		printf("\n\n   ****** Client menu ******\n\n");
		printf("    1. Send shell command.");
		printf("\n    2. Send a message.");
		printf("\n    3. Logout.");
		printf("\n\nPlease choose an operation-->");
		scanf("%d",&n);
		switch(n)
		{
			case 1:
				shell_cmd();
				break;
			case 2:
				printf("\nMessage-->");
				memset(tmp, '\0', BUFFER_SIZE);
				scanf("%s",tmp);
				getchar();
				memset(buff, '\0', BUFFER_SIZE);
				strcat(buff, MSG);
				strcat(buff, tmp);
				if(send(client_sock, buff, BUFFER_SIZE, 0)==0)
				{
					printf("Send success, Press Enter to continue...");
					getchar();
				}
				break;
			case 3:
				send(client_sock, LOGOUT, 1, 0);
				quit=0;
				break;
			default: NULL;
		}
	}
	
	return 0;

}
/*
** Name: shell_cmd
** Function: send shell command and recive result
** Parameter: none
** Return Value: 0, normal; -1 error happened.
*/
int shell_cmd()
{
	char buff[BUFFER_SIZE];
	char tmp[BUFFER_SIZE];
	while(1)
	{
		printf("\nclient @server-->");
		memset(tmp, '\0', BUFFER_SIZE);
		scanf("%s",tmp);		
		fflush(stdin);
		if(strcmp(tmp, "exit")==0)
		{
			printf("Quit Shell.\n");
			break;
		}
		
		memset(buff, '\0', BUFFER_SIZE);	//packup command.
		strcat(buff, CMD);
		strcat(buff, tmp);

		if(send(client_sock, buff, BUFFER_SIZE, 0)<0)	//send
		{
			perror("send_pack error");
			return -1;
		}
		memset(buff, '\0', BUFFER_SIZE);
		while(recv(client_sock, buff, BUFFER_SIZE, 0))	//get result from server.
		{	
			if(bcmp(buff, "SEND_END", 8)==0)
				break;
			printf("%s", buff);
			memset(buff, '\0', BUFFER_SIZE);
		}
	}

	return 0;
}

/*
** Name: regist
** Function: new user to register a account
** Parameter: none
** Return value: 0, succcess. -1, error happend
*/
int regist()
{
	char name[MAXSIZE];
	char pass1[MAXSIZE], pass2[MAXSIZE];
	char buff[BUFFER_SIZE];

	while(1)
	{
		memset(name, '\0', MAXSIZE);	//get name
		get_name(name);

		while(1)	//get password
		{
			set_disp_mode(STDIN_FILENO, 0);	//close echo
			memset(pass1, '\0', MAXSIZE);
			get_passwd(pass1);		//get password
			set_disp_mode(STDIN_FILENO, 1);	//open echo
			printf("\nPlease type in again.\n");
			set_disp_mode(STDIN_FILENO, 0);	//close echo
			memset(pass2, '\0', MAXSIZE);
			get_passwd(pass2);		//get password
			set_disp_mode(STDIN_FILENO, 1);	//open echo

			if(!strcmp(pass1, pass2))
			{
				break;
			}
			else
			{
				printf("\nPassword incorrect.Please try again.\n");
			}
		}
		/* send info to the server */
		memset(buff,'\0',BUFFER_SIZE);
		strcat(buff, REG);
		strcat(buff,name);
		strcat(buff,",");
		strcat(buff,pass1);
		if(send(client_sock, buff, BUFFER_SIZE, 0)==-1)	//send packet
		{
			printf("\nSend error\n");
			return -1;
		}

		/* get result from server*/
		memset(buff, '\0', BUFFER_SIZE);
		recv(client_sock, buff, BUFFER_SIZE, 0);
		if(buff[0]=='0')
		{
			break;	
		}
		else
		{
			printf("\nUser name has existed!!! Please try again.\n");
			printf("\nPress enter to contine...");
			getchar();
			return -1;
		}
	}

	printf("\n--> Regist success. <--");
	printf("\n--> Press enter to continue. <--");
	getchar();
	user_menu();
	return 0;
}
