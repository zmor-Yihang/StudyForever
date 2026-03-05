#include "tcp_client.h"

/* socket 句柄 */
int tcp_client_socket;

/*
 * @brief TCP 客户端初始化函数
 * @details 
 *   1. 创建一个 TCP Socket（IPv4，流式传输）
 *   2. 配置目标服务器地址结构体
 *   3. 发起连接请求到目标服务器
 * @note 
 *   - IP 和 PORT 由 tcp_client.h 中的宏定义指定
 *   - 连接后会自动启用非阻塞模式以支持异步通信
 * @return 无返回值，连接结果可通过全局变量 tcp_client_socket 判断
 */
void tcp_client_init(void)
{
    /* 创建 TCP Socket：AF_INET（IPv4）+ SOCK_STREAM（TCP） */
    tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0);

    /* 初始化目标服务器地址结构体 */
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;                    /* 地址族：IPv4 */
    inet_pton(AF_INET, SERVER_IP, &dest_addr.sin_addr);       /* 转换 IP 字符串为网络字节序 */
    dest_addr.sin_port = htons(SERVER_PORT);                  /* 转换端口为网络字节序 */
    memset(dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero)); /* 清空填充字段 */

    /* 发起连接请求到目标服务器 */
    connect(tcp_client_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

/*
 * @brief 发送数据到已连接的服务器
 * @param[in] socket - Socket 文件描述符
 * @param[in] data - 待发送数据的指针（以 '\0' 结尾的字符串）
 * @note 函数会自动计算字符串长度进行发送
 * @return 无返回值
 */
void tcp_client_send_data(int socket, uint8_t *data)
{
    /* 使用 send 函数发送数据，长度由 strlen 计算得出 */
    send(socket, data, strlen((char *)data), 0);
}

/*
 * @brief 从服务器接收数据（非阻塞模式）
 * @param[in] socket - Socket 文件描述符
 * @param[out] buffer - 接收数据的缓冲区指针
 * @param[in] max_len - 缓冲区最大长度
 * @details
 *   该函数使用 MSG_DONTWAIT 标志实现非阻塞接收：
 *   - 如果有数据可读：返回接收到的字节数
 *   - 如果无数据可读：立即返回 -1（不阻塞等待）
 *   - 接收完成后自动添加字符串结束符 '\0'
 * @return 
 *   - 返回值 > 0：接收到的字节数
 *   - 返回值 == 0：连接已被对方关闭
 *   - 返回值 < 0：无数据可读或发生错误
 */
int tcp_client_receive_data(int socket, uint8_t *buffer, int max_len)
{
    /* 清空缓冲区，防止残留数据影响 */
    memset(buffer, 0, max_len);
    
    /* 使用 MSG_DONTWAIT 标志进行非阻塞接收 */
    int ret = recv(socket, buffer, max_len - 1, MSG_DONTWAIT);
    
    /* 在接收到的数据末尾添加字符串结束符，方便后续处理 */
    if (ret > 0) {
        buffer[ret] = '\0';
    }
    
    /* 返回接收到的字节数 */
    return ret;
}

/*
 * @brief 关闭 TCP 连接
 * @param[in] socket - 要关闭的 Socket 文件描述符
 * @details 
 *   释放 Socket 资源，断开与服务器的连接
 *   关闭后该 Socket 不能再使用
 * @return 无返回值
 */
void tcp_client_close(int socket)
{
    /* 关闭 Socket，释放系统资源 */
    close(socket);
}