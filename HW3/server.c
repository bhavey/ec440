#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

void *thread_run(void * arg);
//Displays errors when a system call fails.
void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    //Creates a sleep time. Server will only accept new requests every 1/10 of a second
    struct timespec sleep_time;
    sleep_time.tv_sec=0;
    sleep_time.tv_nsec=100000000;

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

    //Checks to see if there was a provided port.
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    //Socket function declared as: socket(domain,type,protocol);
    //Domain: AF_INET represents IPv4 and AF_INET6 is IPv6
    //Type: SOCK_STREAM is TCP, others are *_DGRAM, *_SEQPACKET, *_RAW
    //Protocol: IPPROTO_TCP, *_SCTP, *_UDP, *_DCCP. 0 is the default for domain/type

    //Create a socket descriptor, confirm it's created.
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("ERROR opening socket");
    //Zeros out the serv_addr buffer.
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //Set the port to the acquired argument.
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    //By binding to INADDR_ANY, our server doesn't need to know the IP address of the
    //server before running. The bound socket will accept for the bound port that
    //arrive on interfaces 0, 1, or 2. This locks the ammount of available interfaces
    //for the port because you're taking up more interaces then necesary.

    //Set the server address' port to portno
    serv_addr.sin_port = htons(portno);

    //Bind the server address to the socket descriptor. Check if it was acceptable.
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    //Start listening for requests to the serer through the socket descriptor.
    listen(sockfd,5);

    clilen = sizeof(cli_addr);
    //Multithread the code at this part!
    while (1) {
        printf("Initiated server.\n");
        addr_len = sizeof(cli_addr);
        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr,&clilen);
        if (newsockfd < 0) {
            error("ERROR on accept");
            close(newsockfd);
            close(sockfd);
        }
//HERE IS WHERE WE GO.
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}

void *thread_run(void * arg) {
    char buff_in[512];
    char buff_out[512];
    client_s = *(unsigned int *)arg;

//The old code is here!
    int n;
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0) {
        error("ERROR reading from socket");
        close(newsockfd);
        close(sockfd);
    }
    printf("Here is the message from the client: %s",buffer);
    n = write(newsockfd,"I got your message",18);
    if (n < 0) {
        error("ERROR writing to socket");
        close(newsockfd);
        close(sockfd);
    }
}
