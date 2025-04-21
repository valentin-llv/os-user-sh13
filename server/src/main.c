#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// For sockets
#include <unistd.h>
#include <arpa/inet.h>

// For threading
#include <pthread.h>

#include "main.h"

int main() {
    // Initialize mutexes
    if(pthread_mutex_init(&socketInfo.mutex, NULL)) {
        perror("Mutex initialization failed");
        return EXIT_FAILURE;
    }

    // Create socket communication thread
    pthread_t socketThread;
    if (pthread_create(&socketThread, NULL, socketListenThread, NULL) != 0) {
        perror("Failed to create socket thread");
        return EXIT_FAILURE;
    }

    // Wait for the socket thread to finish
    pthread_join(socketThread, NULL);

    // Destroy mutexes
    pthread_mutex_destroy(&socketInfo.mutex);
    
    return 0;
}
