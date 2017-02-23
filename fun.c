#include "fun.h"

/*
** Name: get_info
** Function: get username and passwd from packet
** parameter:name, username
          passwd, password
	  data, the data get from socket.
** Return Value: 0, success. -1, failed.
*/
int get_info(char *name, char *passwd, char *data)
{
	int i=0;
	int n=0;

	while(data[n]!=',')
	{
		name[i]=data[n];
		i++;
		n++;
	}
	name[i]='\0';

	n+=1;
	i=0;
	while(data[n]!='\0')
	{
		passwd[i]=data[n];
		i++;
		n++;
	}
	passwd[i]='\0';
	return 0;
}

/*
** Name: load_info_lib
** Function: load username and passwd into server
** Parameter: none
** Return Value: 0, success; -1, failed
*/
INFO_NODE *load_info_lib()
{
	INFO_NODE *head, *pf, *pb;
	FILE *fp;
	int i=0;

	if((fp=fopen(PATH, "r"))==NULL)
	{
		perror("open");
		return NULL;
	}
	
	while(!feof(fp))
	{
		pb=(INFO_NODE *)malloc(LEN);
		fscanf(fp, "%s%s",pb->name, pb->passwd);
	//	printf("\nname: %s, passwd: %s\n",pb->name,pb->passwd);
		if(i==0)
		{
			pf=head=pb;
		}
		else
			pf->next=pb;
		pb->next=NULL;
		pf=pb;
		i++;
	}
	if(i==0)
		head=NULL;
	fclose(fp);
	return head;
}

/*
** Name: insert_node
** Function: insert a new user info to the chain table
** Parameter: head, the root pointer of chain table
	      name, user name
	      passwd, password of user
** Return Value: 0, success. -1, failed.
*/
int insert_node(INFO_NODE *head, char *name, char *passwd)
{
	INFO_NODE *new=(INFO_NODE *)malloc(LEN);	//creat a new node

	if(new==NULL)
	{
		return -1;
	}
	
	strcpy(new->name, name);	//put name and passwd into the node
	strcpy(new->passwd, passwd);
	
	if(head==NULL)
	{
		head=new;
		head->next=NULL;
	}
	else if(head->next==NULL)	//insert the new node into the chain table. 
	{
		head->next=new;
		new->next=NULL;
	}
	else
	{
		new->next=head->next;
		head->next=new;	
	}
	return 0;
}

/*
** Name: free_chain
** Function: free chain table when server quit
** Parameter: head, the root pointer of chain table
** Return Value: 0, success; -1 failed
*/
int free_chain(INFO_NODE *head)
{
	INFO_NODE *tmp1, *tmp2;

	tmp1=head;
	tmp2=NULL;

	if(head==NULL)
	{
		return -1;
	}

	while(tmp2!=NULL)
	{
		tmp2=tmp1->next;
		free(tmp1);
		tmp1=NULL;
		tmp1=tmp2;
	}
	printf("\nuser info_lib chain freed.\n");
	return 0;
}

/*
** Name: save_info
** Function: save client info to local file
** Parameter: information gona be saved
** Return Value: 0, success; -1, failed
*/
int save_info(char *name, char *passwd)
{
	FILE *fp;
	
	if((fp=fopen(PATH,"a+"))==NULL)		//open user info file
	{
		perror("open in save_info");
		return -1;
	}
	else
	{
		fprintf(fp, "\n");
		fprintf(fp,"%s %s", name, passwd);	//write info to file
	}

	fclose(fp);
	return 0;
}

/*
** Name: check_info
** Function: check whether the client's information match with the infomation library.
** Parameter: head, the root pointer to the chain table
**	      name, passwd, the information gonna to be compared
**	      option, whether check passwd, 0, no need to match; 1, match	
** Return Value: 0, username not fouund; 
**	         1, password didn't correct; 
**	         2, information mathed.
**		 -1, error.
*/
int check_info(INFO_NODE *inf_head, char *name, char *passwd, int option)
{
	int ret_n, ret_p,ret;
	INFO_NODE *tmp=malloc(sizeof(INFO_NODE));
	ret_n=ret_p=ret=0;
	if(inf_head==NULL)
	{
		return 0;
	}
	tmp=inf_head;
	
	while(tmp!=NULL)
	{
		if(!strcmp(tmp->name, name))
		{
			ret_n=1;
			if(option)	//whether check passwd
			{
				if(!strcmp(tmp->passwd, passwd))
				{
					ret_p=1;
					break;
				}
			}
			break;
		}
		tmp=tmp->next;
	}
	free(tmp);
	ret=ret_n+ret_p;
	return ret;
}

