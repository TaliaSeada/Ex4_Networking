#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#define PORT 5060
// client
#define SIZE 1048576

void send_file(FILE *fp, int sockfd) {
    size_t num = 0;
    char data[SIZE] = {0};
    struct timeval begin,end;
    gettimeofday(&begin, 0);
    while (fgets(data, SIZE, fp) != NULL) {
        if ((num += send(sockfd, data, SIZE, 0)) == -1) { //to check if the file is sent or not
            perror("sending file error");
            exit(1);
        }
//        num = num + strlen(data); //to get the lenght of the file
        bzero(data, SIZE);
    }
    gettimeofday(&end, 0);

    printf("%f second to send\n", (double)(end.tv_sec - begin.tv_sec) + (double)(end.tv_usec - begin.tv_usec)*1e-6);
    printf("%ld bytes were sent\n", num);
}

int main(int argc, char **argv) {
    // open a TCP socket
    struct sockaddr_in server_adder;

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
    printf("connected to server\n");
    // send the 1gb.txt file
    char *filename = "1gb.txt";
    FILE *fp;


    for(int i =0; i < 5; i++)
    {
        fp = fopen(filename, "r");
        if(fp == NULL){
            perror("Error in reading file.");
            exit(1);
        }
        send_file(fp, sock);
        fclose(fp);

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
        fp = fopen(filename, "r");
        if(fp == NULL){
            perror("Error in reading file.");
            exit(1);
        }
        send_file(fp, sock);
        fclose(fp);

    }
    // close the socket
    close(sock);
    printf("finished sending files");
    return 0;
}