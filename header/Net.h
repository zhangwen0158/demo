#include<iostream>

class CNet
{
private:
    std::string ip;
    int port;
    char *msg;  //所有的msg里的信息存储下来
    int msgLength;

public:
    CNet(std::string ip, int port);
    ~CNet();
    void cRecv();
};


