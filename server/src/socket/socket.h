#ifndef SOCKET_H
#define SOCKET_H

#include <pthread.h>

#define MAX_CLIENTS 4
#define SERVER_PORT 3000

// Data struct for the socket thread
typedef struct {
    // Socket variables
    int sock;

    // Thread events
    char initialized;
    char threadExit;

    // Mutex for thread safety
    pthread_mutex_t mutex;
} SocketListenInfo;

void *socketListenThread(void *arg);

// Function to set thread exit flag
static void *setThreadExit();

#endif