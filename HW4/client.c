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
    int sockfd, portno, n, i;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUF_SIZE];
    char tmpbuf[BUF_SIZE];
    char outbuf[BUF_SIZE]="0";

    if (argc < 3) {
        argv[1]="127.0.0.1"; //default!
        argv[2]="1234";
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
    printf("Please type in as many numbers as you want. They must be positive integers, and followed by a return. Any non-numeral characters will be ignored. Spaces will be considered new numbers. Press enter on a blank line to finish, type 'exit' to terminated the connection and do nothing.\n");
    int flipvar=1;
    int j=0;
    while (flipvar) {
        j=0;
        bzero(buffer,BUF_SIZE);
        bzero(tmpbuf,BUF_SIZE);
        fgets(buffer,BUF_SIZE-1,stdin);
        flipvar=strcmp(buffer,"\n"); //Break after newline is sent down the line!
        if (!strcmp(buffer,"exit\n")||!strcmp(buffer,"Exit\n")) {
            printf("Exit signal sent to server.\n");
            sprintf(outbuf,"ABORT"); //The server reads this as a "no add"
            break;
        }
        for (i=0; i<strlen(buffer); i++) {
            if (isdigit(buffer[i])) {
                tmpbuf[j]=buffer[i];
                if (tmpbuf[j]!='0') //remove leading zeros.
                    j++;
            }
            if (buffer[i]==' ') { //split up numbers
                tmpbuf[j]='\0';
                if (strlen(tmpbuf)>0)
                    sprintf(outbuf,"%s %s",outbuf,tmpbuf);
                j=0;
                tmpbuf[0]='\0';
            }
        }
        tmpbuf[j]='\0';
//        printf("Number: %s\n",tmpbuf);
        if ((strlen(outbuf)+strlen(tmpbuf)+3)>BUF_SIZE) {
            printf("The current selection exceeds the server transfer limit. Either press enter to send the current numbers, or type exit to leave without sending anything.\n");
            continue;
        }
        sprintf(outbuf,"%s %s",outbuf,tmpbuf);
    }

    printf("Final string: %s.\n",outbuf);

    n = write(sockfd,outbuf,strlen(outbuf));
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
