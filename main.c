/* Created and modified by WuZhuofan.
 * All rights reserved.*/
#define _XOPEN_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

void client(int connect_socket);
int64_t htobe64(int64_t host_64bits);
uint64_t be64toh(uint64_t big_endian_64bits);
enum Operator{
    ADD,
    SUB,
    MUL,
    DIV,
    MOD
};

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
    printf("[cli] server [%s:%d] is connected!\n",ipAddress,port);
    client(connect_socket);
    close(connect_socket);
    printf("[cli] connfd is closed!\n");
    printf("[cli] client is going to exit!\n");
    return 0;
}

void client(int connect_socket){
    while(1){
        char command[6];
        int64_t operator_1, operator_2;
        if (scanf("%6s", command) != 1) {
            fprintf(stderr, "Input format is incorrect.\n");
            continue;
        }
        if (strcmp(command, "EXIT") == 0) {
            printf("[cli] EXIT received\n");
            break;
        } else if (strcmp(command, "SIGINT") == 0) {
            break;
        } else if (strcmp(command, "ADD") == 0||strcmp(command, "SUB") == 0||strcmp(command, "DIV") == 0||strcmp(command, "MUL") == 0||strcmp(command, "MOD") == 0) {
            // 读取两个64位整数
            if (scanf(" %" SCNd64 " %" SCNd64, &operator_1, &operator_2) == 2) {
            } else {
                fprintf(stderr, "Input format is incorrect for ADD command.\n");// 清除输入缓冲区中的错误输入
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
            }
        } else {
            fprintf(stderr, "Unknown command: %s\n", command);
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }

        enum Operator operator;
        if (strcmp(command, "ADD") == 0) {
            operator = ADD;
        } else if (strcmp(command, "SUB") == 0) {
            operator = SUB;
        } else if (strcmp(command, "MUL") == 0) {
            operator = MUL;
        } else if (strcmp(command, "DIV") == 0) {
            operator = DIV;
        } else if (strcmp(command, "MOD") == 0) {
            operator = MOD;
        } else {
            printf("Invalid instruction: %s\n", command);
        }

        int32_t data;
        switch (operator) {
            case ADD:{
                data=0x00000001;
                break;
            }
            case SUB:{
                data=0x00000002;
                break;
            }
            case MUL:{
                data=0x00000004;
                break;
            }
            case DIV:{
                data=0x00000008;
                break;
            }
            case MOD:{
                data=0x00000010;
                break;
            }
        }
        int32_t networkData1 = htonl(data);
        int64_t networkData2 = htobe64(operator_1);
        int64_t networkData3 = htobe64(operator_2);
        char *pdu = (char*) malloc(20);
        memcpy(pdu, &networkData1, sizeof(networkData1));
        memcpy(pdu + sizeof(networkData1), &networkData2, sizeof(networkData2));
        memcpy(pdu + sizeof(networkData1) + sizeof(networkData2), &networkData3, sizeof(networkData3));
        ssize_t bytesWritten = write(connect_socket, pdu, sizeof(pdu));
        if (bytesWritten < 0) {
            perror("Error in write");
        }

        char buffer[8];
        ssize_t bytesRead = read(connect_socket, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("Error in read");
        }
        int64_t receivedData;
        memcpy(&receivedData, buffer, sizeof(int64_t));
        receivedData = be64toh(receivedData);

        switch (operator) {
            case ADD:{
                printf("[rep_rcv] %ld + %ld = %ld\n",operator_1,operator_2,receivedData);
                break;
            }
            case SUB:{
                printf("[rep_rcv] %ld - %ld = %ld\n",operator_1,operator_2,receivedData);
                break;
            }
            case MUL:{
                printf("[rep_rcv] %ld * %ld = %ld\n",operator_1,operator_2,receivedData);
                break;
            }
            case DIV:{
                printf("[rep_rcv] %ld / %ld = %ld\n",operator_1,operator_2,receivedData);
                break;
            }
            case MOD:{
                printf("[rep_rcv] %ld %% %ld = %ld\n",operator_1,operator_2,receivedData);
                break;
            }
        }
    }
}

int64_t htobe64(int64_t host_64bits) {
    int64_t result = 0;
    unsigned char *ptr = (unsigned char *)&result;
    ptr[0] = (host_64bits >> 56) & 0xFF;
    ptr[1] = (host_64bits >> 48) & 0xFF;
    ptr[2] = (host_64bits >> 40) & 0xFF;
    ptr[3] = (host_64bits >> 32) & 0xFF;
    ptr[4] = (host_64bits >> 24) & 0xFF;
    ptr[5] = (host_64bits >> 16) & 0xFF;
    ptr[6] = (host_64bits >> 8) & 0xFF;
    ptr[7] = host_64bits & 0xFF;
    return result;
}

uint64_t be64toh(uint64_t big_endian_64bits) {
    uint64_t result = 0;
    unsigned char *ptr = (unsigned char *)&big_endian_64bits;
    result = ((uint64_t)ptr[0] << 56) |
             ((uint64_t)ptr[1] << 48) |
             ((uint64_t)ptr[2] << 40) |
             ((uint64_t)ptr[3] << 32) |
             ((uint64_t)ptr[4] << 24) |
             ((uint64_t)ptr[5] << 16) |
             ((uint64_t)ptr[6] << 8) |
             (uint64_t)ptr[7];
    return result;
}