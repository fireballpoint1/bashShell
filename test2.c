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
	int status=1;
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
		int status1;
		pid_t t;
		mode = NORMAL;
		getcwd(curDir, 100);
		proc temp3[1024];
		proc temp4[1024];


		int c=0;
		int m=0;


		for(c=0;c<no;c++)
		{
			pid_t pid_result = waitpid(pro[c].id, &status, WNOHANG);

			if(kill(pro[c].id,0)==-1 && errno==ESRCH){
				printf("Process %s  exited whose id is %d\n",pro[c].process,pro[c].id);
				printf("YES");
			}
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
		printf("%s@%s:%s$",username,hostname,path);
		getline( &inputString, &len, stdin);
		if(strcmp(inputString, "exit\n") == 0)
			exit(0);
		char **args2;
		args2=split(inputString,";");
		for(i=0;args2[i]!=NULL;i++)
		{

			cmdArgc = parse(args2[i], cmdArgv, &supplement, &mode);

			if(strcmp(*cmdArgv, "cd") == 0)
			{
				cd(cmdArgv);
			}
			else if(strcmp(*cmdArgv, "") == 0)
			{
				continue;
			}

			else if(strcmp(*cmdArgv, "pwd") == 0)
			{
				pwd();
			}

			else if(strcmp(*cmdArgv, "echo") == 0)
			{
				echo(cmdArgv,cmdArgc);
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


			else 
				execute(cmdArgv, mode, &supplement);
		}
	}

	return 0;
}
