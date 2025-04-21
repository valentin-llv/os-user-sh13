#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// For threading
#include <pthread.h>

// Rendering thread
#include "rendering/rendering.h"
#include "socket/socket.h"
#include "minishell/minishell.h"
#include "utils/utils.h"

#include "main.h"

// Main
int main(int argc, char *argv[]) {
    char *serverIp = NULL;
    int serverPort = 0;

    char playerName[64] = { 0 };

    // Parse command-line arguments
    int result = parseCmdArgs(argc, argv, &serverIp, &serverPort, playerName);
    if(result <= 0) {
        fprintf(stderr, "Usage: %s -a <server_ip> -p <server_port> -n <player_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialize mutexes
    if(pthread_mutex_init(&globalMutex, NULL) != 0 || pthread_mutex_init(&renderingInfo.mutex, NULL) != 0 || pthread_mutex_init(&socketInfo.mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        return EXIT_FAILURE;
    }

    // Prepare thread arguments
    SocketThreadArgs thread_args = { serverIp, serverPort };

    // Create rendering thread
    pthread_t renderThread;
    if(pthread_create(&renderThread, NULL, rendering_thread, NULL) != 0) {
        perror("Failed to create rendering thread");
        return EXIT_FAILURE;
    }

    // Create socket communication thread
    pthread_t socketThread;
    if (pthread_create(&socketThread, NULL, socket_thread, &thread_args) != 0) {
        perror("Failed to create socket thread");

        pthread_cancel(renderThread);
        destroyAllMutexes();
        return EXIT_FAILURE;
    }

    // Create minishell thread
    pthread_t minishellThread;
    if (pthread_create(&minishellThread, NULL, minishell, NULL) != 0) {
        perror("Failed to create minishell thread");

        pthread_cancel(renderThread);
        pthread_cancel(socketThread);

        destroyAllMutexes();
        return EXIT_FAILURE;
    }

    // Wait for a thread to exit
    while(1) {
        pthread_mutex_lock(&socketInfo.mutex);
        if(socketInfo.threadExit) {
            pthread_mutex_unlock(&socketInfo.mutex);
            break;
        }
        pthread_mutex_unlock(&socketInfo.mutex);

        pthread_mutex_lock(&renderingInfo.mutex);
        if(renderingInfo.threadExit) {
            pthread_mutex_unlock(&renderingInfo.mutex);
            break;
        }
        pthread_mutex_unlock(&renderingInfo.mutex);

        pthread_mutex_lock(&minishellInfo.mutex);
        if(minishellInfo.threadExit) {
            pthread_mutex_unlock(&minishellInfo.mutex);
            break;
        }
        pthread_mutex_unlock(&minishellInfo.mutex);
    }

    // Send exit message to all threads
    pthread_mutex_lock(&socketInfo.mutex);
    // Set the thread exit flag
    socketInfo.threadExit = 1;

    // Send exit signal to the socket thread
    if(socketInfo.pipeDest != -1) {
        char signal = 'e';
        write(socketInfo.pipeDest, &signal, sizeof(signal));
    }
    pthread_mutex_unlock(&socketInfo.mutex);

    pthread_mutex_lock(&renderingInfo.mutex);
    renderingInfo.threadExit = 1;
    pthread_mutex_unlock(&renderingInfo.mutex);

    pthread_mutex_lock(&minishellInfo.mutex);
    // Set the thread exit flag
    minishellInfo.threadExit = 1;

    // Send exit signal to the minishell thread
    if(minishellInfo.pipeDest != -1) {
        char signal = 'e';
        write(minishellInfo.pipeDest, &signal, sizeof(signal));
    }
    pthread_mutex_unlock(&minishellInfo.mutex);

    // Fnish threads
    pthread_cancel(minishellThread);

    // Wait for threads to finish
    pthread_join(renderThread, NULL);
    pthread_join(socketThread, NULL);
    pthread_join(minishellThread, NULL);

    destroyAllMutexes();
    return 0;
}