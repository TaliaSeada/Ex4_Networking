#include <sys/socket.h> 
#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 

// client
int main(int argc, char **argv) {
    // open a TCP socket
    char buf[256];
    socklen_t len;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
 	if (sock == -1) {
 		perror("socket");
        return -1;
    } 

    len = sizeof(buf); 
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0) { 
        perror("getsockopt");
        return -1;
    }
    printf("Current: %s\n", buf); 

    // connect with measure.c


    // send the 1gb.txt file


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


    // close the socket
    close(sock); 
    return 0; 
}