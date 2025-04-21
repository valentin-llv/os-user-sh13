#ifndef MAIN_H
#define MAIN_H

extern RenderingInfo renderingInfo;
extern SocketInfo socketInfo;
extern MinishellInfo minishellInfo;

// Thread functions
void *minishell(void *arg);

#endif