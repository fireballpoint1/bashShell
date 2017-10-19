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
#define INPUT_STRING_SIZE 80

#define NORMAL 				00
#define OUTPUT_REDIRECTION 	11
#define INPUT_REDIRECTION 	22
#define BACKGROUND			44
#define OUTPUT_APP	55
extern int top;
typedef struct proc
{
	int id;
	char process[1024];//stores mapping of process and process id
}proc;
extern struct termios shell_tmodes;

extern int shell_terminal;
extern pid_t shell_pgid;

extern int no;
extern proc pro[1024];
extern proc jobs[1024];
typedef void (*sighandler_t)(int);
extern char * home;
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
{	printf("killing you\n");
int i=0;
printf("value of job is %d\n",no );
for(i=0;i<no;i++)
	{	printf("I came here\n");
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
		printf("%s\n","came to this place" );
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
		proc temp3[1024];
		proc temp4[1024];
		int c=0;
		int m=0;
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
				//the process is not brought to the foreground s it is a background process
			}
			int fd0,fd1,fd2,in=0,out=0,append=0;
			char input[64],output[64],output2[64];

			// finds where '<' or '>' or '>>' occurs and make that argv[i] = NULL , to ensure that command wont't read that

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

			//if '<' char was found in string inputted by user
			if(in)
			{ 	
				// fdo is file-descriptor

				if ((fd0 = open(input, O_RDONLY, 0)) < 0) 
					perror("Sorry ,Couldn't open input file , check the permissions");

				// dup2() copies content of fdo in input of preceeding file
				if(dup2(fd0, 0)<0) // STDIN_FILENO here can be replaced by 0 
					printf("duping failed , cannot redirect the file as input\n");
				close(fd0); // necessary
			}

			//if '>' char was found in string inputted by user 
			if (out)
			{
				if ((fd1 = creat(output , 0644)) < 0) 
					perror("Sorry , Couldn't open the output file , check the permissions");

				if(dup2(fd1, STDOUT_FILENO)<0) // 1 here can be replaced by STDOUT_FILENO
					printf("duping failed , cannot redirect the output to file\n");
				close(fd1);
			}

			//if '>>' char was found in string inputted by user 
			if (append)
			{
				if ((fd2 = open(output2 ,O_WRONLY|   O_APPEND)) < 0) 
					perror("Sorry , Couldn't open the output file , check the permissions");

				if(dup2(fd2, STDOUT_FILENO)<0) // 1 here can be replaced by STDOUT_FILENO
					printf("duping failed , cannot redirect the output to file\n");
				close(fd2);
			}


			/* another syntax
			   execvp(*argv, argv);
			   perror("execvp");
			   _exit(1);
			 */
			 

			if ((execvp(*cmdArgv, cmdArgv) < 0)) {     // execute the command  
				printf("*** ERROR: exec failed\n");
				exit(1);
			}


		}
		else
		{
			if(mode == BACKGROUND)
				{ printf("%s\n","background-ed" );
			setpgid(pid,pid);

			pro[no].id=(int)pid;
			jobs[no].id=(int)pid;
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
			//wait(NULL);
	}

}

}