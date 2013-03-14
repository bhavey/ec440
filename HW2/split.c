#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void split(char *input, char **argv) {
	int quote_flag=0;
	char *start = input;
	int index;
	int spc, l, r, amp, pipe, dl, dr;
	char *cur = input;
	int tot_len = strlen(input);
	int length=strlen(cur);
	char *prev=cur;
	while (1) { //First get rid of any leading spaces.
		spc = strcspn(cur, " ");
		if (spc == 0) {
			cur++;
		} else {
			break;
		}
	}
	char * pch;
	length=strlen(cur);
	int length1=length;
	while (1) { //Now get rid of ending spaces.
		pch=strrchr(cur, ' ');
		if ((pch-cur+1)==length1) { //ENDING SPACES
			cur[strlen(cur)-1]='\0'; //Add the null character to end cur.
			length1=strlen(cur);
		} else {
			break;
		}
	}
	prev=cur;
	length=strlen(cur);

	int i=0;
	int j=0;
	int span;
	char keys[]="<>|&";
	char temp[80];
	strcpy(temp,cur);
	int offset_val=0;
	int nobiggerthan=strlen(cur);
	while (1) { //Now simply pull out the desired values and throw them into the argv
		//Find the next instance of a special character.
		span = strcspn(cur+offset_val,keys);

		//Copy over the offseted string to a temp code.
		strcpy(temp,cur+offset_val);


		if (((temp[span]=='<')&&(temp[span+1]=='<'))
		    ||((temp[span]=='>')&&(temp[span+1]=='>')))
			span++; //increase span if it's a '<<' or a '>>'

		temp[span+2]='\0'; //cut off the end of temp. span+1 includes the special char,
		//so simply add +2 to terminate the string.
		for (j=0; j<span+1; j++) {
			argv[i][j]=temp[j]; //copy over temp for over the span into argv
		}
		argv[i][span+2]='\0';

		int k;

		offset_val=offset_val+span+1;
		i++;
		if ((offset_val)>=nobiggerthan) //done
			break;
	}
}


