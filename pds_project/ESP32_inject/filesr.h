
#ifndef FILESR_H
#define FILESR_H

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

#include "probe_req.h"

#define MAXBUFL 255

int file_send_recv(int sockfd, struct addrinfo *res, char filename[]);

addrinfo *get_socket(int &sock);

#endif