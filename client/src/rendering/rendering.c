#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Include SDL2 for rendering
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h> // Include SDL_ttf for text rendering

// For threads
#include <pthread.h>

#include "rendering.h"

// Update global instances to include initialization flags
RenderingInfo renderingInfo = {
    .window = NULL,
    .renderer = NULL,
    .button = { 350, 250, 100, 50 },

    .connectButtonClicked = 0,

    .scene = CONNECT_BUTTON,

    .initialized = 0,
    .threadExit = 0,

    .mutex = PTHREAD_MUTEX_INITIALIZER
};

TTF_Font *font;
SDL_Color textColor = { 255, 255, 255, 255 }; // White color

// Texts
SDL_Texture *textTextureConnect;
SDL_Texture *textTextureLoading;

void *rendering_thread(void *arg) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return setThreadExit();
    }

    if(TTF_Init() != 0) {
        fprintf(stderr, "TTF_Init Error: %s\n", TTF_GetError());
        return setThreadExit();
    }

    renderingInfo.window = SDL_CreateWindow("SH13 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if(!renderingInfo.window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());

        SDL_Quit();
        return setThreadExit();
    }

    renderingInfo.renderer = SDL_CreateRenderer(renderingInfo.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderingInfo.renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());

        SDL_DestroyWindow(renderingInfo.window);
        SDL_Quit();

        return setThreadExit();
    }

    font = TTF_OpenFont("public/fonts/sans.ttf", 14);
    if(!font) {
        fprintf(stderr, "TTF_OpenFont Error: %s\n", TTF_GetError());

        SDL_DestroyRenderer(renderingInfo.renderer);
        SDL_DestroyWindow(renderingInfo.window);
        SDL_Quit();

        return setThreadExit();
    }

    SDL_Surface *textSurface = TTF_RenderText_Solid(font, "Connect", textColor);
    if(!textSurface) {
        fprintf(stderr, "TTF_RenderText_Solid Error: %s\n", TTF_GetError());

        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderingInfo.renderer);
        SDL_DestroyWindow(renderingInfo.window);
        SDL_Quit();

        return setThreadExit();
    }

    SDL_Texture *textTextureConnect = SDL_CreateTextureFromSurface(renderingInfo.renderer, textSurface);
    SDL_FreeSurface(textSurface);
    if(!textTextureConnect) {
        fprintf(stderr, "SDL_CreateTextureFromSurface Error: %s\n", SDL_GetError());

        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderingInfo.renderer);
        SDL_DestroyWindow(renderingInfo.window);
        SDL_Quit();

        return setThreadExit();
    }

    pthread_mutex_lock(&renderingInfo.mutex);
    renderingInfo.initialized = 1; // Mark as initialized
    pthread_mutex_unlock(&renderingInfo.mutex);

    while(1) {
        // Check if thread exit flag is set
        pthread_mutex_lock(&renderingInfo.mutex);
        if(renderingInfo.threadExit) {
            pthread_mutex_unlock(&renderingInfo.mutex);
            break;
        }
        pthread_mutex_unlock(&renderingInfo.mutex);

        // Handle SDL events
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                setThreadExit();
                break;
            } else if(event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;

                if(renderingInfo.scene == CONNECT_BUTTON) {
                    if(x >= renderingInfo.button.x && x <= renderingInfo.button.x + renderingInfo.button.w &&
                        y >= renderingInfo.button.y && y <= renderingInfo.button.y + renderingInfo.button.h) {
                        
                        pthread_mutex_lock(&renderingInfo.mutex);
                        renderingInfo.connectButtonClicked = 1; // Mark button as clicked
                        pthread_mutex_unlock(&renderingInfo.mutex);
                    }
                }
            }
        }

        // Clear the scene
        SDL_SetRenderDrawColor(renderingInfo.renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderingInfo.renderer);

        // Handle display scene management
        pthread_mutex_lock(&socketInfo.mutex);
        if(socketInfo.connected == 0) displayConnectScene();
        else displayWaitingForPlayersScene();
        pthread_mutex_unlock(&socketInfo.mutex);

        SDL_RenderPresent(renderingInfo.renderer);
    }

    SDL_DestroyTexture(textTextureConnect);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderingInfo.renderer);
    SDL_DestroyWindow(renderingInfo.window);

    TTF_Quit();
    SDL_Quit();

    return NULL;
}

void displayConnectScene() {
    // Render the button
    SDL_SetRenderDrawColor(renderingInfo.renderer, 255, 0, 0, 255); // Red button
    SDL_RenderFillRect(renderingInfo.renderer, &renderingInfo.button);

    // Render the text
    SDL_Rect textRect = {renderingInfo.button.x + 10, renderingInfo.button.y + 10, renderingInfo.button.w - 20, renderingInfo.button.h - 20};
    SDL_RenderCopy(renderingInfo.renderer, textTextureConnect, NULL, &textRect);
}

void displayWaitingForPlayersScene() {

}

// Function to set the thread exit flag
static void *setThreadExit() {
    pthread_mutex_lock(&renderingInfo.mutex);
    renderingInfo.threadExit = 1; // Mark as exited
    pthread_mutex_unlock(&renderingInfo.mutex);

    return NULL;
}