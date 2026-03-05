#include "tcp_server.h"

int tcp_server_socket;
int client_sock;

void tcp_server_init(void)
{
    /* 创建 TCP 服务器套接字 */
    tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* 绑定地址和端口 */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bind(tcp_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    /* 监听 */
    listen(tcp_server_socket, 5);

    /* 接受连接 - 保存到全局变量 client_sock */
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    client_sock = accept(tcp_server_socket, (struct sockaddr *)&client_addr, &addr_len);
}

void tcp_server_send_data(int socket, uint8_t *data)
{
    send(socket, data, strlen((char *)data), 0);
}

int tcp_server_receive_data(int socket, uint8_t *buffer, size_t max_len)
{
    memset(buffer, 0, max_len);
    int len = recv(socket, buffer, max_len - 1, MSG_DONTWAIT);
    if (len > 0)
    {
        buffer[len] = '\0'; // 添加字符串结束符
    }
    return len;
}

void tcp_server_close(void)
{
    close(tcp_server_socket);
}