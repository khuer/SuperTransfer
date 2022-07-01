#include "log.h"
#include "network.h"
#include <iostream>
#include <stdio.h>
#include <time.h>

using namespace std;

void parseMsg(string msg)
{
    // auto findCh = [](string &str, char ch, int startPos) -> int
    // {
    //     for (int i = startPos; i < str.size();i++)
    //     {
    //         if(str[i] == ch)
    //         {
    //             return i;
    //         }
    //     }
    //     return -1;
    // };
    int namePos = msg.find("Name:", 0);
    // int end = findCh(msg, ' ', namePos);
    int sizePos = msg.find("Size:", namePos + 5);

    cout << msg.substr(namePos + 5, sizePos-(namePos + 5)) << endl;
    cout << msg.substr(sizePos+5,msg.size()-(sizePos+5)) << endl;
}

void readFile(SOCKET sock)
{
    char *buf = new char[MAXBUFFER];
    int len = recv(sock, buf, 2048, 0);
    if(len == SOCKET_ERROR) {
        std::cout << "a client disconnected! " << std::endl;
        closesocket(sock);
        return;
    }
    string msg(buf);
    
    int namePos = msg.find("Name:", 0);
    int sizePos = msg.find("Size:", namePos + 5);

    // cout << msg.substr(namePos + 5, sizePos-(namePos + 5)) << endl;
    // cout << msg.substr(sizePos+5,msg.size()-(sizePos+5)) << endl;
    string fileName = "media/" + msg.substr(namePos + 5, sizePos - (namePos + 5));
    size_t totalSize = atoi(msg.substr(sizePos + 5, msg.size() - (sizePos + 5)).c_str());
    FILE *f;
    f = fopen(fileName.c_str(), "wb+");

    time_t startSec;
    time_t endSec;
    startSec = time(NULL);
    long num = 0;
    while(1) {
        int len = recv(sock, buf, MAXBUFFER, 0);
        if(len == SOCKET_ERROR) {
            std::cout << "a client disconnected! " << std::endl;
            closesocket(sock);
            endSec = time(NULL);
            cout << "total " << endSec - startSec << " s" << endl;
            size_t mSize = totalSize / 1024 / 1024;
            
            if(endSec - startSec == 0)
                cout << "speed: " << mSize  << " MB/s"<<endl;
            else{
                double speed;
                speed = mSize / (endSec - startSec);
                cout << "speed: " << speed  << " MB/s"<<endl;
            }

            fclose(f);
            return;
        }else{
            fwrite(buf, 1, len, f);
        }
    }
}

int main()
{
    std::cout << "Hello world" << std::endl;
    NetworkServer net;
    net.initialize("192.168.123.240", 1235);
    net.startListen(NetworkServer::Protocol::HTTP);
    char ch;
    while(true){
        cin >> ch;
        if(ch=='n'){
            net.shutDown();
            break;
        }
    }
}
