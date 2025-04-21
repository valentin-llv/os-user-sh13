#ifndef MINISHELL_H
#define MINISHELL_H

#include <pthread.h>

typedef struct {
    char name[64];

    // Thread events
    char initialized;
    char threadExit;
    int pipeSource;
    int pipeDest;

    // Mutex for thread safety
    pthread_mutex_t mutex;
} MinishellInfo;

void *minishell(void *arg);

// Function to set the thread exit flag
static void *setThreadExit();

#endif