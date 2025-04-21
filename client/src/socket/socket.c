#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// For sockets
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

// For threads
#include <pthread.h>

#include "../utils/utils.h"

#include "socket.h"
#include <sys/select.h>

SocketInfo socketInfo = {
    .sock = -1,

    .connected = 0,

    .initialized = 0,
    .threadExit = 0,

    .pipeSource = -1,
    .pipeDest = -1,
    
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

void *socket_thread(void *arg) {
    // Get thread args
    SocketThreadArgs *threadArgs = (SocketThreadArgs *) arg;
    char* ipAdress = NULL;
    int adrType = 0;
    int serverPort = threadArgs->serverPort;

    // Verify provided IP address and its type: IPV4 or IPV6
    int ipVerificationResult = isValidIpAddress(threadArgs->serverAdress);

    // Gather IP adress to use
    if(ipVerificationResult <= 0) {
        // Invalid IP, user might have used a domain name instead of an IP address
        int dnsResult = dnsSearch(threadArgs->serverAdress, &ipAdress, &adrType);

        if(dnsResult > 0) {
            char* adressTypeName = (char *) malloc(5 * sizeof(char));

            // Check if malloc failed
            if(adressTypeName == NULL) return setThreadExit();

            if(adrType == AF_INET) sprintf(adressTypeName, "IPv4");
            else sprintf(adressTypeName, "IPv6");
            
            printf("Domain name %s resolved into an %s adress: %s\n", threadArgs->serverAdress, adressTypeName, ipAdress);
            free(adressTypeName);
        } else {
            printf("Domain name %s failed to resolve into an IP adress\n", threadArgs->serverAdress);
            return setThreadExit();
        }
    } else {
        // User provided a valid IP address
        ipAdress = threadArgs->serverAdress;
        adrType = ipVerificationResult;
    }

    // Create socket
    socketInfo.sock = socket(adrType, SOCK_STREAM, 0);// Use TCP socket
    if (socketInfo.sock < 0) {
        printf("Socket creation failed\n");
        return setThreadExit();
    }

    // Setup socket
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = adrType;
    serv_addr.sin_port = htons(serverPort);

    int res = inet_pton(adrType, ipAdress, &serv_addr.sin_addr);
    if (res <= 0) {
        printf("Invalid address / address not supported\n");
        return setThreadExit();
    }

    pthread_mutex_lock(&socketInfo.mutex);
    socketInfo.initialized = 1; // Mark as initialized
    pthread_mutex_unlock(&socketInfo.mutex);

    // Wait until the user clicks the connect button to start or an exit signal is received
    while (1) {
        pthread_mutex_lock(&renderingInfo.mutex);
        if(renderingInfo.connectButtonClicked) {
            renderingInfo.connectButtonClicked = 0; // Reset the button click flag

            pthread_mutex_unlock(&renderingInfo.mutex);
            break; // Exit the loop to proceed with the connection
        }
        pthread_mutex_unlock(&renderingInfo.mutex);

        pthread_mutex_lock(&socketInfo.mutex);
        if(socketInfo.threadExit) {
            pthread_mutex_unlock(&socketInfo.mutex);
            return setThreadExit();
        }
        pthread_mutex_unlock(&socketInfo.mutex);
    }

    // Connect to the server
    int status = connect(socketInfo.sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (status < 0) {
        printf("Connection to server failed\n");

        close(socketInfo.sock);
        return setThreadExit();
    } else {
        cleanPrintRet();
        printf("Successfully connected to server at %s:%d\n", ipAdress, serverPort);
        cleanPrintRestore();
    }

    pthread_mutex_lock(&socketInfo.mutex);
    socketInfo.connected = 1; // Mark as connected
    pthread_mutex_unlock(&socketInfo.mutex);

    // Setup self-pipe to handle thread exit signal
    int pipefd[2];
    if(pipe(pipefd) == -1) {
        perror("pipe");
        return setThreadExit();
    }

    socketInfo.pipeSource = pipefd[0];
    socketInfo.pipeDest = pipefd[1];

    // Setup the file descriptor set for select
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(socketInfo.sock, &readfds);
    FD_SET(socketInfo.pipeSource, &readfds);

    int maxfd = socketInfo.sock > socketInfo.pipeSource ? socketInfo.sock : socketInfo.pipeSource;

    // Communication loop
    char buffer[1024] = {0};
    while(1) {
        // Wait for data on the socket or the pipe
        int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if(activity < 0) {
            perror("select");
            break;
        }

        // Check if data is available on the pipe
        if(FD_ISSET(socketInfo.pipeSource, &readfds)) {
            // Read the signal from the pipe
            char signal;
            read(socketInfo.pipeSource, &signal, sizeof(signal));

            if(signal == 'e') break; // Exit the loop to close the connection
        } else if(FD_ISSET(socketInfo.sock, &readfds)) {
            // Data is available on the socket
            int valread = read(socketInfo.sock, buffer, sizeof(buffer));

            if(valread > 0) {
                printf("Server: %s\n", buffer);
                // Process the received message here
            } else if(valread == 0) {
                printf("\x1b[1;31mServer closed the connection\x1b[0m\n");
                break; // Exit the loop to close the connection
            } else {
                perror("Read error");
                break; // Exit the loop on read error
            }
        }
    }

    close(socketInfo.sock);
    return setThreadExit();
}

static int sendMessageToServer(char action, char *params[], int paramCount) {
    if(socketInfo.sock == -1) {
        fprintf(stderr, "Socket is not connected.\n");
        return -1;
    }

    // Construct the message
    char message[1024] = {0};
    message[0] = action;
    strcat(message, " ");

    for(int i = 0; i < paramCount; i++) {
        strcat(message, params[i]);
        if(i < paramCount - 1) {
            strcat(message, " ");
        }
    }

    // Send the message to the server
    if(write(socketInfo.sock, message, strlen(message)) == -1) {
        perror("Failed to send message to server");
        return -1;
    }

    return 0;
}

static int isValidIpAddress(char *ipAddress) {
    struct sockaddr_in sa;

    // Assume IPV 4
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));

    if(result == 1) return AF_INET;
    else if(result == 0) { // Incorrect IP adress family
        // Try IPV 6
        result = inet_pton(AF_INET6, ipAddress, &(sa.sin_addr));

        if(result <= 0) return result; // Invalid IP
        else return AF_INET6;
    } else return result; // Invalid IP adress format (IPV 4 or IPV 6), return error
}

static int dnsSearch(char* domain, char** ipAdress, int* adrType) {
    // Do a DNS search to resolve domain name
    struct addrinfo* dnsResult = NULL;
    int result = getaddrinfo(domain, 0, 0, &dnsResult);

    if(result != 0) return -1;

    // Get IP adress type
    *adrType = dnsResult->ai_addr->sa_family;

    int adrSize = *adrType == AF_INET ? INET_ADDRSTRLEN : INET6_ADDRSTRLEN;
    *ipAdress = (char *) malloc(adrSize * sizeof(char));

    // Check if malloc failed
    if(*ipAdress == NULL) return -1;

    // Convert IP address to string
    if(dnsResult->ai_addr->sa_family == AF_INET) { // IPV4 address found
        struct sockaddr_in *p = (struct sockaddr_in *) dnsResult->ai_addr;
        inet_ntop(AF_INET, &p->sin_addr, *ipAdress, adrSize);
    } else if (dnsResult->ai_addr->sa_family == AF_INET6) { // IPV6 address found
        struct sockaddr_in6 *p = (struct sockaddr_in6 *) dnsResult->ai_addr;
        inet_ntop(AF_INET6, &p->sin6_addr, *ipAdress, adrSize);
    }

    freeaddrinfo(dnsResult);
    return 1;
}

// Function to set the thread exit flag
static void *setThreadExit() {
    pthread_mutex_lock(&socketInfo.mutex);
    socketInfo.threadExit = 1; // Mark as exited
    pthread_mutex_unlock(&socketInfo.mutex);

    return NULL;
}