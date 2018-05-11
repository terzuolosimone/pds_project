#include <iostream>
#include <set>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <tins/tins.h>
#include <thread>
#include <future>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/sendfile.h>
#include <signal.h>

using std::set;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    int n;
    char size[20];
    char sendBuff[1025];
    char recvBuff[10000];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(15100); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 
    listen(listenfd, 10); 

    char filename[30];
    sprintf(filename, "test.txt");
    std::ofstream outfile (filename);

    while(1)
    { 
        printf("Waiting for client...\n");
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        printf("Connection accepted from esp32!\n");
        memset(recvBuff, '0', sizeof(recvBuff));
        recvBuff[0] = '\0';
        snprintf(sendBuff, sizeof(sendBuff), "Saluti dal server!\nLa informo che anche oggi la madonna è un gran troione :)\n");
        n = 0;

        read(connfd, size, sizeof(size));
        printf("size of client buf = %d\n\n", atoi(size));

        int i; char c;

        for(i=0;i<atoi(size);i++){
            read(connfd, &c, sizeof(char));
            recvBuff[i] =  c;
        }
        recvBuff[i] = '\0';
        outfile << recvBuff;
        printf("\n");
        printf("Sending response...\n");
        write(connfd, sendBuff, strlen(sendBuff)); 
        close(connfd);
        printf("Closed connection with esp32!\n");
        sleep(1);
     }
}
