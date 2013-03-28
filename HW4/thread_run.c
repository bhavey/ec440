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

//This runs the thread and sends the sum back.
void *thread_run(void *arg) {
    char buffer[BUF_SIZE];
    char bigbuf[BUF_SIZE];
    char *pch;
    int i, j, n, newsockfd;
    int abort;
    int small_sum;
    int addvals[10000];
    int tot_vals;
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
    //Lock the thread so variables can't be messed around with.
    pthread_mutex_lock (&runsum);
    //This checks if a call is being made for ABORT or for sum.
    if ((strcmp(buffer,"ABORT")==0)) {
        abort=1; //called for abort
        printf("Got a request for Abort, add nothing, print abort message...\n");
    } else { //Call for a sum.
        sum_struct.client_calls++;
        abort=0;
        printf("Got a request for a sum.\n");
    }
    printf("String: %s\n",buffer);

    small_sum=0;
    if (abort) { //execute abort request, send nothing to client.
        sprintf(bigbuf,"Recieved your request for abort. Nothing done.\n");
        n = write(newsockfd,bigbuf,sizeof(bigbuf));
    } else {
        i=0;
        pch = strtok(buffer, " ");
        while (pch != NULL) {
            addvals[i]=atoi(pch);
            i++;
            pch = strtok (NULL, " ");
        }
        tot_vals=i;
        i=0;
        for (i=0; i<tot_vals; i++)
            small_sum+=addvals[i];
        sum_struct.sum+=small_sum;
        sprintf(bigbuf,"Your sum: %d\nThe current Grand Total is %d and I have served %d clients so far.",small_sum,sum_struct.sum,sum_struct.client_calls);
        n = write(newsockfd,bigbuf,sizeof(bigbuf));
    }
    printf("Sum: %d\n",sum_struct.sum);
    printf("Client calls: %d\n",sum_struct.client_calls);
    //Make sure we were able to write to the socket.
    if (n < 0) {
        error("ERROR writing to socket");
        close(newsockfd);
    }
    //We're done here...
    close(newsockfd);
    pthread_mutex_unlock (&runsum);
    pthread_exit((void*) 0);
    return 0;
}
#endif
