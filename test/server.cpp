#include <sys/socket.h>
#include <iostream>
#include <stdio.h> // to include fgets/fputs
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h> // to include struct sockaddr_in
#include <unistd.h> // to include socket close method
#include <sys/types.h> // to include pid_t
#include <thread>

using namespace std;

#define PORT 3000
#define BUFFERSIZE 1024
#define LISTENMQ 10

const char WORDS[] = 
{
    "beautiful,adj.美丽的;美好的;很好的;出色的;巧妙的/affinity,n.密切关系;喜好;喜爱;密切的关系;类同;"
};

void serve(int clientFd)
{
    char buf[20]={0};
    while(true){
        int length = recv(clientFd, buf, sizeof(buf), 0);
        if(length <= 0)
        {
            break;
        }
        cout << "recive: " << buf << endl;
        if(strcmp(buf,"word")==0){
            send(clientFd, WORDS, sizeof(WORDS)+1, 0);
        }
        
        memset(buf, 0, length);
    }

    close(clientFd);
}

void waitAndProcessRequest(int listendFd)
{
    struct sockaddr_in clientAddr;
    int conn;
    socklen_t clientLen = sizeof(clientAddr);
    while(true){
        if((conn = accept(listendFd, (struct sockaddr *)&clientAddr, &clientLen)) < 0)
        {
            printf("Error while listening.\n");
            return;
        }
        cout << "---New client connected---" << endl;
        serve(conn);
        cout << "---client log out---" << endl;
    }
}

int main(int argv, char** argc)
{
    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    // server PORT
    serverAddr.sin_port = htons(PORT);
    // server IP
    serverAddr.sin_addr.s_addr = inet_addr("192.168.123.240");

    if(bind(server, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("Error while binding IP to server.\n");
        exit(1);
    }

    if(listen(server, LISTENMQ) == -1)
    {
        printf("Error while listening.\n");
        exit(1);
    }

    thread processThread = thread([server]()
                                  { waitAndProcessRequest(server); });

    char str[20] = {0};
    while (true)
    {
        cin >> str;
        if (strcmp("exit", str) == 0)
        {
            close(server);

            exit(0);
        }
        memset(str, 0, 20);
    }

    cout << "exit...." << endl;
    processThread.join();

    return 0;

}