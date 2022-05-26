#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#define PORT 5060
// client
#define SIZE 1024

void send_file(FILE *fp, int sockfd) {
    char data[SIZE] = {0};
    while(fgets(data,SIZE,fp) != NULL){
        if(send(sockfd,data,sizeof (data),0) == -1){
            perror("error sending data");
            exit(1);
        }
        bzero(data,SIZE);
    }
}

int main(int argc, char **argv) {
    // open a TCP socket
    struct sockaddr_in server_adder, cli;

    char buf[256];
    socklen_t len;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return -1;
    }
    bzero(&server_adder, sizeof(server_adder));

    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt");
        return -1;
    }
    printf("Current: %s\n", buf);

    // connect with measure.c
    server_adder.sin_family = AF_INET;
    server_adder.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_adder.sin_port = htons(PORT);

    int connection = connect(sock, (struct sockaddr *) &server_adder, sizeof(server_adder));
    if (connection != 0) {
        printf("error connecting to server...");
    }

    // send the 1gb.txt file
    char *filename = "1gb.txt";
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        perror("Error in reading file.");
        exit(1);
    }
    for(int i =0; i < 5; i++)
    {
        send_file(fp, sock);

    }


    // change the CC
    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0) {
        perror("setsockopt");
        return -1;
    }
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) {
        perror("getsockopt");
        return -1;
    }
    printf("New: %s\n", buf);

    // send the 1gb.txt file
    for(int i =0; i < 5; i++)
    {
        send_file(fp, sock);

    }

    // close the socket
    close(sock);
    printf("finished sending files");
    return 0;
}