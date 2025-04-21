// For command-line argument parsing
#include <getopt.h>

// For threading
#include <pthread.h>

#include "utils.h"

int parseCmdArgs(int argc, char *argv[], char **serverAddress, int *serverPort, char* playerName) {
    int opt;
    while((opt = getopt(argc, argv, "a:p:n:")) != -1) {
        switch (opt) {
            case 'a':
                *serverAddress = optarg;
                break;

            case 'p':
                *serverPort = atoi(optarg);
                break;

            case 'n':
                strcpy(playerName, optarg);
                break;

            default:
                return -1;
        }
    }

    if(*serverAddress == NULL || *serverPort <= 0) return -1; // Changed condition to check for <= 0
    else return 1;
}

void cleanPrintRet() {
    printf("\x1b[0G");
}

void cleanPrintRestore() {
    printf("minishell>");
    fflush(stdout);
}

/*

    Threads management

*/

pthread_mutex_t globalMutex; // Global mutex for shared resources

// Function to destroy all mutexes
void destroyAllMutexes() {
    pthread_mutex_destroy(&globalMutex);
    pthread_mutex_destroy(&renderingInfo.mutex);
    pthread_mutex_destroy(&socketInfo.mutex);
}