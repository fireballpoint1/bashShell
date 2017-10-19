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

		printf("%s@%s->", getlogin(),curDir);
		getline( &inputString, &len, stdin);
		if(strcmp(inputString, "quit\n") == 0)
			exit(0);
		char **args2;
		args2=split(inputString,";");
		char **afterpiping;
		int piping=1;
		char **afterredirect,**afterredirect2,**afterredirect3;
		for(i=0;args2[i]!=NULL;i++)
		{	int j;
			char **args3;
			int   p[2];
			pid_t pid4;
			int   fd_in = 0;
			int   i = 0;
			char** cmd=split(args2[i],"|");
			for(j=0;cmd[j]!=NULL;j++){
				printf("%d\n",j );
				if(j==1){

					piping=0;
					break;
				}

			}
			if(piping==0)
			{
				for(j=0;cmd[j]!=NULL;j++){


					char *cmdArgv[INPUT_STRING_SIZE];

					cmdArgc = parse(cmd[j], cmdArgv, &supplement, &mode);

					//fprintf(stderr, "%s %s %d %s\n",cmdArgv[0],cmdArgv[1],j,cmd[j] );
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
				printf("%s\n","pahaunch gye" );
				printf("%s\n",cmd[0] );
				execute(cmdArgv, mode, &supplement,cmdArgc);
			}


		}       

		//if(piping==1)  // case of no piping
		//execute(cmdArgv , 0 ,n,mode, &supplement,cmdArgc);           /* otherwise, execute the command */


	}
	return  0;
}
