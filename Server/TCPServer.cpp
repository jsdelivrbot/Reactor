


extern "C"
{
#include "DebugText.h"
}

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "FastSharedBuffer.hpp"


#define BUFFER_SIZE 1024


extern FastSharedBuffer<uint8_t,uint16_t>     lowRateBuffer;


void* TCPServer (void* p) 
{
    int                 port = 1234;
    int                 server_fd, client_fd, err;
    struct sockaddr_in  server, client;
    char                buf[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) 
    {
        exit(-1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

    err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
    if (err < 0) 
    {
        DebugPrintf("Could not bind socket\n");
    }

    err = listen(server_fd, 128);
    if (err < 0) 
    {
        DebugPrintf("Could not listen on socket\n");
    }

    DebugPrintf("Server is listening on %d\n", port);

    while (true) 
    {
        socklen_t client_len = sizeof(client);
        client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

        if (client_fd < 0) 
        {
            DebugPrintf("Could not establish new connection\n");
        }

        while (true) 
        {
            //int read = recv(client_fd, buf, BUFFER_SIZE, 0);

            //if (!read) break; // done reading
            //if (read < 0) DebugPrintf("Client read failed\n");
            //

            uint8_t     byte    = lowRateBuffer.Get();
            err = send(client_fd, &byte, 1, 0);
            if (err < 0) 
            {
                DebugPrintf("Client write failed\n");
                break;
            }

            //err = send(client_fd, buf, read, 0);
            //if (err < 0) DebugPrintf("Client write failed\n");
        }
    }

    return 0;
}




