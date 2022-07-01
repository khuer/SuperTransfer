// #include "log.h"
// #include "network.h"
// #include <stdio.h>
// #include <sys/stat.h>
// #include <string>
// #include <fstream>
// #include <iostream>

// using namespace std;

// void sendFile(const char *fileName,NetworkClient *client)
// {
//     struct stat s;
//     stat(fileName, &s);

//     char name[50] = {'\0'};
//     int index = 0;
//     for (int i = 0; fileName[i] != '\0';i++)
//     {
//         if(fileName[i] == '/') {
//             index = i+1;
//         }
//     }

//     for (int i = index; fileName[i] != '\0';i++)
//     {
//         name[i - index] = fileName[i];
//     }
//     string msg;
//     msg += "Name:";
//     msg += name;
//     msg += " Size:";
//     msg += to_string(static_cast<long>(s.st_size));
//     cout << msg << endl;

//     client->sendMessage(msg.c_str(), msg.size());

//     char *buf = new char[MAXBUFFER];

//     long packageSum = s.st_size / MAXBUFFER;
//     cout << "packageSum  " << packageSum << endl;

//     FILE *f;
//     f = fopen(fileName, "rb");
//     if(f==nullptr) {
//         cout << "open file error!" << endl;
//         exit(0);
//     }

//     int len = 0;
//     long times = 0;
//     while(1){
//         len = fread(buf, 1, MAXBUFFER, f);
//         if(len < 1){
//             cout << "over" << endl;
//             break;
//         };
//         //times++;
//         //cout << times*100 / packageSum << "     "<<len<<endl;

//         client->sendMessage(buf, len);
//     }
//     fclose(f);
//     delete[] buf;
// }

// int main(int argc,char **argv)
// {
//     NetworkClient net;
//     net.initialize();
//     net.connectServer("192.168.123.240", 1235);
//     if(argc <= 1){
//         exit(0);
//     }
//     sendFile(argv[1],&net);
//     cout << "quit" << endl;
// }