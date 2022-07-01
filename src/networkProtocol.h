#ifndef _NETWORK_PROTOCOL_H
#define _NETWORK_PROTOCOL_H

#include <WinSock2.h>
#include <thread>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include "log.h"
#pragma comment(lib,"ws2_32.lib")

class NetworkProtocol
{
public:
    virtual void serveClient(SOCKET sock) = 0;
};

class HttpProtocol : public NetworkProtocol
{
public:
    void serveClient(SOCKET sock) override;

private:
    void processRequest(SOCKET clientFd, std::vector<std::string> &headerPart,
                        std::vector<std::string> &bodys);
    void processGet(SOCKET clientFd, std::string &path,
                    std::vector<std::string> &bodys);
    int sendFile(SOCKET clientFd, std::string &path);
    int get_line(SOCKET sock, char *buf, int size);
    void split(std::string str,char c,std::vector<std::string> &);
    void unimplemented(SOCKET client);
    void handleMethod(std::string path,SOCKET sock);
    void headers(SOCKET client, const char *filename,std::string format);
    void showFilesHtml(SOCKET client, std::string path,
                       std::vector<std::string> fileNames);
    void fileShow(SOCKET client);
};

struct header_t{
    SOCKET mSock;
    header_t(SOCKET sock) : mSock(sock) {}
    header_t() = delete;
    int sendOk(){
        if(send(mSock, "HTTP/1.0 200 OK\r\n", strlen("HTTP/1.0 200 OK\r\n"), 0) == -1){
            MYLOGE("send error!");
            return -1;
        }
        // if(send(mSock, "Server: jdbhttpd/0.1.0\r\n",
        //                 strlen("Server: jdbhttpd/0.1.0\r\n"), 0) == -1){
        //     MYLOGE("send error!");
        //     return -1;
        // }
        return 1;
    }
    int over(){
        if(send(mSock, "\r\n", strlen("\r\n"), 0) == -1){
            MYLOGE("send error!");
            return -1;
        }
        return 1;
    }
    int sendParam(char *msg){
        if(send(mSock, msg, strlen(msg), 0) == -1){
            MYLOGE("send error!");
            return -1;
        }
        return 1;
    }
    int sendContentLength(unsigned long length){
        char msg[30];
        sprintf(msg,"Content-Length: %d\r\n", length);
        if(send(mSock, msg, strlen(msg), 0) == -1){
            MYLOGE("send error!");
            return -1;
        }
        return 1;
    }
    int sendContentType(const char *type){
        char msg[30];
        sprintf(msg,"Content-Type: %s\r\n", type);
        if(send(mSock, msg, strlen(msg), 0) == -1){
            MYLOGE("send error!");
            return -1;
        }
        return 1;
    }
};

#endif
