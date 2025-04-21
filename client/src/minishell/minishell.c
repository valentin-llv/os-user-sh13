// For threading
#include <pthread.h>

// For select
#include <sys/select.h>
#include <unistd.h>

#include "../utils/utils.h"

#include "minishell.h"

MinishellInfo minishellInfo = {
    .initialized = 0,
    .threadExit = 0,

    .pipeSource = -1,
    .pipeDest = -1,

    .mutex = PTHREAD_MUTEX_INITIALIZER
};

void *minishell(void *arg) {
    // Setup self-pipe to handle thread exit signal
    int pipefd[2];
    if(pipe(pipefd) == -1) {
        perror("pipe");
        return setThreadExit();
    }

    minishellInfo.pipeSource = pipefd[0];
    minishellInfo.pipeDest = pipefd[1];

    // Setup the file descriptor set for select
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(minishellInfo.pipeSource, &readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    int maxfd = minishellInfo.pipeSource > STDIN_FILENO ? minishellInfo.pipeSource : STDIN_FILENO;

    // Set the thread as initialized
    pthread_mutex_lock(&minishellInfo.mutex);
    minishellInfo.initialized = 1; // Mark as initialized
    pthread_mutex_unlock(&minishellInfo.mutex);

    // Wait socket and rendering threads are initialized
    while(1) {
        pthread_mutex_lock(&socketInfo.mutex);
        if(socketInfo.initialized) {
            pthread_mutex_unlock(&socketInfo.mutex);
            break;
        }
        pthread_mutex_unlock(&socketInfo.mutex);

        pthread_mutex_lock(&renderingInfo.mutex);
        if(renderingInfo.initialized) {
            pthread_mutex_unlock(&renderingInfo.mutex);
            break;
        }
        pthread_mutex_unlock(&renderingInfo.mutex);

        pthread_mutex_lock(&minishellInfo.mutex);
        if(minishellInfo.threadExit) {
            pthread_mutex_unlock(&minishellInfo.mutex);
            return setThreadExit();
        }
        pthread_mutex_unlock(&minishellInfo.mutex);
    }

    // Print shell>
    printf("minishell> ");
    fflush(stdout); // Force flush buffer

    char input[256];
    while(1) {
        // Wait for data on the pipe or stdin
        int activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if(activity < 0) {
            perror("select");
            break;
        }

        // Check if data is available on the pipe
        if(FD_ISSET(minishellInfo.pipeSource, &readfds)) {
            // Read the signal from the pipe
            char signal;
            read(minishellInfo.pipeSource, &signal, sizeof(signal));

            if(signal == 'e') break; // Exit signal received
        } else if(FD_ISSET(STDIN_FILENO, &readfds)) {
            // Read input from stdin
            fgets(input, sizeof(input), stdin);

            // Remove trailing newline
            input[strcspn(input, "\n")] = 0;

            if(strcmp(input, "help") == 0) {
                printf("Available commands:\n");
                printf("  help - Show this help message\n");
                printf("  exit - Exit the program\n");
            } else if(strcmp(input, "exit") == 0) {
                break;
            } else printf("Unknown command: %s\n", input);

            printf("minishell> ");
            fflush(stdout);
        }
    }

    return setThreadExit();
}

// Function to set the thread exit flag
static void *setThreadExit() {
    pthread_mutex_lock(&minishellInfo.mutex);
    minishellInfo.threadExit = 1; // Mark as exited
    pthread_mutex_unlock(&minishellInfo.mutex);

    return NULL;
}