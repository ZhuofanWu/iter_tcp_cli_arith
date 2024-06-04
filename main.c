/* Created and modified by WuZhuofan.
 * All rights reserved.*/
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void client(int connect_socket);

int main(int argc, char *argv[]) {
    char *ipAddress = argv[1];
    int port = atoi(argv[2]);
    int connect_socket=socket(AF_INET, SOCK_STREAM, 0);
    if (connect_socket < 0) {
        perror("Error in socket");
        exit(1);
    }
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, ipAddress, &(server_address.sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        exit(1);
    }
    if (connect(connect_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error in connect");
        exit(1);
    }
    client(connect_socket);
    close(connect_socket);

    return 0;
}

void client(int connect_socket){
    while(1){

    }
}