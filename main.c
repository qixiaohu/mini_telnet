#include "server.h"
#include "fun.h"

/*
** Name: init_daemon
** Function: turn the process into a daemon process.
** Parameter: void
**Return value; 0, success.
*/
int init_daemon()
{
	pid_t pid;
	int stdin_fd;
	int stdout_fd;
	int stderr_fd;
	int i;

	pid=fork();
	if(pid>0)
		exit(0);
	else if(pid<0)
		exit(-1);

	setsid();

	signal(SIGHUP, SIG_IGN);

	if(pid=fork())
		exit(0);
	else if(pid<0)
		exit(-1);

	for(i=0; i<4; i++)
		close(i);
	chdir("/root/Documents/mini_telnet/");
	stdin_fd=open("stdin.log", O_WRONLY|O_CREAT, 0600);
	stdout_fd=open("stdout.log", O_WRONLY|O_CREAT, 0600);
	stderr_fd=open("stderr.log", O_WRONLY|O_CREAT, 0600);
	dup2(stdin_fd, 0);
	dup2(stdout_fd, 1);
	dup3(stderr_fd, 2);

	umask(0);

	return 0;
}

int main()
{
	init_daemon();
	server();
	return 0;	
}
