#include <iostream>
#include <stdio.h> // to include fgets/fputs
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <map>

using namespace std;

#define PORT 3000
#define BUFFERSIZE 1024
#define LISTENMQ 10

const static char *SOURCE_PATH="resource";

int get_line(int sock, char *buf, int size);
void split(string str,char c,vector<string> &);
void unimplemented(int client);
void handleMethod(string path,int sock);
void headers(int client, const char *filename,string format);
void showFilesHtml(int client,string path,vector<string> fileNames);


void findFiles(const char *root,vector<string> &files)
{
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(root)) == nullptr) {
        perror("opendir");
        return;
    }

    while((ent = readdir(dir)) != nullptr) {
        if(!(strcmp(ent->d_name,".")) || !(strcmp(ent->d_name,"..")))
            continue;
        files.push_back(ent->d_name);
    }
}

void serve(int clientFd)
{
    char buf[256]={0};
    get_line(clientFd,buf,sizeof(buf));
    string msg(buf);
    vector<string> msgParts;
    split(msg,' ',msgParts);
    for(auto i:msgParts) {
        cout<< i<<"  ,  ";
    }
    cout<<endl;

    if(msgParts[0] != "GET" && msgParts[0] != "POST"){
        unimplemented(clientFd);
        close(clientFd);
        return;
    }

    string path="."+msgParts[1];
    if(path[path.size()-1] == '/') {
        path+="index.html";
    }

    int numchars=1;
    buf[0] = 'A';
    buf[1] = '\0';
    while ((numchars > 0) && strcmp("\n", buf)) /* read & discard headers */
        numchars = get_line(clientFd, buf, sizeof(buf));

    handleMethod(path,clientFd);

    cout<<"handle over"<<endl;

    close(clientFd);
}

void handleMethod(string path,int sock)
{
    cout<<"PATH "<<path<<endl;
    int pos=path.find('.',1);
    string format;
    if(pos==string::npos){
        cout<<"error url"<<endl;
        return;
    }
    format=path.substr(pos+1,path.size()-pos-1);
    cout<<format<<endl;
    headers(sock,path.c_str(),format);

    if(path == "./files_show.html"){
        vector<string> files;
        findFiles(SOURCE_PATH,files);
        showFilesHtml(sock,SOURCE_PATH,files);
    } else{
        cout << "prepare to send data"<<endl;
        char buf[1024];
        int fd=open(path.c_str(),O_RDWR);
        cout << "111"<<endl;
        if(fd==-1){
            perror("open");
            return;
        }

        int len=0;
        while(len=read(fd,buf,sizeof(buf))){
            if(len < 0){
                cout << "222"<<endl;
                perror("read");
                close(fd);
                return;
            }
            if(send(sock,buf,len,0) == -1) {
                cout << "333"<<endl;
                perror("send");
                close(fd);
                return;
            }
        }
        close(fd);
        
    }
    cout<<"send compelete: "<<path<<endl;
}

void split(string str,char c,vector<string> &ret)
{
    int start=0;
    bool findBody=false;
    int i=0;
    for(int i=0;i<str.size();i++){
        if (!findBody){
            if(str[i] != c){
                findBody=true;
                start=i;
            }
        } else{
            if (str[i] == c){
                findBody = false;
                ret.push_back(str.substr(start,i-start));
            }
        }
    }
    if(str[str.size()-1] != c){
        ret.push_back(str.substr(start,i-start));
    }
}

int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';

    return (i);
}

void unimplemented(int client)
{
    char buf[1024];

    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

void showFilesHtml(int client,string path,vector<string> fileNames)
{
    char buf[1024];

    sprintf(buf, "<HTML><BODY><h1>文件列表\r\n");
    send(client, buf, strlen(buf), 0);

    for(auto filename:fileNames) {
        sprintf(buf, "<div><a href=\"%s\"> %s\r\n",
                (path+"/"+filename).c_str(),filename.c_str());
        send(client, buf, strlen(buf), 0);
    }

    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

void headers(int client, const char *filename,string format)
{
    char buf[1024];
    (void)filename; /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    if(send(client, buf, strlen(buf), 0) == -1){
        perror("send");
        return;
    }

    strcpy(buf, "Server: jdbhttpd/0.1.0\r\n");
    if(send(client, buf, strlen(buf), 0) == -1){
        perror("send");
        return;
    }

    if(format == "html"){
        sprintf(buf, "Content-Type: text/html; charset=UTF-8\r\n");
        if(send(client, buf, strlen(buf), 0) == -1){
            perror("send");
            return;
        }
    }
    // else if(format == "png"){
    else {
        struct stat st;
        stat(filename,&st);
        sprintf(buf, "Content-Length: %s\r\n",to_string(st.st_size).c_str());
        if(send(client, buf, strlen(buf), 0) == -1){
            perror("send");
            return;
        }
    
        if(format == "png"){
            sprintf(buf, "Content-Type: img/png\r\n");
            if(send(client, buf, strlen(buf), 0) == -1){
                perror("send");
                return;
            }
        }else if(format == "png"){
            sprintf(buf, "Content-Type: video/mpeg4\r\n");
            if(send(client, buf, strlen(buf), 0) == -1){
                perror("send");
                return;
            }
        }
        
    }
    
    strcpy(buf, "\r\n");
    if(send(client, buf, strlen(buf), 0) == -1){
        perror("send");
        return;
    }
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
    int port=1234;
    if(argv==2){
        port=atoi(argc[1]);
    }


    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    // server PORT
    serverAddr.sin_port = htons(port);
    // server IP
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);       //inet_addr(INADDR_ANY);

    if(bind(server, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind");
        exit(1);
    } else {
        cout<<"bind port "<< port << endl;
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