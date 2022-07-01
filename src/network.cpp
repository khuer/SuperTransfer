#include "network.h"
#include "log.h"

Network::Network()
{
    int ret = WSAStartup(MAKEWORD(2, 2), &mWsaData);
    if(ret) {
        MYLOGE("network startup error!");
        exit(0);
    }
}

int NetworkServer::initialize(const char *ip, int16_t port)
{
    mSock=socket(AF_INET,SOCK_STREAM,0);
    if (mSock == -1) {
        MYLOGE("create socket error!");
        return -2;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET; //地址为IPv4协议
    addr.sin_port = htons(port); //端口为port
    addr.sin_addr.S_un.S_addr = inet_addr(ip); //具体绑定本机的地址
    int ret = bind(mSock, (sockaddr *)&addr, sizeof(addr)); //绑定
    if (ret == -1) {
        MYLOGE("绑定地址端口失败");
        return -1;
    }

    mIp = std::string(ip);
    mPort = port;

    return 1;
}

void NetworkServer::listenLoop(int maxListen)
{
    int ret=listen(mSock,maxListen);
    if (ret == -1) {
        MYLOGE("监听套接字失败");
        return;
    }
    MYLOGI("listenLoop start");

    // fd_set readSet;
    // FD_ZERO(&readSet);
    // FD_SET(mSock, &readSet);
    while(1) {
        if(goExit) {
            // for (int i = 0; i < readSet.fd_count;i++){
            //     closesocket(readSet.fd_array[i]);
            // }
            MYLOGI("bye~");
            closesocket(mSock);
            isExit = true;
            break;
        }

        sockaddr addrCli;
        int len = sizeof(addrCli);
        SOCKET sockCli=accept(mSock,&addrCli,&len);
        MYLOGI("---------------new client connected---------------");
        mProtocal->serveClient(sockCli);
        closesocket(sockCli);
        MYLOGI("-------------------disconnectd--------------------\n");

        // fd_set readSetTemp = readSet;
        // timeval timeout;
        // timeout.tv_sec = 3;
        // timeout.tv_usec = 0;
        // int ret = select(0, &readSetTemp,nullptr,nullptr,&timeout);
        // if(ret > 0) {
        //     for (int i = 0; i < readSetTemp.fd_count; i++) {
        //         if (readSetTemp.fd_array[i] == mSock) {
        //             if(readSetTemp.fd_count < FD_SETSIZE) {
        //                 sockaddr addrCli;
        //                 int len = sizeof(addrCli);
        //                 SOCKET sockCli=accept(mSock,&addrCli,&len);
        //                 mCallback(sockCli);
        //                 // FD_SET(sockCli, &readSet);
        //                 MYLOGI("new client connected! ");
        //             }
        //         } 
        //         // else {
        //         //     mCallback(readSetTemp.fd_array[i]);
        //         //     FD_CLR(readSetTemp.fd_array[i], &readSet);
        //         // }
        //     }
        // }
    }
}

int NetworkClient::initialize()
{
    mSock=socket(AF_INET,SOCK_STREAM,0);
    if (mSock == -1) {
        MYLOGE("create socket error!");
        return -2;
    }
    return 0;
}

int NetworkClient::connectServer(const char *ip, int16_t port)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.S_un.S_addr = inet_addr(ip);
    int ret = connect(mSock, (sockaddr*)&addr, sizeof(addr));
    if (ret == -1) {
        MYLOGE("connect error!");
        return -1;
    }
    return 0;
}

void NetworkClient::sendMessage(const char *msg, size_t size)
{
    send(mSock, msg, size, 0);
}

