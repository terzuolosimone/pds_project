
#include "server.h"

using std::set;
using std::cout;
using std::endl;
using std::string;
using std::runtime_error;

void service(int connfd, struct sockaddr_in cliaddr, socklen_t cliaddrlen){
	char buff[MAXBUFL];
	char filename[30];
	sprintf(filename, "ESP32_test_%d.txt", cliaddr.sin_port);
	int i;
	std::ofstream outfile (filename);
	FILE *fo;
	fo = fopen("ESPnames.txt", "a+");
	fprintf(fo, "%s\n", filename);
	fclose(fo);
	while(true){
		bzero(buff, MAXBUFL);
		char filesize[10];
		recvfrom(connfd, filesize, sizeof(uint32_t), 0, (struct sockaddr*)&cliaddr, &cliaddrlen);
		uint32_t size = ntohl((*(uint32_t*)filesize));
		recvfrom(connfd, buff, size*sizeof(char), 0, (struct sockaddr*)&cliaddr, &cliaddrlen);
		if(size > 0)
			outfile << buff << endl;
	}
}

int server(){
	int listenfd, connfd, err=0;
	short port;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t cliaddrlen = sizeof(cliaddr);
	// Get port to listen to from file config.txt
	char filename_config[] = "config.txt";
	FILE *fp = fopen(filename_config, "r");
	char port_in_file[20];
	char tmp[20];
	if(fp == NULL)
		return -1;
	while(fscanf(fp, "%s", tmp) != EOF){
		if(strcmp(tmp, "PORT:")==0)
			fscanf(fp, "%s", port_in_file);	
	}
	fclose(fp);
	port = atoi(port_in_file);
	// Create socket TCP
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	// Specify address to bind to
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenfd, (SA*) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);
	int x = 0;
	while(true){
		// Accept section
		x++;
		cout << "Waiting for connections..." << endl;
		connfd = accept(listenfd, (SA*) &cliaddr, &cliaddrlen) + x;
		if(connfd < 0)
			return -1;
		cout << "New connection from client "
		<< inet_ntoa(cliaddr.sin_addr) << ":"
		<< ntohs(cliaddr.sin_port) << endl;
		// Fork section
		int childpid;
		if((childpid = fork())<0)
			return -1;
		else if(childpid > 0){
			/* Parent process - close connection with current socket,
			 *  open a new one for other clients
			 */
			close(connfd);
		}
		else{
			/* Child process - close connection with parent, use new
			 *  socket for services
			 */
			close(listenfd);
			cout << "New child spawned!" << endl;
			service(connfd, cliaddr, cliaddrlen);
			cout << "The child has been killed" << endl;
			exit(0);
		}
		close(connfd);
	}
}