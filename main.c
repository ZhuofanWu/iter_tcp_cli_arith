/* Created and modified by WuZhuofan.
 * All rights reserved.*/
/* 2024-06-22 14:38:34 +0800`s submit ----Correctness 100.0
 * This commit will be the last commit to Git&Github. Here are some of my thoughts:
 * I wrote the basic framework of the programme before the exam, but the functions for byte order conversion were a big hurdle.
 * You can see in my Git commit history that I've been struggling between using the official library and designing my own function.
 * Later, I discovered the limitations of the official library and decided to design my own byte order conversion function.
 * After completing the coding in the early morning of today, I tested the function and it ran smoothly.
 * However, during the debugging process using the memcpy() function,
 * I noticed that the data in the "pdu" variable changed from 255 to -1, even though they were both '\377'.
 * Upon researching, I found out that this was due to the difference in "unsigned" representation.
 * From that point on, all the difficulties were resolved.
 *
 * The biggest realization I gained from this experiment is that when encountering a bug,
 * it is beneficial to isolate and debug the core functionality separately to identify the problem.
 * Initially, I was debugging the client and server programs together, which made it difficult to pinpoint the exact error,
 * and only increased the amount of unnecessary code.
 * However, after reorganizing my thoughts and approach, the key problem was easily resolved.
 */
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
int64_t hton64(int64_t host64);
int64_t ntoh64(int64_t net64);
int32_t hton32(int32_t host32);
int32_t ntoh32(int32_t net32);

enum Operator{
    ADD,SUB,MUL,DIV,MOD };

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
    printf("[cli] server[%s:%d] is connected!\n",ipAddress,port);
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
        data = hton32(data);
        operator_1 = hton64(operator_1);
        operator_2 = hton64(operator_2);

        unsigned char pdu[20]={};
        memcpy(pdu, &data, sizeof(int32_t));
        memcpy(pdu + sizeof(int32_t), &operator_1, sizeof(int64_t));
        memcpy(pdu + sizeof(int32_t) + sizeof(int64_t), &operator_2, sizeof(int64_t));

        ssize_t bytesWritten = write(connect_socket, pdu, sizeof(pdu));
        if (bytesWritten < 0) {
            perror("Error in write");
        }

        int64_t receivedData;
        char buffer[8];
        ssize_t bytesRead = read(connect_socket, buffer, sizeof(buffer));
        if (bytesRead < 0) {
            perror("Error in read");
        }
        memcpy(&receivedData, buffer, sizeof(int64_t));
        receivedData = ntoh64(receivedData);
        operator_1 = ntoh64(operator_1);
        operator_2 = ntoh64(operator_2);
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

int64_t hton64(int64_t host64) {
    union {
        int64_t i64;
        unsigned char bytes[8];
    } value;

    value.i64 = host64;

    unsigned char temp = value.bytes[0];
    value.bytes[0] = value.bytes[7];
    value.bytes[7] = temp;

    temp = value.bytes[1];
    value.bytes[1] = value.bytes[6];
    value.bytes[6] = temp;

    temp = value.bytes[2];
    value.bytes[2] = value.bytes[5];
    value.bytes[5] = temp;

    temp = value.bytes[3];
    value.bytes[3] = value.bytes[4];
    value.bytes[4] = temp;

    return value.i64;
}

int64_t ntoh64(int64_t net64) {
    return hton64(net64);
}

int32_t hton32(int32_t host32) {
    union {
        int32_t i32;
        unsigned char bytes[4];
    } value;
    value.i32=host32;
    unsigned char temp = value.bytes[0];
    value.bytes[0] = value.bytes[3];
    value.bytes[3] = temp;

    temp = value.bytes[1];
    value.bytes[1] = value.bytes[2];
    value.bytes[2] = temp;

    return value.i32;
}

int32_t ntoh32(int32_t net32) {
    return hton32(net32);
}