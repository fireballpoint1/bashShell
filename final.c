#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

#include <sys/stat.h>
#include <termios.h>
#include "parse.h"
#include "execute.h"
#include "split.h"
void execute(char **, int, char **,int);

int parse(char *, char **, char **, int *);

char **split(char *,char *);
#define INPUT_STRING_SIZE 80

#define NORMAL 				00
#define OUTPUT_REDIRECTION 	11
#define INPUT_REDIRECTION 	22
#define BACKGROUND			44
#define OUTPUT_APP	55
struct termios shell_tmodes;
int top=0;
int shell_terminal;
pid_t shell_pgid;
typedef struct proc
{
	int id;
	char process[1024];//stores mapping of process and process id
}proc;

int no=0;
proc pro[1024];
proc jobs[1024];
typedef void (*sighandler_t)(int);
char * home;
/*handle the signals*/
void getprompt()
	{
		char a[200],*user;
		user=getenv("LOGNAME");
		gethostname(a,200);
		char *c,fi[100];
		c=getcwd(NULL,0);

		fprintf(stderr,"<%s@%s:%s>",user,a,c);

		return;

	}
	void sig_handler(int signo)
	{
		if(signo==SIGINT)
		{
			fprintf(stderr,"\n");
			getprompt();
		}

	}


	void remove_np(char * a)
	{
		char ** nline = split(a," \t\n");
		int i;
		for(i=0;i<=strlen(nline[0]);i++)
			a[i]=nline[0][i];

	}

int main(int argc, char *argv[])
	{
		shell_terminal = STDIN_FILENO;
		while (tcgetpgrp (shell_terminal) != (shell_pgid = getpgrp ()))
			kill (- shell_pgid, SIGTTIN);

	/* Ignore interactive and job-control signals.  */
		signal (SIGINT, SIG_IGN);
		signal (SIGQUIT, SIG_IGN);
		signal (SIGTSTP, SIG_IGN);
		

	/* Put ourselves in our own process group.  */
		shell_pgid = getpid ();
		if (setpgid (shell_pgid, shell_pgid) < 0)
		{
			perror ("Couldn't put the shell in its own process group");
			exit (1);
		}

	/* Grab control of the terminal.  */
		tcsetpgrp (shell_terminal, shell_pgid);

	/* Save default terminal attributes for shell.  */
		tcgetattr (shell_terminal, &shell_tmodes);
		int i, mode = NORMAL, cmdArgc;
		size_t len = INPUT_STRING_SIZE;
		char *cpt, *inputString, *supplement = NULL;
		inputString = (char*)malloc(sizeof(char)*INPUT_STRING_SIZE);
		char curDir[100];
		home = (char *)malloc(100*sizeof(char));
		getcwd(home,100);
		char hostname[1024];
		gethostname(hostname,sizeof(hostname));
		char *username;
		username=getenv("USER");

		while(1)
		{	
			if(signal(SIGINT,sig_handler)==SIG_ERR)
				perror("Signal not caught!!");
			

			mode = NORMAL;
			getcwd(curDir, 100);
			proc temp3[1024];
			proc temp4[1024];
			int c=0;
			int m=0;

			for(c=0;c<no;c++)
			{

				if(kill(pro[c].id,0)==-1 && errno==ESRCH)
					printf("Process %s  exited whose id is %d\n",pro[c].process,pro[c].id);
				else
				{	
					temp3[m]=pro[c];
					temp4[m]=jobs[c];
					m++;
				}
			}
			no=m;
			for(c=0;c<no;c++)
			{
				pro[c]=temp3[c];
				jobs[c]=temp4[c];
			}
			int len1=strlen(home);
			int totallen=strlen(curDir);
			int ii;
			int flag=0;
			char path[100];
			for(ii=0;ii<len1;ii++)
				if(curDir[ii]!=home[ii])
					flag=1;
				if(flag==0)
				{

					for(ii=len1;ii<=totallen;ii++)
						path[ii-len1+1]=curDir[ii];
					path[0]='~';



				}
				else
				{
					for(ii=0;ii<totallen;ii++)
						path[ii]=curDir[ii];


				}	
			//	printf("%s@%s:%s$",username,hostname,path);
			
			printf("%s@%s:%s$", username,hostname,path);
			int x;
			x=getline( &inputString, &len, stdin);
				if(strcmp(inputString,"\n")==0)
					{	continue;
					//printf("%s\n","got it" );
					}
					//printf("%c\n", );
				if(x==-1)
					exit(1);	
				
			if(strcmp(inputString, "exit\n") == 0)
				exit(0);
			char **args2;
			args2=split(inputString,";");
			for ( i = 0; args2[i]!=NULL; ++i)
			{
				
				/* code */
			}
			char **afterpiping;
			int piping=1;
			char **afterredirect,**afterredirect2,**afterredirect3;
			for(i=0;args2[i]!=NULL;i++)
				{	int j;
					char **args3;
					int   p[2];
					pid_t pid4;
					int   fd_in = 0;
					
					char** cmd=split(args2[i],"|");
					for(j=0;cmd[j]!=NULL;j++){
				//		printf("%s\n",cmd[j] );
						if(j==1){

							piping=0;
							break;
						}

					}
					if(piping==0)
					{
						for(j=0;cmd[j]!=NULL;j++){


							char *cmdArgv[INPUT_STRING_SIZE];
							*cmdArgv=NULL;
							cmdArgc = parse(cmd[j], cmdArgv, &supplement, &mode);

						//fprintf(stderr, "%s %s %d %c\n",cmdArgv[0],cmdArgv[1],j,cmd[j][1] );
							pipe(p);
							if ((pid4 = fork()) == -1)
							{
								exit(1);
							}
							else if (pid4 == 0)
						{	//fprintf(stderr, "%s %s\n","came here",cmd[j] );
					dup2(fd_in, 0);
					if (cmd[j+1] != NULL)
						dup2(p[1], 1);
					close(p[0]);

					execute(cmdArgv, mode, &supplement,cmdArgc);
					exit(2);
				}
				else
				{
					wait(NULL);
					close(p[1]);
					fd_in = p[0];
					i++;
				}
			}
		}else{
			char *cmdArgv[INPUT_STRING_SIZE];

			cmdArgc = parse(cmd[0], cmdArgv, &supplement, &mode);
	
			execute(cmdArgv, mode, &supplement,cmdArgc);
		}


	}       

			//if(piping==1)  // case of no piping
				//execute(cmdArgv , 0 ,n,mode, &supplement,cmdArgc);           /* otherwise, execute the command */


}
return  0;
}

