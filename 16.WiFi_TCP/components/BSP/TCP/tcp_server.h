#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define SERVER_PORT 8080

extern int tcp_server_socket;
extern int client_sock;

void tcp_server_init(void);
void tcp_server_send_data(int socket, uint8_t *data);
int tcp_server_receive_data(int socket, uint8_t *buffer, size_t max_len);
void tcp_server_close(void);

#endif // TCP_SERVER_H__
