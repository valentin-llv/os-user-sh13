#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>

int parseCmdArgs(int argc, char *argv[], char **server_ip, int *server_port, char* player_name);

void cleanPrintRet();
void cleanPrintRestore();

/*

    Threads management

*/

// Global mutex for shared resources
extern pthread_mutex_t globalMutex;

// Function to destroy all mutexes
void destroyAllMutexes();

#include "../rendering/rendering.h"
#include "../socket/socket.h"

// Global instances of rendering and socket information, used to delete their mutexes
extern RenderingInfo renderingInfo;
extern SocketInfo socketInfo;

#endif