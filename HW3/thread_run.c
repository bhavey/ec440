
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#define BUF_SIZE 256
void error(const char*);

//This runs the server and send the correct data back.
void *thread_run(void *arg) {
    char buffer[BUF_SIZE];
    char bigbuf[2048];
//    char bigbuf[65536];
    int n, newsockfd;
    int dmesg;
    //copy over the client handler
    newsockfd = *(unsigned int *)arg;

    //Initialize the buffer to 0.
    bzero(buffer,BUF_SIZE);

    //Read in the buffer.
    n = read(newsockfd,buffer,255);
    //Error reading :/
    if (n<0) {
        error("ERROR reading from socket");
        close(newsockfd);
    }

    //This checks if a call is being made for dmesg or for ftp print.
    if ((strncmp(buffer,"dmesg",5)==0) && ((int)strlen(buffer)==6)) {
        //Calling for dmesg.
        dmesg=1;
        printf("Got a request for dmesg.\n");
    } else { //Call for an FTP print.
        dmesg=0;
        printf("Got a request for an FTP print.\n");
    }

    if (dmesg) { //execute dmesg.
        pid_t pid;
        char *dmesgCmd[] = {"dmesg",0}; //Command corresponding to dmesg.

        if ((pid = fork()) < 0) //Fork Error
            printf("Error Forking\n",sizeof("Error Forking\n"));
        else if (pid==0) { //Child process. Execute dmesg!
            //freopen("/dev/null", "a", stdout);
            setbuf(stdout, bigbuf); //stdout now goes to bigbuf.
            execvp(dmesgCmd[0],dmesgCmd); //Now run dmesg!
        } else { //Parent process
            pid_t childPid;
            int status;
            childPid = wait(&status); //wait until dmesg is done.
            fclose (stdout);
        }
        strncpy (buffer,bigbuf,255);
        n = write(newsockfd,buffer,sizeof(buffer));
    } else {
        n = write(newsockfd,"You requested an FTP print",sizeof("You requested an FTP print"));
    }
    //Make sure we were able to write to the socket.
    if (n < 0) {
        error("ERROR writing to socket");
        close(newsockfd);
    }
    //We're done here...
    close(newsockfd);
    return 0;
}
