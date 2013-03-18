
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
    int i, n, newsockfd;
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
        int log_size=0;
        FILE *fp;
        fp = fopen("/var/log/kernel.log","r");
        if (fp == NULL) {
            printf("Could not find system log files.\n");
            n = write(newsockfd,"Could not find system log files.\n",sizeof("Could not find system log files.\n"));
            close(newsockfd);
            return 0;
        } else {
            while (1) { //get the file size!
                fgets (bigbuf,2048,fp);
                if (!feof(fp))
                    log_size++;
                else
                    break;
            }
            fclose(fp);
            fp = fopen("/var/log/kernel.log","r");
            while (1) {
                fgets (buffer,2048,fp);
                if (!feof(fp)) {
                    if (i > log_size-30)
                        n = write(newsockfd, buffer, sizeof(buffer));
                    i++;
                } else
                    break;
            }
        }
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
