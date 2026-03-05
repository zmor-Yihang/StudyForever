# ESP32 Socket API 完整函数参考

## 常用数据结构

### struct sockaddr

通用套接字地址结构：

```c
struct sockaddr {
    uint8_t sa_len;         // 地址长度
    sa_family_t sa_family;  // 地址族（AF_INET, AF_INET6等）
    char sa_data[14];       // 地址数据
};
```

### struct sockaddr_in (IPv4)

IPv4套接字地址结构：

```c
struct sockaddr_in {
    uint8_t sin_len;            // 结构长度
    sa_family_t sin_family;     // AF_INET
    in_port_t sin_port;         // 端口号（网络字节序）
    struct in_addr sin_addr;    // IP地址（网络字节序）
    char sin_zero[8];           // 填充字节
};
```

### struct sockaddr_in6 (IPv6)

IPv6套接字地址结构：

```c
struct sockaddr_in6 {
    uint8_t sin6_len;           // 结构长度
    sa_family_t sin6_family;    // AF_INET6
    in_port_t sin6_port;        // 端口号（网络字节序）
    uint32_t sin6_flowinfo;     // IPv6流信息
    struct in6_addr sin6_addr;  // IPv6地址
    uint32_t sin6_scope_id;     // 作用域ID
};
```

### struct addrinfo

地址信息结构（用于getaddrinfo）：

```c
struct addrinfo {
    int ai_flags;               // 输入标志（AI_PASSIVE, AI_CANONNAME等）
    int ai_family;              // 协议族（AF_INET, AF_INET6, AF_UNSPEC）
    int ai_socktype;            // 套接字类型（SOCK_STREAM, SOCK_DGRAM）
    int ai_protocol;            // 协议（IPPROTO_TCP, IPPROTO_UDP）
    socklen_t ai_addrlen;       // ai_addr的长度
    struct sockaddr *ai_addr;   // 套接字地址
    char *ai_canonname;         // 规范主机名
    struct addrinfo *ai_next;   // 链表中的下一个结构
};
```

### struct hostent

主机信息结构（用于gethostbyname）：

```c
struct hostent {
    char *h_name;           // 官方主机名
    char **h_aliases;       // 主机别名列表
    int h_addrtype;         // 主机地址类型（AF_INET）
    int h_length;           // 地址长度
    char **h_addr_list;     // 主机IP地址列表
};
```

### struct msghdr

消息头结构（用于sendmsg/recvmsg）：

```c
struct msghdr {
    void *msg_name;             // 可选的地址
    socklen_t msg_namelen;      // 地址的大小
    struct iovec *msg_iov;      // 分散/聚集数组
    int msg_iovlen;             // msg_iov中的元素数
    void *msg_control;          // 辅助数据
    socklen_t msg_controllen;   // 辅助数据缓冲区长度
    int msg_flags;              // 接收消息的标志
};
```

## 常用宏定义

### 地址族（Address Family）

```c
#define AF_INET     2       // IPv4
#define AF_INET6    10      // IPv6
#define AF_UNIX     1       // 本地通信（Unix域套接字）
#define AF_UNSPEC   0       // 未指定
```

### 套接字类型（Socket Type）

```c
#define SOCK_STREAM     1   // 流式套接字（TCP）
#define SOCK_DGRAM      2   // 数据报套接字（UDP）
#define SOCK_RAW        3   // 原始套接字
```

### 协议类型（Protocol）

```c
#define IPPROTO_IP      0   // IP协议
#define IPPROTO_TCP     6   // TCP协议
#define IPPROTO_UDP     17  // UDP协议
```

### shutdown参数

```c
#define SHUT_RD     0       // 关闭读
#define SHUT_WR     1       // 关闭写
#define SHUT_RDWR   2       // 关闭读写
```

### send/recv标志

```c
#define MSG_PEEK        0x01    // 预览数据，不移除
#define MSG_DONTWAIT    0x08    // 非阻塞操作
#define MSG_MORE        0x10    // 发送者还有更多数据要发送
```

### 套接字选项层级

```c
#define SOL_SOCKET      0xfff   // 套接字层选项
#define IPPROTO_TCP     6       // TCP层选项
#define IPPROTO_IP      0       // IP层选项
```

### 常用套接字选项

```c
#define SO_REUSEADDR    0x0004  // 允许重用本地地址
#define SO_KEEPALIVE    0x0008  // 保持连接活跃
#define SO_BROADCAST    0x0020  // 允许发送广播
#define SO_RCVBUF       0x1002  // 接收缓冲区大小
#define SO_SNDBUF       0x1001  // 发送缓冲区大小
#define SO_RCVTIMEO     0x1006  // 接收超时
#define SO_SNDTIMEO     0x1005  // 发送超时
#define TCP_NODELAY     0x01    // 禁用Nagle算法
```

## 典型使用场景

### TCP服务器流程

```c
// 1. 创建套接字
int server_sock = socket(AF_INET, SOCK_STREAM, 0);

// 2. 绑定地址和端口
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
server_addr.sin_addr.s_addr = INADDR_ANY;
bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

// 3. 监听
listen(server_sock, 5);

// 4. 接受连接
struct sockaddr_in client_addr;
socklen_t addr_len = sizeof(client_addr);
int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);

// 5. 接收/发送数据
recv(client_sock, buffer, size, 0);
send(client_sock, data, len, 0);

// 6. 关闭套接字
closesocket(client_sock);
closesocket(server_sock);
```

### TCP客户端流程

```c
// 1. 创建套接字
int client_sock = socket(AF_INET, SOCK_STREAM, 0);

// 2. 连接服务器
struct sockaddr_in server_addr;
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "192.168.1.100", &server_addr.sin_addr);
connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

// 3. 发送/接收数据
send(client_sock, data, len, 0);
recv(client_sock, buffer, size, 0);

// 4. 关闭套接字
closesocket(client_sock);
```

### UDP通信流程

```c
// 创建UDP套接字
int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);

// 绑定地址（可选，服务器需要）
struct sockaddr_in local_addr;
local_addr.sin_family = AF_INET;
local_addr.sin_port = htons(8080);
local_addr.sin_addr.s_addr = INADDR_ANY;
bind(udp_sock, (struct sockaddr*)&local_addr, sizeof(local_addr));

// 发送数据
struct sockaddr_in dest_addr;
dest_addr.sin_family = AF_INET;
dest_addr.sin_port = htons(9090);
inet_pton(AF_INET, "192.168.1.200", &dest_addr.sin_addr);
sendto(udp_sock, data, len, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));

// 接收数据
struct sockaddr_in from_addr;
socklen_t from_len = sizeof(from_addr);
recvfrom(udp_sock, buffer, size, 0, (struct sockaddr*)&from_addr, &from_len);

// 关闭套接字
closesocket(udp_sock);
```

### 使用getaddrinfo解析主机名

```c
// 设置查询条件
struct addrinfo hints;
memset(&hints, 0, sizeof(hints));
hints.ai_family = AF_UNSPEC;    // IPv4或IPv6
hints.ai_socktype = SOCK_STREAM; // TCP

// 解析主机名
struct addrinfo *result;
int ret = getaddrinfo("www.example.com", "80", &hints, &result);
if (ret != 0) {
    printf("getaddrinfo error: %s\n", gai_strerror(ret));
    return;
}

// 遍历结果并尝试连接
for (struct addrinfo *p = result; p != NULL; p = p->ai_next) {
    int sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
        // 连接成功
        break;
    }
    closesocket(sock);
}

// 释放内存
freeaddrinfo(result);
```

## 1. Socket创建和配置函数

### socket
**函数原型：**
```c
int socket(int domain, int type, int protocol);
```
**参数：**
domain：协议族（AF_INET-IPv4, AF_INET6-IPv6, AF_UNIX-本地通信）
type：套接字类型（SOCK_STREAM-TCP, SOCK_DGRAM-UDP, SOCK_RAW-原始套接字）
protocol：协议类型（通常为0，表示使用默认协议）
**返回值：**
成功：返回套接字文件描述符（非负整数）
失败：返回-1，errno被设置
**作用：**创建一个网络通信的端点（套接字）

### bind
**函数原型：**
```c
int bind(int s, const struct sockaddr *name, socklen_t namelen);
```
**参数：**
s：套接字文件描述符
name：指向sockaddr结构的指针，包含要绑定的地址信息
namelen：地址结构的长度
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**将套接字绑定到指定的本地地址和端口

### listen
**函数原型：**
```c
int listen(int s, int backlog);
```
**参数：**
s：套接字文件描述符
backlog：等待连接队列的最大长度
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**将套接字设置为监听状态，等待客户端连接（仅用于TCP服务器）

### accept
**函数原型：**
```c
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
```
**参数：**
s：监听套接字文件描述符
addr：指向sockaddr结构的指针，用于存储客户端地址信息（可为NULL）
addrlen：输入时为addr指向缓冲区的大小，输出时为实际地址长度
**返回值：**
成功：返回新的套接字文件描述符（用于与客户端通信）
失败：返回-1，errno被设置
**作用：**接受客户端连接请求，创建新的套接字用于数据传输（仅用于TCP服务器）

### connect
**函数原型：**
```c
int connect(int s, const struct sockaddr *name, socklen_t namelen);
```
**参数：**
s：套接字文件描述符
name：指向sockaddr结构的指针，包含服务器地址信息
namelen：地址结构的长度
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**连接到远程服务器（用于TCP客户端或连接型UDP）

### shutdown
**函数原型：**
```c
int shutdown(int s, int how);
```
**参数：**
s：套接字文件描述符
how：关闭方式（SHUT_RD-关闭读, SHUT_WR-关闭写, SHUT_RDWR-关闭读写）
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**关闭套接字的读、写或读写功能

### closesocket
**函数原型：**
```c
int closesocket(int s);
```
**参数：**
s：套接字文件描述符
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**关闭套接字，释放相关资源

### socketpair
**函数原型：**
```c
int socketpair(int domain, int type, int protocol, int sv[2]);
```
**参数：**
domain：通信域（例如PF_LOCAL）
type：套接字类型（例如SOCK_STREAM）
protocol：使用的协议（通常为0）
sv：两个整数的数组，用于存储创建的套接字的文件描述符
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**创建一对相互连接的套接字（用于进程间通信）

## 2. Socket数据发送函数

### send
**函数原型：**
```c
ssize_t send(int s, const void *dataptr, size_t size, int flags);
```
**参数：**
s：套接字文件描述符
dataptr：指向要发送数据的指针
size：要发送的数据长度（字节）
flags：发送标志（通常为0，MSG_DONTWAIT-非阻塞，MSG_MORE-还有更多数据）
**返回值：**
成功：返回实际发送的字节数
失败：返回-1，errno被设置
**作用：**通过已连接的套接字发送数据（用于TCP或已连接的UDP）

### sendto
**函数原型：**
```c
ssize_t sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t to_len);
```
**参数：**
s：套接字文件描述符
dataptr：指向要发送数据的指针
size：要发送的数据长度（字节）
flags：发送标志（通常为0）
to：指向目标地址的sockaddr结构指针
to_len：目标地址结构的长度
**返回值：**
成功：返回实际发送的字节数
失败：返回-1，errno被设置
**作用：**通过套接字向指定地址发送数据（主要用于UDP）

### sendmsg
**函数原型：**
```c
ssize_t sendmsg(int s, const struct msghdr *message, int flags);
```
**参数：**
s：套接字文件描述符
message：指向msghdr结构的指针，包含要发送的数据和控制信息
flags：发送标志（通常为0）
**返回值：**
成功：返回实际发送的字节数
失败：返回-1，errno被设置
**作用：**通过套接字发送消息，支持发送多个数据块和控制信息

## 3. Socket数据接收函数

### recv
**函数原型：**
```c
ssize_t recv(int s, void *mem, size_t len, int flags);
```
**参数：**
s：套接字文件描述符
mem：指向接收缓冲区的指针
len：接收缓冲区的大小（字节）
flags：接收标志（通常为0，MSG_DONTWAIT-非阻塞，MSG_PEEK-预览数据不移除）
**返回值：**
成功：返回实际接收的字节数（0表示连接关闭）
失败：返回-1，errno被设置
**作用：**从已连接的套接字接收数据（用于TCP或已连接的UDP）

### recvfrom
**函数原型：**
```c
ssize_t recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
```
**参数：**
s：套接字文件描述符
mem：指向接收缓冲区的指针
len：接收缓冲区的大小（字节）
flags：接收标志（通常为0）
from：指向sockaddr结构的指针，用于存储发送方地址信息（可为NULL）
fromlen：输入时为from指向缓冲区的大小，输出时为实际地址长度
**返回值：**
成功：返回实际接收的字节数
失败：返回-1，errno被设置
**作用：**从套接字接收数据并获取发送方地址（主要用于UDP）

### recvmsg
**函数原型：**
```c
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```
**参数：**
sockfd：套接字文件描述符
msg：指向msghdr结构的指针，用于接收数据和控制信息
flags：接收标志（通常为0）
**返回值：**
成功：返回实际接收的字节数
失败：返回-1，errno被设置
**作用：**从套接字接收消息，支持接收多个数据块和控制信息

## 4. Socket选项配置函数

### setsockopt
**函数原型：**
```c
int setsockopt(int s, int level, int optname, const void *opval, socklen_t optlen);
```
**参数：**
s：套接字文件描述符
level：选项所属协议层（SOL_SOCKET-套接字层，IPPROTO_TCP-TCP层，IPPROTO_IP-IP层）
optname：选项名称（SO_REUSEADDR-地址重用，SO_KEEPALIVE-保活，TCP_NODELAY-禁用Nagle算法等）
opval：指向选项值的指针
optlen：选项值的长度
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**设置套接字选项，控制套接字行为

### getsockopt
**函数原型：**
```c
int getsockopt(int s, int level, int optname, void *opval, socklen_t *optlen);
```
**参数：**
s：套接字文件描述符
level：选项所属协议层
optname：选项名称
opval：指向用于存储选项值的缓冲区指针
optlen：输入时为缓冲区大小，输出时为实际选项值长度
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**获取套接字选项的当前值

## 5. Socket信息查询函数

### getsockname
**函数原型：**
```c
int getsockname(int s, struct sockaddr *name, socklen_t *namelen);
```
**参数：**
s：套接字文件描述符
name：指向sockaddr结构的指针，用于存储本地地址信息
namelen：输入时为name指向缓冲区的大小，输出时为实际地址长度
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**获取套接字绑定的本地地址和端口

### getpeername
**函数原型：**
```c
int getpeername(int s, struct sockaddr *name, socklen_t *namelen);
```
**参数：**
s：套接字文件描述符
name：指向sockaddr结构的指针，用于存储对端地址信息
namelen：输入时为name指向缓冲区的大小，输出时为实际地址长度
**返回值：**
成功：返回0
失败：返回-1，errno被设置
**作用：**获取连接到套接字的对端地址和端口

## 6. IP地址转换函数

### inet_ntop
**函数原型：**
```c
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```
**参数：**
af：地址族（AF_INET或AF_INET6）
src：指向网络字节序IP地址的指针
dst：指向用于存储字符串形式IP地址的缓冲区
size：缓冲区大小
**返回值：**
成功：返回指向dst的指针
失败：返回NULL，errno被设置
**作用：**将网络字节序的IP地址转换为点分十进制字符串（IPv4）或冒号分隔字符串（IPv6）

### inet_pton
**函数原型：**
```c
int inet_pton(int af, const char *src, void *dst);
```
**参数：**
af：地址族（AF_INET或AF_INET6）
src：指向字符串形式IP地址的指针
dst：指向用于存储网络字节序IP地址的缓冲区
**返回值：**
1：转换成功
0：输入格式无效
-1：地址族不支持，errno被设置
**作用：**将点分十进制字符串（IPv4）或冒号分隔字符串（IPv6）转换为网络字节序的IP地址

## 7. 主机名解析函数

### gethostbyname
**函数原型：**
```c
struct hostent *gethostbyname(const char *name);
```
**参数：**
name：主机名或域名字符串
**返回值：**
成功：返回指向hostent结构的指针，包含主机信息
失败：返回NULL，h_errno被设置
**作用：**通过主机名获取主机的IP地址信息（仅支持IPv4，已过时，建议使用getaddrinfo）

### gethostbyname_r
**函数原型：**
```c
int gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop);
```
**参数：**
name：主机名或域名字符串
ret：指向hostent结构的指针，用于存储结果
buf：用户提供的缓冲区
buflen：缓冲区大小
result：指向hostent指针的指针，成功时指向ret，失败时为NULL
h_errnop：指向错误码的指针
**返回值：**
成功：返回0
失败：返回错误码
**作用：**gethostbyname的线程安全版本

### getaddrinfo
**函数原型：**
```c
int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res);
```
**参数：**
nodename：主机名或IP地址字符串（可为NULL）
servname：服务名或端口号字符串（可为NULL）
hints：指向addrinfo结构的指针，指定查询条件（可为NULL）
res：指向addrinfo指针的指针，用于返回结果链表
**返回值：**
成功：返回0
失败：返回非零错误码
**作用：**根据主机名和服务名获取地址信息（支持IPv4和IPv6，推荐使用）

### freeaddrinfo
**函数原型：**
```c
void freeaddrinfo(struct addrinfo *ai);
```
**参数：**
ai：指向getaddrinfo返回的addrinfo链表的指针
**返回值：**
无
**作用：**释放getaddrinfo分配的内存

### gai_strerror
**函数原型：**
```c
const char *gai_strerror(int ecode);
```
**参数：**
ecode：getaddrinfo返回的错误码
**返回值：**
指向描述错误的字符串的指针
**作用：**返回描述getaddrinfo错误码的字符串

