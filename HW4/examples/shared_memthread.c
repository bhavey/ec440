#include<pthread.h>
#include<stdio.h>
int sharedx=0;

void *threadFunc(void *arg) {
    printf(" %d %s", sharedx,(char*)arg);
    sharedx++;
}

int main(void) {
    pthread_t pth[10];  // this is our thread identifier
    int i = 0;
    for (i=0; i<10; i++) {
        pthread_create(&pth[i],NULL,threadFunc,"processing...");
    }
}
