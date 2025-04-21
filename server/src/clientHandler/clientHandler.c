#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// For sockets
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

// For threading
#include <pthread.h>

#include "clientHandler.h"

// Init global client array
ClientThreadArgs** clients = NULL;
int clientCount = 0;

void *clientHandler(void *arg) {
    ClientThreadArgs *clientArgs = (ClientThreadArgs *)arg;
    int client_sock = clientArgs->client_sock;

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientArgs->client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

    printf("[SERVER] New client connected: %s\n", client_ip);
    
    while(1) {
        // Check for thread exit flag
        pthread_mutex_lock(&clientArgs->mutex);
        if(clientArgs->threadExit) {
            pthread_mutex_unlock(&clientArgs->mutex);
            break;
        }
        pthread_mutex_unlock(&clientArgs->mutex);

        // Handle client communication
        // TODO
    }

    close(client_sock);
    return setThreadExit(clientArgs);
}

// Function to set the thread exit flag
static void *setThreadExit(ClientThreadArgs *clientArgs) {
    pthread_mutex_lock(&clientArgs->mutex);
    clientArgs->threadExit = 1; // Mark as exited
    pthread_mutex_unlock(&clientArgs->mutex);

    free(clientArgs);

    return NULL;
}