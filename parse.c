#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include<string.h>
#include<sys/wait.h>
#define INPUT_STRING_SIZE 80

#define NORMAL 				00
#define OUTPUT_REDIRECTION 	11
#define INPUT_REDIRECTION 	22
#define BACKGROUND			44
#define OUTPUT_APP	55

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

