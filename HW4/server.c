#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#include "thread_run.c"
#include "error.h"
#include "server.h"

#define BUF_SIZE 65536

struct ext_struct sum_struct;

void error(const char*);
void *thread_run(void * arg);
pthread_mutex_t runsum;

int main(int argc, char *argv[]) {
    pthread_attr_t attr;
    pthread_t threads;
//    pthread_mutex_t totcli;
    unsigned int ids;

    sum_struct.sum=0;
    sum_struct.client_calls=0;
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[BUF_SIZE];
    struct sockaddr_in serv_addr, cli_addr;

    //Checks to see if there was a provided port.
    if (argc < 2) {
        argv[1]="1234"; //Default port!
    }

    //Create a socket descriptor, confirm it's created.
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("ERROR opening socket");
    //Zeros out the serv_addr buffer.
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //Set the port to the acquired argument.
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //Set the server address' port to portno
    serv_addr.sin_port = htons(portno);

    //Bind the server address to the socket descriptor. Check if it was acceptable.
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    //Start listening for requests to the serer through the socket descriptor.
    listen(sockfd,5);

    clilen = sizeof(cli_addr);
    //Multithread the code at this part!

    pthread_mutex_init(&runsum, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    printf("Initiated server.\n");
    while (1) {
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
        if (newsockfd < 0) {
            error("ERROR on accept");
            close(newsockfd);
        } else {
            ids = newsockfd;
            pthread_create(&threads, &attr, thread_run, &ids);
        }
    }
    close(newsockfd);
    close(sockfd);
    pthread_mutex_destroy(&runsum);
    pthread_exit(NULL);
    return 0;
}
