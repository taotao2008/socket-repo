#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) 
{
	char *host;
	struct hostent *hostinfo;
	struct servent *servinfo;
	int sockfd, server_len, result;
	struct sockaddr_in address;
	char buffer[128];
	if (argc == 1) {
		host = "localhost";
	} else {
		host = argv[1];
	}
	hostinfo = gethostbyname(host);
	if (!hostinfo) {
		fprintf(stderr, "cannot get info for host %s\n", host);
		exit(1);
	}

	servinfo = getservbyname("daytime", "udp");
	if (!servinfo) {
		fprintf(stderr, "no daytime service\n");
		exit(1);
	}
	printf("daytime port is %d\n", ntohs(servinfo -> s_port));

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr = *(struct in_addr *) *hostinfo -> h_addr_list;
	address.sin_port = servinfo -> s_port;
	server_len = sizeof(address);
	result = sendto(sockfd, buffer, 1, 0, (struct sockaddr *) &address, server_len);

	result = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *) &address, &server_len);
	buffer[result] = '\0';
	printf("read %d bytes:%s\n", result, buffer);
	close(sockfd);
	exit(0);

}
