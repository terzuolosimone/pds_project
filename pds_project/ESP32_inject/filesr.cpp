
#include "filesr.h"

using std::set;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

addrinfo *get_socket(int &sock){
	int sockfd;
	struct addrinfo *res;
	int result;
	char buff[MAXBUFL];
	// Get port and server address from file config.txt
	char filename_config[] = "config.txt";
	FILE *fp = fopen(filename_config, "r");
	char port_in_file[20];
	char destaddress_in_file[20];
	char tmp[20];
	if(fp == NULL)
		return NULL;
	while(fscanf(fp, "%s", tmp) != EOF){
		if(strcmp(tmp, "PORT:")==0)
			fscanf(fp, "%s", port_in_file);	
		else if(strcmp(tmp, "SERVER_ADDRESS:")==0)
			fscanf(fp, "%s", destaddress_in_file);
	}
	fclose(fp);
	// Get address info
	if(getaddrinfo(destaddress_in_file, port_in_file, NULL, &res)!=0)
		return NULL;
	// Create socket
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockfd == -1)
		return NULL;
	result = connect(sockfd, res->ai_addr, res->ai_addrlen);
	if(result == -1)
		return NULL;
	sock = sockfd;
	return res;
}

int file_send_recv(int sockfd, struct addrinfo *res, char filename[]){
	/* Set timeval and select, so if the timers runs out
	 * because various reasons, everything stops (we have to think about that)
	 */
	/*struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	fd_set cset;
	FD_ZERO(&cset);
	FD_SET(sockfd, &cset);
	int n;
	if((n=select(FD_SETSIZE, &cset, NULL, NULL, &timeout))==-1)
		return -1;
	if(n>0){*/
		// Here we send the file
		int fp = open(filename, O_RDONLY);
		struct stat statistics;
		fstat(fp, &statistics);
		int size = statistics.st_size;
		uint32_t size_32 = htonl(size);
		sendto(sockfd, &size_32, sizeof(size_32), 0, res->ai_addr, res->ai_addrlen);
		sendfile(sockfd, fp, NULL, size);
	/*}
	else cout << "Timeout expired: is server down?" << endl;*/
}