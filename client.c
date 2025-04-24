#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup 失败\n");
        return 1;
    }

    // 创建套接字
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("套接字创建失败\n");
        WSACleanup();
        return 1;
    }

    // 设置服务器地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("连接失败\n");
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("已连接到服务器\n");

    // 发送和接收数据
    while (1) {
        printf("请输入消息: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';  // 移除换行符

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // 发送消息
        send(client_socket, buffer, strlen(buffer), 0);

        // 接收服务器的回显
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0) {
            printf("服务器回显: %s\n", buffer);
        }
    }

    // 清理
    closesocket(client_socket);
    WSACleanup();

    return 0;
}