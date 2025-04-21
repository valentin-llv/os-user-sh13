#ifndef SOCKET_H
#define SOCKET_H

#include <pthread.h>

// Data struct for the socket thread
typedef struct {
    // Socket variables
    int sock;

    // Status variables
    char connected;

    // Thread events
    char initialized;
    char threadExit;
    int pipeSource;
    int pipeDest;

    // Mutex for thread safety
    pthread_mutex_t mutex;
} SocketInfo;

// Define a struct for thread arguments
typedef struct {
    char *serverAdress;
    int serverPort;
} SocketThreadArgs;

// Function prototype for the socket thread
void *socket_thread(void *arg);

// Static functions
static int sendMessageToServer(char action, char *params[], int paramCount);

static int isValidIpAddress(char *ipAddress);
static int dnsSearch(char* domain, char** ipAdress, int* adrType);

// Function to set thread exit flag
static void *setThreadExit();

#include "../rendering/rendering.h"
extern RenderingInfo renderingInfo;

#endif // SOCKET_H
