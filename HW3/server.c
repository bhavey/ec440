/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
//Stolen from www.linuxhowtos.org/C_C++/socket.html
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//Displays errors when a system call fails.
void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    //The sockaddr_in and serv_addr structures are defined in <sys/socket.h>
    struct sockaddr_in serv_addr, cli_addr;
    //sockaddr_in contains sin_family, it needs to be AF_INET
    //Clear sockaddr_in with memset
    //sockaddr_in has: sin_family, sin_port, in_addr, sin_addr, sin_zero
    //sin_family must be AF_INET, sin_port is the listening port, in_addr is IPv4
    //network address. Idk wtf sin_addr does, sin_zero must stay = 0.

   //serv_addr contains the address of the server.
   //cli_addr contains the address of the client.

    int n;
    //Checks to see if there was a provided port.
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0)
        error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0)
        error("ERROR writing to socket");
    close(newsockfd);
    close(sockfd);
    return 0;
}
