#include"../header/Net.h"
#include"../header/userType.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <errno.h>

#define MAX_BUFFER_SIZE 1024

CNet::CNet(std::string ip, int port)
{
    this->ip = ip;
    this->port = port;
    this->msg = (char*)malloc(1024*sizeof(char)*10);
    this->msgLength = 0;
}

CNet::~CNet()
{
}

void CNet::cRecv() {

    int sockfd;
    struct sockaddr_in addr;
    char buffer[MAX_BUFFER_SIZE];

    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(1);
    }

    // 设置套接字属性，允许接收组播数据
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
        perror("Failed to set socket options");
        exit(1);
    }

    // 绑定套接字到指定的端口
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(this->port);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Failed to bind socket");
        exit(1);
    }

    // 加入组播组
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(this->ip.c_str());
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, 12) < 0) {
        perror("Failed to join multicast group");
        printf("%d %s",errno,strerror(errno));
        exit(1);
    }

    // 接收组播数据
    struct sockaddr_in sender_addr;
    socklen_t sender_addr_len;
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        sender_addr_len = sizeof(sender_addr);
        ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&sender_addr, &sender_addr_len);
        if (recv_len < 0) {
            perror("Failed to receive data");
            exit(1);
        }

        // // 获取发送者的地址信息
        // char sender_ip[INET_ADDRSTRLEN];
        // inet_ntop(AF_INET, &(sender_addr.sin_addr), sender_ip, INET_ADDRSTRLEN);

        recvMsg *tempMsg = (recvMsg*)buffer;
        for(int i = 0 ; i < tempMsg->len ; i++) {
            this->msg[this->msgLength] = tempMsg->msg[i];
            this->msgLength ++;
        }
        
        // 处理接收到的数据
        printf("Received :%d: %s\n", ntohs(sender_addr.sin_port), buffer);
    }

    // 关闭套接字
    close(sockfd);
}