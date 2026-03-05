#ifndef TCP_CLIENT_H__
#define TCP_CLIENT_H__

#define SERVER_IP "192.168.4.2"
#define SERVER_PORT 42650

#include <string.h>
#include "lwip/netdb.h"
#include "lwip/sockets.h"

extern int tcp_client_socket;

void tcp_client_init(void);
void tcp_client_send_data(int socket, uint8_t *data);
int tcp_client_receive_data(int socket, uint8_t *buffer, int max_len);
void tcp_client_close(int socket);

#endif // TCP_CLIENT_H__
