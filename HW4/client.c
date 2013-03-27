//Stolen from www.linuxhowtos.org/C_C++/socket.html
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include "error.h"

#ifndef BUF_SIZE
#define BUF_SIZE 65536

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUF_SIZE];
    char outbuf[BUF_SIZE]="0";

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
      server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Please type in as many numbers as you want. They must be positive integers, and followed by an enter. Any non-numeral characters will be ignored. Press enter to finish.\n");
    int flipvar=1;
    while (flipvar) {
        bzero(buffer,BUF_SIZE);
        fgets(buffer,BUF_SIZE-1,stdin);
        printf("Number: %s",buffer);
        flipvar=strcmp(buffer,"\n");
        if (!strcmp(buffer,"exit\n")||!strcmp(buffer,"Exit\n")) {
            printf("Exit signal sent to server.\n");
            sprintf(outbuf,"ABORT"); //The server reads this as a "no add"
            break;
        }
        buffer[strlen(buffer)-1]='\0';
        if ((strlen(outbuf)+strlen(buffer)+3)>BUF_SIZE) {
            printf("The current selection exceeds the server transfer limit. Either press enter to send the current numbers, or type exit to leave without sending anything.\n");
            continue;
        }
        sprintf(outbuf,"%s %s",outbuf,buffer);
    }
    printf("Final string: %s.\n",outbuf);
    return 0;
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    bzero(buffer,BUF_SIZE);
    n = read(sockfd,buffer,BUF_SIZE-1);
    if (n < 0)
        error("ERROR reading from socket");
    printf("%s\n",buffer);
    close(sockfd);
    return 0;
}
#endif
