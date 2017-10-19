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

