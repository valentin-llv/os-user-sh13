#ifndef CLIENT_HANDLER_H
#define CLIENT_HANDLER_H

#include <pthread.h>
#include <netinet/in.h>

// Structure to pass client info to thread
typedef struct {
    int client_sock;
    struct sockaddr_in client_addr;

    // Thread events
    char initialized;
    char threadExit;

    // Mutex for thread safety
    pthread_mutex_t mutex;
} ClientThreadArgs;

extern ClientThreadArgs** clients;
extern int clientCount;

void *clientHandler(void *arg);

// Function to set the thread exit flag
static void *setThreadExit();

#endif