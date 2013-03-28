#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "server.h"

#ifndef BUF_SIZE
#define BUF_SIZE 65536

void error(const char*);

//This runs the server and send the correct data back.
void *thread_run(void *arg) {
    char buffer[BUF_SIZE];
    char bigbuf[BUF_SIZE];
    int i, n, newsockfd;
    int abort;
    int addvars[10000];
    //copy over the client handler
    newsockfd = *(unsigned int *)arg;

    //Initialize the buffer to 0.
    bzero(buffer,BUF_SIZE);

    //Read in the buffer.
    n = read(newsockfd,buffer,BUF_SIZE);
    //Error reading :/
    if (n<0) {
        error("ERROR reading from socket");
        close(newsockfd);
    }

    //This checks if a call is being made for ABORT or for sum.
    if ((strcmp(buffer,"ABORT")==0)) {
        abort=1; //called for abort
        printf("Got a request for Abort, add nothing, print abort message...\n");
    } else { //Call for a sum.
        abort=0;
        printf("Got a request for a sum.\n");
    }
    printf("String: %s\n",buffer);

    if (abort) { //execute abort request, send nothing to client.
        sprintf(bigbuf,"Recieved your request for abort. Nothing done.\n");
        n = write(newsockfd,bigbuf,sizeof(bigbuf));
    } else {
        sprintf(bigbuf,"Got request for sum!\n");
        n = write(newsockfd,bigbuf,sizeof(bigbuf));
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
#endif
