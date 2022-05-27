#include<stdio.h>

#if defined _WIN32
#include<winsock2.h>   //winsock2 should be before windows
#pragma comment(lib,"ws2_32.lib")
#else
// Linux and other UNIXes
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#endif
#define SIZE 1024
#define SERVER_PORT 5060  //The port that the server listens
#define SIZEFILE 1048576
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

// server
int main() {
#if defined _WIN32
    // Windows requires initialization
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
#else
    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket
#endif

    // Open the listening (server) socket
    int listeningSocket = -1;

    if ((listeningSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Could not create listening socket : %d"
#if defined _WIN32
                ,WSAGetLastError()
#else
                , errno
#endif
        );
    }

    // Reuse the address if the server socket on was closed
    // and remains for 45 seconds in TIME-WAIT state till the final removal.
    int enableReuse = 1;
    if (setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR,
#if defined _WIN32
            (const char*)
#endif
                   &enableReuse,

                   sizeof(int)) < 0) {
        printf("setsockopt() failed with error code : %d",
#if defined _WIN32
                WSAGetLastError()
#else
               errno
#endif
        );
    }

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(SERVER_PORT);  //network order

    // Bind the socket to the port with any IP at this port
    if (bind(listeningSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1) {
        printf("Bind failed with error code : %d",errno);
        return -1;
    }

    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    if (listen(listeningSocket, 500) == -1) //500 is a Maximum size of queue connection requests
        //number of concurrent connections
    {
        printf("listen() failed with error code : %d",errno);
        return -1;
    }

    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");
    struct sockaddr_in clientAddress;  //
    socklen_t clientAddressLen = sizeof(clientAddress);
    double times[5] = {0};
    double sum = 0;
    double avgC;
    double sum2 = 0;
    double avgR;
    double times2[5] = {0};
    int clientSocket = accept(listeningSocket,(struct sockaddr*)&clientAddress,&clientAddressLen);
    char buffer[SIZEFILE];
    size_t oldFile = 0;
    unsigned long sumGot = 0;
    size_t currentRecive = 0;
    while(sumGot < 10 * SIZEFILE)
    {
        struct timeval begin, end;
        gettimeofday(&begin,0);
        currentRecive += recv(clientSocket,buffer,SIZEFILE,0);
        sumGot += currentRecive;
        currentRecive = 0;
        if(sumGot <= 0)
        {
            break;
        }
        bzero(buffer,SIZEFILE);
        gettimeofday(&end,0);
        size_t file = sumGot / SIZEFILE;
        if(file != oldFile)
        {
            printf("gotten %ld bytes \n",sumGot);
            printf("getting file number: %ld\n",file);
            oldFile = file;

        }
        if(sumGot < 5 * SIZEFILE)
        {
            times[file] += (double)(end.tv_sec - begin.tv_sec) + (double)(end.tv_usec - begin.tv_usec)*1e-6;
        }
        else
        {
            times2[file - 5] += (double)(end.tv_sec - begin.tv_sec) + (double)(end.tv_usec - begin.tv_usec)*1e-6;
        }

    }
    for(int i = 0; i < 5; i++)
    {
        sum += times[i];
    }
    avgC = sum/5.0;

    for(int i = 0; i < 5; i++)
    {
        sum2 += times2[i];
    }
    avgR = sum2/5.0;

    printf("seconds for cubic 30 percent packet loss: %.6f\n", avgC);
    printf("seconds for reno 30 percent packet loss: %.6f\n", avgR);
    close(clientSocket);
    return 0;


}