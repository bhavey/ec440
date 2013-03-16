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

    //This is the communication buffer.
    char sendBuff[1025];
    time_t ticks;

    //Socket function declared as: socket(domain,type,protocol);
    //Domain: AF_INET represents IPv4 and AF_INET6 is IPv6
    //Type: SOCK_STREAM is TCP, others are *_DGRAM, *_SEQPACKET, *_RAW
    //Protocol: IPPROTO_TCP, *_SCTP, *_UDP, *_DCCP. 0 is the default for domain/type
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while(1) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        ticks = time(NULL);
        snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        write(connfd, sendBuff, strlen(sendBuff));

        close(connfd);
        sleep(1);
     }
}
