#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8888
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

// 客户端处理线程的参数结构
typedef struct {
    int socket;
    struct sockaddr_in address;
} client_info;

// 处理客户端连接的线程函数
void *handle_client(void *arg) {
    client_info *client = (client_info *)arg;
    char buffer[BUFFER_SIZE];
    char *client_ip = inet_ntoa(client->address.sin_addr);
    int client_port = ntohs(client->address.sin_port);
    
    printf("新客户端连接，IP: %s, 端口: %d\n", client_ip, client_port);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = recv(client->socket, buffer, BUFFER_SIZE, 0);
        
        if (bytes_received <= 0) {
            printf("客户端断开连接，IP: %s, 端口: %d\n", client_ip, client_port);
            break;
        }
        
        printf("收到来自 %s:%d 的消息: %s\n", client_ip, client_port, buffer);
        
        // 发送回显消息
        send(client->socket, buffer, strlen(buffer), 0);
    }

    close(client->socket);
    free(client);
    return NULL;
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;
    pthread_t thread_id;

    // 创建套接字
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        printf("套接字创建失败\n");
        return 1;
    }

    // 设置服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 设置套接字选项，允许地址重用
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        printf("设置套接字选项失败\n");
        return 1;
    }

    // 绑定套接字
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("绑定失败\n");
        close(server_socket);
        return 1;
    }

    // 监听连接
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        printf("监听失败\n");
        close(server_socket);
        return 1;
    }

    printf("服务器正在监听端口 %d...\n", PORT);

    // 持续接受新的客户端连接
    while (1) {
        client_info *client = malloc(sizeof(client_info));
        socklen_t client_addr_size = sizeof(client->address);

        // 接受客户端连接
        client->socket = accept(server_socket, (struct sockaddr*)&client->address, &client_addr_size);
        
        if (client->socket < 0) {
            printf("接受连接失败\n");
            free(client);
            continue;
        }

        // 创建新线程处理客户端连接
        if (pthread_create(&thread_id, NULL, handle_client, (void*)client) < 0) {
            printf("创建线程失败\n");
            close(client->socket);
            free(client);
            continue;
        }

        // 分离线程，使其独立运行
        pthread_detach(thread_id);
    }

    // 清理
    close(server_socket);
    return 0;
}