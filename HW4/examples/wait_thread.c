#include <pthread.h>

volatile int running_threads = 0;
pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER;

void * threadStart() { //The thread job
    pthread_mutex_lock(&running_mutex);
    running_threads--;
    pthread_mutex_unlock(&running_mutex);
}

int main() {
    int i;
    int num_threads=2;
    for (i = 0; i < num_threads; i++) {
        pthread_mutex_lock(&running_mutex);
        running_threads++;
        pthread_mutex_unlock(&running_mutex); //launch thread
    }

    while (running_threads > 0) {
        sleep(1);
    }
}
