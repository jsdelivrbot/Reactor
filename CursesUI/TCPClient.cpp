

#include "DebugText.h"
#include <stdlib.h>
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <netinet/tcp.h>
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/time.h>
#include <netdb.h>


int     sockfd;


void TCPClientConnect(char* hostname, int portno)
{
    struct sockaddr_in  serveraddr;
    struct hostent*     server;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        DebugPrintf("ERROR opening socket");
    }

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL)
    {
        DebugPrintf("ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char*) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char*)server->h_addr,
          (char*)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, (const sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        DebugPrintf("ERROR connecting");
        exit(-1);
    }

}




//
//
//
void TCPRead( void* buffer, uint32_t numberOfBytes )
{
    int n;

    /* print the server's reply */
    n = read(sockfd, buffer, numberOfBytes);
    if (n < 0)
    {
        DebugPrintf("ERROR reading from socket");
        exit(-1);
    }
}



