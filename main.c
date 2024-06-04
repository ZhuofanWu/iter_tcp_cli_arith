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
#include <inttypes.h>

void client(int connect_socket);
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
    printf("[cli] server [%s:%d] is connected!",ipAddress,port);
    client(connect_socket);
    close(connect_socket);

    return 0;
}

void client(int connect_socket){
    while(1){
        char command[6];
        int64_t operator_1, operator_2;
        if (scanf("%5s", command) != 1) { //input problem?
            fprintf(stderr, "Input format is incorrect.\n");
            continue; // 重新读取输入
        }
        // 检查特殊命令
        if (strcmp(command, "EXIT") == 0) {
            printf("[cli] EXIT received\n");
            break; // 退出循环，结束程序
        } else if (strcmp(command, "SIGINT") == 0) {
            break;
        } else {
            // 读取两个64位整数
            if (scanf("%3s %" SCNd64 " %" SCNd64, command, &operator_1, &operator_2) == 2) {
            } else {
                fprintf(stderr, "Input format is incorrect for ADD command.\n");
                // 清除输入缓冲区中的错误输入
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
            }
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

        switch (operator) {
            case ADD:{

            }
        }
    }
}