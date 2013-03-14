#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int check_type(char *inp);

int execute(char ***luvmakin) {
	int i,j,k;
	int topword,topindex;
	int types[80];
	int initial_size=0;
	int stdinv=0, stdoutv=0;
	char ***copy = malloc(sizeof(char**) * 80);
	for (i=0; i<80; i++)
		copy[i] = malloc(sizeof(char**) * 80);

	for (i=0; i<80; i++)
		for (j=0; j<80; j++)
			copy[i][j] = malloc(sizeof(char*) * 80);

	for (i=0; luvmakin[i][0][0]!='\0'; i++)
		initial_size++;
//	initial_size--;

	int amper;
	for (i=0; luvmakin[i][0][0]!='\0'; i++) {
		amper=0;
		for (j=0; luvmakin[i][j][0]!='\0'; j++) {
			for (k=0; luvmakin[i][j][k]!='\0'; k++)
				copy[i][j][k]=luvmakin[i][j][k];
			printf("copy[%d][%d]: %s\n",i,j,copy[i][j]);
			//Checks the type of the last value, returns one of the following:
			//0: none, 1: |, 2: > or 1>, 3: 2>, 4: >> or 1>>, 5: 2>>, 6: &>, 7: <, 8: &
		}
		types[i]=check_type(copy[i][j-1]);
		printf("types[%d]: %d\n\n",i,types[i]);
		if ((types[i]!=0)&&(types[i]!=8))
			copy[i][j-1][0]='\0'; //Clear out any added deals.
		copy[i][j][0]='\0';
	}

		copy[i][j][0]='\0';
	//See if we're to wait or not.
	if (types[i-1]==8) {
		amper=1;
		copy[i-1][j-1][k-1]='\0';
	}

	if (types[0]==7) //Sees if we start with a input redirect
		stdinv=1;

	//sees if we end w. an output redirect
	printf("initial_size: %d\n",initial_size);
	printf("types[0]: %d\n",types[initial_size-1]);
	if ((types[initial_size-1]!=7)&&(types[initial_size-1]!=8)&&(types[initial_size-1]!=1)
	    &&(types[initial_size-1]!=0))
		stdoutv=1;
	int pipesize=initial_size-stdinv-stdoutv-1;

	printf("in: %d, out: %d, pipesize: %d\n",stdinv,stdoutv,pipesize);
	printf("amper: %d\n",amper);


	int curtop;
	//For whatever reason the NULL values in the above loop aren't taking...
	//This will insure they go at the end of every split.
	for (i=0; copy[i][0][0]!='\0'; i++) {
		curtop=0;
		for (k=0; copy[i][k][0]!='\0';k++) {
			printf("copy[%d][%d]: %s\n",i,k,copy[i][k]);
			curtop++;
		}
		copy[i][curtop]=NULL;
	}

	printf("\nStarting Execution...\n");
	int status;
	int *pipes = malloc(sizeof(int) * pipesize * 2);
	//Create the necessary pipes!
	for (i=0; i<pipesize; i++)
		pipe(pipes + 2*i);

	i=0;
	if (pipesize==0)
		goto BOTTOM;
	TOP:
	if (fork() == 0) { //child process
		if (i!=0)
			dup2(pipes[i*2-2],0);
		if (i!=pipesize)
			dup2(pipes[i*2+1],1);
		for (j=0; j<(2*pipesize); j++)
			close(pipes[j]);

		execvp(copy[i][0],copy[i]);
	} else {
		i++;
		if (i!=(pipesize+1))
			goto TOP;
	}
	for (j=0; j<(2*pipesize); j++)
		close(pipes[j]);

	for (j=0; j<(2*pipesize); j++)
		wait(&status);

BOTTOM:
	if (pipesize==0) {
		int pipes[2];
		pipe(pipes);
		if (fork() == 0) {
			dup2(pipes[0],0);
			close(pipes[1]);
			execvp(copy[0][0],copy[0]);
		} else {
			int status;
			wait(&status);
		}
	}


	return 0;
}

//Checks the type of the last value, returns one of the following:
//0: none, 1: |, 2: > or 1>, 3: 2>, 4: >> or 1>>, 5: 2>>, 6: &>, 7: <, 8: &
int check_type(char *inp) {
	int length=strlen(inp);
	length--;
	if (length==0) {
		if(inp[0]=='|') {
			return 1;
		} else if(inp[0]=='>')
			return 2;
		else if(inp[0]=='<')
			return 7;
		else if(inp[0]=='&')
			return 8;
		else
			return 0;

	} else if (length==1) {
		if((inp[0]=='2')&&(inp[1]=='>'))
			return 3;
		else if((inp[0]=='>')&&(inp[1]=='>'))
			return 4;
		else if((inp[0]=='&')&&(inp[1]=='>'))
			return 6;
		else if(inp[1]=='&')
			return 8;
		else
			return 0;

	} else if (length==2) {
		if((inp[0]=='2')&&(inp[1]=='>')&&(inp[2]=='>'))
			return 5;
		else if(inp[2]=='&')
			return 8;
		else
			return 0;

	} else {
		if (inp[length]=='&')
			return 8;
		else
			return 0;
	}
}
