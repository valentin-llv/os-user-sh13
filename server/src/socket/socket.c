#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// For sockets
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../clientHandler/clientHandler.h"

#include "socket.h"

SocketListenInfo socketInfo = {
    .sock = -1,

    .initialized = 0,
    .threadExit = 0,

    .mutex = PTHREAD_MUTEX_INITIALIZER
};

void *socketListenThread(void* args) {
    struct sockaddr_in serverAddr;
    pthread_t tid;

    // Create socket
    if((socketInfo.sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return setThreadExit();
    }

    // Bind
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);
    if(bind(socketInfo.sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(socketInfo.sock);

        return setThreadExit();
    }

    // Listen
    if(listen(socketInfo.sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");

        close(socketInfo.sock);
        return setThreadExit();
    }

    printf("[SERVER] Listening on port %d...\n", SERVER_PORT);

    // Accept loop
    while(1) {
        ClientThreadArgs *args = malloc(sizeof(ClientThreadArgs));

        socklen_t clientAddrLen = sizeof(args->client_addr);
        args->client_sock = accept(socketInfo.sock, (struct sockaddr *)&args->client_addr, &clientAddrLen);
        if(args->client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        // Save thread info
        clients = (ClientThreadArgs **) realloc(clients, (clientCount + 1) * sizeof(ClientThreadArgs *));
        clients[clientCount] = args;
        clientCount++;
        
        // Create a new thread for the client
        if(pthread_create(&tid, NULL, clientHandler, args) != 0) {
            perror("Failed to create thread for client");

            close(args->client_sock);
            free(args);
        } else pthread_detach(tid);
    }

    close(socketInfo.sock);
}

// Function to set the thread exit flag
static void *setThreadExit() {
    pthread_mutex_lock(&socketInfo.mutex);
    socketInfo.threadExit = 1; // Mark as exited
    pthread_mutex_unlock(&socketInfo.mutex);

    return NULL;
}