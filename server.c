#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    int client_addr_size = sizeof(client_addr);

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup 失败\n");
        return 1;
    }

    // 创建套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("套接字创建失败\n");
        WSACleanup();
        return 1;
    }

    // 设置服务器地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 绑定套接字
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("绑定失败\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // 监听连接
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("监听失败\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("服务器正在监听端口 %d...\n", PORT);

    // 接受客户端连接
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_socket == INVALID_SOCKET) {
        printf("接受连接失败\n");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("客户端已连接\n");

    // 接收和发送数据
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client_socket, buffer, BUFFER_SIZE, 0) > 0) {
            printf("收到消息: %s\n", buffer);
            
            // 发送回显消息
            send(client_socket, buffer, strlen(buffer), 0);
        } else {
            break;
        }
    }

    // 清理
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}