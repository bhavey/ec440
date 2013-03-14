#include <unistd.h>
#include <stdio.h>

int syntax_check(char ***luvmakin) {
	int i,j,k;

	printf("Command: %s\n",luvmakin[0][0]);

	int topword,topindex;
	int topword2,topindex2;
	int splits[40];
	int flagvar;
	int redirect;
	flagvar=0;
	//flagvar=1 if there is a redirect from a file to the stdin of a command in the first cmd
	redirect=0;
	//redirect=1 if there was a redirect in the last run.

	//Make sure you reset these values to zero, otherwise luvmakin[topword] will get
	//called at an uninitialized index and seg fault the program.
	topword=0;
	topindex=0;
	k=0;
	i=0;

//	for (i=0; luvmakin[i][0][0]!='\0'; i++) {
//		for (k=0; luvmakin[i][k][0]!='\0';k++) {
//			printf("luvmakin[%d][%d]: %s\n",i,k,luvmakin[i][k]);
//		}
//	}
	//Checks for ambiguous redirects or other such nonesense.
	//Cuts power to execute if somethings wrong.
	for (i=0; luvmakin[i][0][0]!='\0'; i++) {
		topword=0;
		topindex=0;
		for (k=0; luvmakin[i][k][0]!='\0'; k++)
			topword++;
		for (k=0; luvmakin[i][topword-1][k]!='\0'; k++)
			topindex++;
		topword--;
		topindex--;

//		printf("i: %d, topword: %d, topindex: %d\n",i,topword,topindex);
//		printf("topwordluv: %s\n",luvmakin[i][topword]);
//		printf("topindexluv: %c\n",luvmakin[i][topword][topindex]);

		if (luvmakin[i][topword][topindex]=='>')
			redirect++;
		if ((luvmakin[i][topword][topindex]=='<')&&(i!=0)) {
			//If there is a file to stdin redirect it *must* happen in the first
			//command. If here it doesn't.
			printf("Syntax Error. Redirects from files into stdin must occur during");
			printf(" the first command.\n\n");
			return -1;
		}
		if (( (luvmakin[i][topword][topindex]=='<')||(luvmakin[i][topword][topindex]=='>'))
		    &&(luvmakin[i+1][0][0]=='\0')) {
			//Here if there is a redirect w.o. a file.
			printf("Syntax Error. You're building a buffer redirect to nowhere.");
			printf(" Almost like a bridge to Alaska.\n\n");
			return -1;
		} if (( (luvmakin[i][topword][topindex]=='<')||(luvmakin[i][topword][topindex]=='>'))
		    &&(luvmakin[i+1][0][0]!='\0')&&(luvmakin[i+2][0][0]!='\0')
			&&(redirect==1)) {
			//Here if there was a redirect followed by two splits,
			//as in there is either another redirect *or* a pipe following the file
			//Don't allow this to happen!
			printf("Syntax Error. Ben's bash requires file redirects to occur only");
			printf(" at the end of a command. Please change your syntax.\n\n");
			return -1;
		} if (( (luvmakin[i][topword][topindex]=='<')||(luvmakin[i][topword][topindex]=='>'))
		    &&(luvmakin[i+1][0][0]!='\0')&&(luvmakin[i+2][0][0]=='\0')) {
			//Check if we have redirects to redirects. Most of the syntax has been
			//checked by this point, so it only cares if the last character of the
			//next command is a special character. If so, things don't work.

			topword2=0;
			topindex2=0;
			for (k=0; luvmakin[i+1][k][0]!='\0'; k++)
				topword2++;
			for (k=0; luvmakin[i+1][topword2-1][k]!='\0'; k++)
				topindex2++;
			topword2--;
			topindex2--;

			char doop;
			doop=luvmakin[i+1][topword2][topindex2];
			if ((doop=='<')||(doop=='>')||(doop=='|')) {
				//Oh God.... what have I done!
				printf("Syntax Error! Redirect/pipe goes nowhere.\n\n");
				return -1;
			}
			//If there is no '<', everything is correct syntax
		}
		if (redirect>1) {
			printf("Contains more then one file input redirect.\n\n");
			return -1;
		}
//		printf("Correct Syntax!\n");
	}
	//Now we're done checking for proper syntax. If you made it to this point everything
	//is running smoothly. Now just run the program!!!
	execute(luvmakin);
}

