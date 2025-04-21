#ifndef RENDERING_H
#define RENDERING_H

#include <pthread.h>

#include <SDL2/SDL.h>

// Display management
typedef enum {
    CONNECT_BUTTON,
    WAITING_FOR_PLAYERS,
    PLAY,
} Scene;

// Data struct for the rendering thread
typedef struct {
    // SDL2 elements
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect button;

    // Event handling
    int connectButtonClicked;

    // State
    Scene scene;

    // Thread event handling
    int initialized;
    int threadExit;

    // Mutex for thread safety
    pthread_mutex_t mutex;
} RenderingInfo;

// Function prototype for the rendering thread
void *rendering_thread(void *arg);

// Scenes display
void displayConnectScene();
void displayWaitingForPlayersScene();

// Function to set the thread exit flag
static void *setThreadExit();

#include "../socket/socket.h"
extern SocketInfo socketInfo;

#endif // RENDERING_H
