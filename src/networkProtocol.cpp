#include "networkProtocol.h"
#include "network.h"

using namespace std;

const static char *SOURCE_PATH="resource";

void HttpProtocol::serveClient(SOCKET clientFd)
{
    vector<string> headerParts;
    vector<string> bodys;
    char buf[256]={0};
    if(get_line(clientFd,buf,sizeof(buf)) == -1){
        MYLOGE("invalid socket!");
        return;
    }
    string msg(buf);

    while(true) {
        int len = get_line(clientFd, buf, sizeof(buf));
        if(len==0){
            break;
        }
        bodys.push_back(buf);
        MYLOGI("body:  %s", buf);
        if(strcmp("\n", buf)){
            break;
        }
    }
    
    split(msg,' ',headerParts);
    MYLOGI("method:%s  uri:%s  ", headerParts[0].c_str(), headerParts[1].c_str());

    processRequest(clientFd, headerParts, bodys);

    // if(headerParts[0] != "GET" && headerParts[0] != "POST"){
    //     unimplemented(clientFd);
    //     closesocket(clientFd);
    //     return;
    // }

    // string path="."+headerParts[1];
    // if(path[path.size()-1] == '/') {
    //     path+="index.html";
    // }
}

void HttpProtocol::processRequest(SOCKET clientFd, vector<string> &headerPart,vector<string> &bodys)
{
    string &method = headerPart[0];
    string &path = headerPart[1];
    if(method=="GET"){
        MYLOGI("Method:GET");
        processGet(clientFd, path, bodys);
    }else if(method=="POST"){
        MYLOGI("Method:POST");
        unimplemented(clientFd);
    }else{
        unimplemented(clientFd);
    }
}

void HttpProtocol::processGet(SOCKET clientFd, string &path,vector<string> &bodys)
{
    bool virtualFile = false;
    header_t header(clientFd);
    header.sendOk();

    if(path=="/"){
        path = ".." + path;
        path += "index.html";
    } else if(path == "/files_show.html"){
        virtualFile = true;
    }

    string fileFormat;
    int pos=path.find('.',1);
    if(pos==string::npos){
        cout<<"error url"<<endl;
        return;
    }
    fileFormat=path.substr(pos+1,path.size()-pos-1);
    const char *format = "text/html";
    if(fileFormat == "png"){
        format = "img/png";
    }
    header.sendContentType(format);
    header.over();

    if(virtualFile)
        fileShow(clientFd);
    else
        sendFile(clientFd, path);
    MYLOGI("send file compeleted: %s", path.c_str());
}

int HttpProtocol::sendFile(SOCKET clientFd, string &path)
{
    char *buf = new char[1024];
    FILE *f=nullptr;
    f = fopen(path.c_str(), "rb");
    if(f==nullptr){
        MYLOGE("no such file: %s", path.c_str());
        return -1;
    }
    int len = 0;
    while(len=fread(buf, 1, 1024, f)){
        if(send(clientFd, buf, len, 0) == -1){
            MYLOGE("send error!");
            return -1;
        }
    }
    fclose(f);
    return 1;
}

int HttpProtocol::get_line(SOCKET sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    while ((i < size - 1) && (c != '\n'))
    {
        READSOCK(sock, &c, 1, 0, n);

        if (n > 0)
        {
            if (c == '\r')
            {
                READSOCK(sock, &c, 1, MSG_PEEK, n);

                if ((n > 0) && (c == '\n'))
                    READSOCK(sock, &c, 1, 0, n);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }else if(n==-1){
            return n;
        }else
            c = '\n';
    }
    buf[i] = '\0';

    return (i);
}

void HttpProtocol::split(string str,char c,vector<string> &ret)
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

void HttpProtocol::fileShow(SOCKET client)
{
    char buf[1024];

    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(client, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(client, buf, strlen(buf), 0);
}

void HttpProtocol::unimplemented(SOCKET client)
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

void HttpProtocol::showFilesHtml(SOCKET client,string path,vector<string> fileNames)
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

void HttpProtocol::headers(SOCKET client, const char *filename,string format)
{
    char buf[1024];
    (void)filename; /* could use filename to determine file type */

    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    if(send(client, buf, strlen(buf), 0) == -1){
        MYLOGE("send error!");
        return;
    }

    strcpy(buf, "Server: jdbhttpd/0.1.0\r\n");
    if(send(client, buf, strlen(buf), 0) == -1){
        MYLOGE("send error!");
        return;
    }

    if(format == "html"){
        sprintf(buf, "Content-Type: text/html; charset=UTF-8\r\n");
        if(send(client, buf, strlen(buf), 0) == -1){
            MYLOGE("send error!");
            return;
        }
    }
    // else if(format == "png"){
    else {
        struct stat st;
        stat(filename,&st);
        sprintf(buf, "Content-Length: %s\r\n",to_string(st.st_size).c_str());
        if(send(client, buf, strlen(buf), 0) == -1){
            MYLOGE("send error!");
            return;
        }
    
        if(format == "png"){
            sprintf(buf, "Content-Type: img/png\r\n");
            if(send(client, buf, strlen(buf), 0) == -1){
                MYLOGE("send error!");
                return;
            }
        }else if(format == "png"){
            sprintf(buf, "Content-Type: video/mpeg4\r\n");
            if(send(client, buf, strlen(buf), 0) == -1){
                MYLOGE("send error!");
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
