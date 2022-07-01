#ifndef _NETWORK_H
#define _NETWORK_H

#include <WinSock2.h>
#include <thread>
#include "networkProtocol.h"
#include <memory>
#include <mutex>
#include <condition_variable>
#pragma comment(lib,"ws2_32.lib")

// for use __func__ to print function name when occur error
// define a func
#define READSOCK(sock,buf,len,flag,ret)                     \
    do{                                                     \
        ret = recv((sock), (buf), (len), (flag));           \
        if(ret<=0){                                         \
            MYLOGE("clent disconnected, read failed!");     \
        }                                                   \
    }while(0)                                               \

const uint32_t MAXBUFFER = 1024*1024*10;

class Network
{
public:
    Network();
    ~Network() { WSACleanup(); }
    void disConnect()
    {
        closesocket(mSock);
    }

protected:
    WSADATA mWsaData;
    SOCKET mSock;
};

class NetworkClient: public Network
{
public:
    //NetworkClient();
    int initialize();
    int connectServer(const char *ip, int16_t port);
    void sendMessage(const char *msg, size_t size);
};

class NetworkServer: public Network
{
public:
    ~NetworkServer()
    {
        if(listenTread.joinable()){
            isExit = true;
            listenTread.join();
        }
    }

    enum Protocol
    {
        TCP = 0,
        HTTP = 1,
    };
    
    int initialize(const char *ip, int16_t port);
    void startListen(Protocol protocal)
    {
        switch(protocal){
            case Protocol::TCP:
                break;
            case Protocol::HTTP:
                mProtocal = std::make_shared<HttpProtocol>();
                break;
        }
        
        goExit = false;
        isExit = false;
        listenTread = std::thread([this]()
                              { listenLoop(); });
    }
    void shutDown()
    {
        goExit = true;
        Sleep(2000);
        if(!isExit){
            NetworkClient client;
            client.initialize();
            client.connectServer(mIp.c_str(), mPort);
            client.disConnect();
        }

        listenTread.join();
    }

private:
    void listenLoop(int maxListen=5);

    std::string mIp;
    int mPort;
    std::shared_ptr<NetworkProtocol> mProtocal;
    bool isExit;
    bool goExit;
    std::thread listenTread;
    std::mutex mShutMutex;
    std::condition_variable mShutCond;
};

#endif
