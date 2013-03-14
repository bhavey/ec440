#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include "syntax_check.h"
#include "execute.h"
#include "split.h"

#define MAX_LINE 80

int main(void) {
//	signal(SIGINT,SIG_IGN);
	char ***luvmakin = malloc(sizeof(char**) * 80);
        int i,j;
	for (i=0; i<80; i++) {
		luvmakin[i] = malloc(sizeof(char*) * 80);
	}
	for (i=0; i<80; i++) {
		for (j=0; j<80; j++) {
			luvmakin[i][j] = malloc(sizeof(char*) * 80);
		}
	}

        char * buffer = malloc(sizeof(char) * MAX_LINE);

        char * args; //Command line arguments
        char **argv = (char**) calloc(MAX_LINE, sizeof(char*));
        for (i=0; i<MAX_LINE; i++) {
                argv[i] = (char*) calloc(MAX_LINE, sizeof(char));
                argv[i][0] = '\0';
        }

        i=0;
        char * command;
        char * params[MAX_LINE];
	int tot_args;
        while (1) {
                printf("bs>");
                fflush(stdout); //Flush the stdout.
                fgets(buffer, MAX_LINE, stdin); //Writes stdin to buffer char array
                i=0; //Always set to 0!

		for (i=0; i<MAX_LINE; i++) //Get rid of the new line!
			if(buffer[i]=='\n')
				buffer[i]='\0';

                int position1=0;
                int pos[40];

                int j=0;

                i=0;
                j=1;
                int k=0;
                char *tempstr;

		//CLEAN OUT ARGV BEFORE SENDING IT TO BUFFER:
		for (i=0; i<MAX_LINE; i++) {
			for (j=0; j<MAX_LINE; j++) {
				argv[i][j]='\0';
			}
		}

                split(buffer,argv);

		int next_deal=0;
		int spandex;
		int differential=0;

		//Clean out luvmakin!
		for (i=0; i<80; i++) {
			for (j=0; j<79; j++) {
				for (k=79; k>=0; k--) {
					luvmakin[i][j][k]='\0';
				}
			}
		}

		//Split everything by words!!!
		for (k=0; argv[k][0]!='\0'; k++) {
			for (j=0; next_deal <= strlen(argv[k]); j++) {
				spandex=strcspn(argv[k]+next_deal," ");
				while (spandex==0) { //ignore double spaces!!! :)
					spandex=strcspn(argv[k]+next_deal," ");
					next_deal++;
				}
				strcpy(luvmakin[k][j-differential],argv[k]+next_deal);
				luvmakin[k][j-differential][spandex]='\0';
				next_deal=next_deal+spandex+1;
			}
			next_deal=1; //Doesn't account for double spaces for times *after* one
		}


                command=luvmakin[0][0];
                if (!strcmp(command,"exit")) { //Check it "exit" was called
                        printf("Thank you for using Ben's prompt!\n");
                        break;
                }

		syntax_check(luvmakin); //Start that shit going.

                i=0; //Gots to set i back to 0!

	}
	return 0;
}
