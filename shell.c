#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <sys/fcntl.h>
void execute(char **,int, char **,int);
void handle_signal(int);
int parse(char *, char **, char **, int *);
void chop(char *);
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
int cd(char **args)//cd command
{

	if(!args[1])//check for no argument in cd
	{


		chdir(home);
		return 1;

	}
	int i=0;
	char dir[1024];
	strcpy(dir,args[1]);
	for(i=0;dir[i]!='\0';i++) //check for ~ in cd command
		if(dir[i]=='~' && i>0)
			perror("error");
		if(dir[0]=='~')
		{
			char temp1[1024];
			char temp2[1024];
			int j=1;
			int m=0;
			for(;dir[j]!='\0';j++,m++)
				temp1[m]=dir[j];
			int k=m;
			j=0;
			for(;home[j]!='\0';j++)
			{
				temp2[j]=home[j];
			}
			int l=0;
			for(l=0;l<k;l++)
			{
				temp2[j]=temp1[l];
				j++;
			}
			temp2[j]='\0';
			strcpy(dir,temp2);
		}
		strcpy(args[1],dir);
		if(args[1][0]!='\0')
		{
			if(chdir(args[1])!=0)
			{
				perror("error");
			}
		}
		return 1;
	}
int echo(char **args,int argc) //echo command
{


	if (argc > 1)
	{

		int j;
		for(j=0;args[1][j]!='\0';j++)
		{
			if(args[1][j]!='"')
				printf("%c",args[1][j]);

		}
		printf(" ");
	}
	int i;
	for (i = 2; i < argc; i++)
	{
		int j;
		for(j=0;args[i][j]!='\0';j++)
		{
			if(args[i][j]!='"')
				printf("%c",args[i][j]);

		}
		printf(" ");
	}

	printf("\n");
	return 1;
}
int  pwd()//pwd command
{	char cwd[1024];
	getcwd(cwd,sizeof(cwd));
	printf("%s\n",cwd);
	return 1;
}
int fg_to_bg(proc t,proc u,int val)
{

	tcsetpgrp(shell_terminal,shell_pgid);

	tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);	
	int i;
	for(i=val-1;i<no-1;i++)
	{
		pro[i]=pro[i+1];
		jobs[i]=jobs[i+1];
	}
	pro[no-1]=t;
	jobs[no-1]=u;
}
int killall()
{	printf("came here killing you haha\n");
int i=0;
printf("value of no is %d\n",no );
for(i=0;i<no;i++)
	{	printf("I came here haha\n");
printf("killing process %d\n",pro[i].id );
kill(pro[i].id,9);
}
no=1;
return 1;
}
int bg_to_fg(char **args)
{
	int i=0;
	int val=0;
	while(args[1][i]!='\0')
	{
		val=10*val+args[1][i]-'0';
		i++;
	}
	int pgid=getpgid(pro[val-1].id);
	tcsetpgrp(STDIN_FILENO,pgid);
	kill(pro[val-1].id,SIGCONT);
	int status,wpid;
	do 
	{
		wpid = waitpid(pro[val-1].id, &status, WUNTRACED);
		if(WIFSTOPPED(status))
		{
			fg_to_bg(pro[val-1],jobs[val-1],val);
			return 1;

		}

	}while (!WIFEXITED(status) && !WIFSIGNALED(status));//wait for foreground process*/
		tcsetpgrp(STDIN_FILENO,getpgid(getpid()));
		proc t;
		int c=0;
		int j=0;
		proc temp[1024];
		proc temp2[1024];
		for(c=0;c<no;c++)
		{
			if(c!=val-1)
			{	
				temp[j]=pro[c];
				temp2[j]=jobs[c];
				j++;
			}
		}
		no=j;
		for(c=0;c<no;c++)
		{
			pro[c]=temp[c];
			jobs[c]=temp2[c];
		}
		return 1;
	}
	int printbgjobs()
	{
		int i=0;
		for(;i<no;i++)
		{
			printf("[%d] %s[%d]",i+1,jobs[i].process,jobs[i].id);
			printf("\n");

		}
		return 1;
	}
	int kjobs(char **args)
	{
		printf("%s\n","came here" );
		int i=0;
		int val=0;;
		while(args[1][i]!='\0')
		{

			val=10*val+args[1][i]-'0';
			i++;
		}
		printf("%d\n",val);
		printf("%d" ,pro[val-1].id);

		int sig=0;
		i=0;
		while(args[2][i]!='\0')
		{
			sig=10*sig+args[2][i]-'0';
			i++;
		}
		printf("%d \n",sig);
		if(kill(pro[val-1].id,sig)<0)
			perror("failed");
		else
			fprintf(stderr, "successfully killed %d\n",pro[val-1].id );
		int c=0;
		int m=0;
		//		printf("arbads");
		proc temp3[1024];
		proc temp4[1024];
		for(c=0;c<val-1;c++)
		{
				//	pid_t pid_result = waitpid(pro[c].id, &status, WNOHANG);

			if(kill(pro[c].id,0)==-1 && errno==ESRCH)
				printf("Process %s  exited whose id is %d\n",pro[c].process,pro[c].id);
			else
			{	
				temp3[m]=pro[c];
				temp4[m]=jobs[c];
				m++;
			}
		}
		for(c=val;c<no;c++){
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

		return 1;
	}
	/*prints the prompt onto the stderr*/
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

/*handle the signals*/
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
				int x;
				printf("%s@%s:%s$ "," ",username,hostname,path);
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
char **split(char *line,char *delim)//split using delimiters
{
	char * newline=(char*)malloc((1024)*sizeof(char));
	strcpy(newline,line);
	int size = 20,i = 0;
	char **tokens = malloc(size * sizeof(char*));
	char *token;
	if (!tokens) 
	{
		fprintf(stderr, "allocation error\n");
		exit(EXIT_FAILURE);
	}
	token = strtok(newline, delim);
	while (token != NULL) 
	{
		tokens[i] = token;
		i++;
		if (i >= size) 
		{
			size+=20;
			tokens = realloc(tokens,size * sizeof(char*));
			if (!tokens) 
			{
				fprintf(stderr, "allocation error\n");
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, delim);
	}
	tokens[i] = NULL;
	return tokens;
}

int parse(char *inputString, char *cmdArgv[], char **supplementPtr, int *modePtr)
{
	int cmdArgc = 0, terminate = 0;
	char *srcPtr = inputString;
	//printf("%s\n",srcPtr );
	//printf("parse fun%sends", inputString);
	while(*srcPtr != '\0' && terminate == 0)
	{
		if(*srcPtr!=' ' && *srcPtr!='&'){ 

			*cmdArgv = srcPtr;


			
			cmdArgc++;
			cmdArgv++;
	//		printf("%sfirst one\n",cmdArgv[0] );

		}
		while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\0' && *srcPtr != '\n' && terminate == 0)
		{

			switch(*srcPtr)
			{
				case '&':
				*modePtr = BACKGROUND;
				break;



			}
			srcPtr++;
		}
		while((*srcPtr == ' ' || *srcPtr == '\t' || *srcPtr == '\n') && terminate == 0)
		{
			*srcPtr = '\0';
			srcPtr++;
			
			
		}
		
		
	}
	//printf("%sfirst one\n",cmdArgv[0] );
	/*srcPtr++;
	 *srcPtr = '\0';
	 destPtr--;*/
	 *cmdArgv = '\0';
	 //fprintf(stderr, "checking%s %s\n",cmdArgv[0-cmdArgc],cmdArgv[1-cmdArgc]);
	 return cmdArgc;
	}

	void chop(char *srcPtr)
	{
		while(*srcPtr != ' ' && *srcPtr != '\t' && *srcPtr != '\n')
		{
			srcPtr++;
		}
		*srcPtr = '\0';
	}
	void execute(char **cmdArgv, int mode, char **supplementPtr,int cmdArgc)
	{

		if(strcmp(*cmdArgv, "cd") == 0)
		{
			cd(cmdArgv);
		}

		else if(strcmp(*cmdArgv, "pwd") == 0)
		{
			pwd();
		}

		else if(strcmp(*cmdArgv, "echo") == 0)
		{
			echo(cmdArgv,cmdArgc);
		}
		else if(strcmp(*cmdArgv,"overkill")==0)
			killall();
		// else if(strcmp(*cmdArgv,"fg")==0)
		// 	bg_to_fg(cmdArgv);
		
		else if(strcmp(*cmdArgv,"jobs")==0)
		{	
			int val=printbgjobs();
			val;
		}
		else if(strcmp(*cmdArgv,"kjob")==0)
			kjobs(cmdArgv);
		else if(strcmp(*cmdArgv,"fg")==0){
										//make the job args[1] the foreground process

			if(cmdArgc==2)
			{
				int status;
				int pgid5,pid5;
				int child_pid;
				pid5=pro[atoi(cmdArgv[1])-1].id;
			if(pid5>=0)								//get the pgid of the job
			{
				//fprintf(stderr,"%s\n",getname(pid)->pname);                                        
				pgid5=getpgid(pid5);
				tcsetpgrp(shell_terminal,pgid5);						//give control of the terminal to the process
				child_pid=pgid5;
				if(killpg(pgid5,SIGCONT)<0)								//send a SIGCONT signal
					perror("can't continue!");
				waitpid(pid5,&status,WUNTRACED);						//wait for it to stop or terminate
				if(!WIFSTOPPED(status))
				{
					//remove					//if terminated remove from jobs table
					child_pid=0;
				}
				else
					fprintf(stderr,"\n[%d]+ stopped ",child_pid);		//if stopped print message
				tcsetpgrp(shell_terminal,shell_pgid);							//return control of terminal to the shell
			}
			else
				fprintf(stderr,"no such job!!\n");
		}
		else
			fprintf(stderr,"Takes Exactly one argument!!\n");

	}

	else if(strcmp("pinfo",*cmdArgv)==0)
	{
		FILE *fp;
		char buf1[256],buf2[256];
		int pid;
		char name[256],state;
		long unsigned int size;
		if(cmdArgc==1)
		{
			sprintf(buf1,"/proc/%d/stat",getpid());					//open the stat and exe files
			sprintf(buf2,"/proc/%d/exe",getpid());
		}
		else
		{
			sprintf(buf1,"/proc/%s/stat",(cmdArgv[1]));
			sprintf(buf2,"/proc/%s/exe",(cmdArgv[1]));
		}
		if((fp=fopen(buf1,"r"))!=NULL)
		{
			/*scan the stat file*/
			fscanf(fp,"%d %*s %c %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d",&pid,&state,&size);												
			fclose(fp);									
			printf("pid -- %d\nProcess Status -- %c\nmemory -- %lu\n",pid,state,size);
			readlink(buf2,name,256);
			printf("Executable Path -- %s\n",name);
		}
		else
			perror("No such process");

	}


	else{ 

		pid_t pid, pid2;
		FILE *fp;
		int mode2 = NORMAL, cmdArgc, status1, status2;
		char *cmdArgv2[INPUT_STRING_SIZE], *supplement2 = NULL;
		char *argv[64];
		int i;
		
		pid = fork();

		if( pid < 0)
		{

			exit(-1);
		}
		else if(pid == 0)
		{




			if(mode == BACKGROUND)
			{

				setpgid(getpid(),getpid()); 
				sigset_t emptyset;
				sigemptyset(&emptyset);
				signal (SIGINT, SIG_DFL);
				signal (SIGQUIT, SIG_DFL);
				signal (SIGTSTP, SIG_DFL);
				signal (SIGTTIN, SIG_DFL);
				signal (SIGTTOU, SIG_DFL);
				signal (SIGCHLD, SIG_DFL);
				
			}
			int fd0,fd1,fd2,in=0,out=0,append=0;
			char input[64],output[64],output2[64];

			

			for(i=0;cmdArgv[i]!='\0';i++)
			{
				if(strcmp(cmdArgv[i],"<")==0)
				{        
					cmdArgv[i]=NULL;
					strcpy(input,cmdArgv[i+1]);
					in=2;           
				}               

				else if(strcmp(cmdArgv[i],">>")==0)
				{      
					cmdArgv[i]=NULL;
					strcpy(output2,cmdArgv[i+1]);
					append=2;
				}         

				else if(strcmp(cmdArgv[i],">")==0)
				{      
					cmdArgv[i]=NULL;
					strcpy(output,cmdArgv[i+1]);
					out=2;
				}         
			}

			
			if(in)
			{ 	
			

				if ((fd0 = open(input, O_RDONLY, 0)) < 0) 
					perror("Sorry ,Couldn't open input file , check the permissions");

			
				if(dup2(fd0, 0)<0) 
					printf("duping failed , cannot redirect the file as input\n");
				close(fd0); 
			}

		
			if (out)
			{
				if ((fd1 = creat(output , 0644)) < 0) 
					perror("Sorry , Couldn't open the output file , check the permissions");

				if(dup2(fd1, STDOUT_FILENO)<0)
					printf("duping failed , cannot redirect the output to file\n");
				close(fd1);
			}

			
			if (append)
			{
				if ((fd2 = open(output2 ,O_WRONLY|   O_APPEND)) < 0) 
					perror("Sorry , Couldn't open the output file , check the permissions");

				if(dup2(fd2, STDOUT_FILENO)<0) 
					printf("duping failed , cannot redirect the output to file\n");
				close(fd2);
			}




			if ((execvp(*cmdArgv, cmdArgv) < 0)) { 
				printf("*** ERROR: exec failed\n");
				exit(1);
			}


		}
		else
		{
			if(mode == BACKGROUND)
				{ 
			setpgid(pid,pid);

			pro[no].id=(int)pid;
			jobs[no].id=(int)pid;
							printf("[%d]\n",pid);
			strcpy(jobs[no].process,"\0");
			strcpy(pro[no].process,cmdArgv[0]);
			int j=0;
			for(;cmdArgv[j]!=NULL;j++)
			{	
				strcat(jobs[no].process,cmdArgv[j]);
				strcat(jobs[no].process," ");

			}
			
			no++;
			
		}

		else{
			
			waitpid(pid, &status1, 0);
		}
			
	}

}

}
