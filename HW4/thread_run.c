
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>

#ifndef BUF_SIZE
#define BUF_SIZE 65536

void error(const char*);

//This runs the server and send the correct data back.
void *thread_run(void *arg) {
    char buffer[BUF_SIZE];
    char bigbuf[BUF_SIZE];
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
        pid_t pid;
        char *tailCmd[] = {"tail","-n","30","/var/log/kernel.log",0}; //Equiv to dmesg

        if ((pid = fork()) < 0) //Fork Error
            printf("Error Forking\n",sizeof("Error Forking\n"));
        else if (pid ==  0) { //child process! Run that shit.
            setbuf(stdout, bigbuf); //stdout goes to bigbuf now.
            execvp(tailCmd[0],tailCmd); //Now run dmesg clone command!
        } else { //Parent process.
            fclose(stdout);
            pid_t childPid;
            int status;
            childPid = wait(&status); //wait for tail to finish.
        }
      n = write(newsockfd,bigbuf,sizeof(bigbuf));
    } else {
        pid_t pid;
        char *writeCmd[] = {"/bin/bash","write_message.sh",buffer,0}; //Equiv to dmesg
        char *ftpCmd[] = {"/bin/bash","ftp.sh",0}; //Equiv to dmesg
        if ((pid = fork()) < 0) //Fork Error
            printf("Error Forking\n",sizeof("Error Forking\n"));
        else if (pid ==  0) { //child process! Run that shit.
            execvp(writeCmd[0],writeCmd); //Now run dmesg clone command!
        } else { //Parent process.
            pid_t childPid;
            int status;
            childPid = wait(&status); //wait for tail to finish.
        }
        if ((pid = fork()) < 0) //Fork Error
            printf("Error Forking\n",sizeof("Error Forking\n"));
        else if (pid ==  0) { //child process! Run that shit.
            execvp(ftpCmd[0],ftpCmd); //Now run dmesg clone command!
        } else { //Parent process.
            pid_t childPid;
            int status;
            childPid = wait(&status); //wait for tail to finish.
        }
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
