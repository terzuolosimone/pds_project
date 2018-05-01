
#ifndef SERVER_H
#define SERVER_H

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

#define SA struct sockaddr
#define LISTENQ 15
#define MAXBUFL 255

int server();

#endif
