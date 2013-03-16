//Stolen from:
//http://www.thegeekstuff.com/2011/12/c-socket-programming/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int listenfd = 0, connfd = 0;
    //The sockaddr_in and serv_addr structures are defined in <sys/socket.h>
    struct sockaddr_in serv_addr;
    //sockaddr_in contains sin_family, it needs to be AF_INET
    //Clear sockaddr_in with memset
    //sockaddr_in has: sin_family, sin_port, in_addr, sin_addr, sin_zero
    //sin_family must be AF_INET, sin_port is the listening port, in_addr is an IPv4
    //network address. Idk wtf sin_addr does, sin_zero must stay = 0.

    //serv_addr contains the address of the server.

    //you can convert short int to network bytes with htons function (host to network)

    //This is the communication buffer.
    char sendBuff[1025];
    time_t ticks;

    //Socket function declared as: socket(domain,type,protocol);
    //Domain: AF_INET represents IPv4 and AF_INET6 is IPv6
    //Type: SOCK_STREAM is TCP, others are *_DGRAM, *_SEQPACKET, *_RAW
    //Protocol: IPPROTO_TCP, *_SCTP, *_UDP, *_DCCP. 0 is the default for domain/type
    //set the socket descriptor with socket.
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    //Set the serv_addr structure. sin_family needs to be AF_INET.
    serv_addr.sin_family = AF_INET;
    //By binding to INADDR_ANY, our server doesn't need to know the IP address of the
    //server before running. The bound socket will accept for the bound port that
    //arive on interfaces 0, 1, or 2. This locks the ammount of available interfaces
    //for the port because you're taking up more interaces then necesary.
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //Set the server address to port 5000
    serv_addr.sin_port = htons(5000);

    //Binds the server to the socket descriptor.
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    //Listen for requests to the server through the socket descriptor.
    listen(listenfd, 10);

    //Now this is the code that runs for server requests.
    while(1) {
        //Calling accept causes the server to sleep until a server request is made,
        //at which point connfd is set to the descriptor of the client socket
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        printf("Server request from: %s\n","this");
        //Find the time.
        ticks = time(NULL);
        //print the time time to the sendbuffer.
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        //write the sendbuffer to the the client socket descriptor.
        write(connfd, sendBuff, strlen(sendBuff));

        //Close the client socket.
        close(connfd);
        sleep(1);
        //This is there in case there are tons of requests, in which case the server
        //would eat up the CPU time.
     }
}
