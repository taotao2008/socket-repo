#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
/* 服务器程序监听的端口号 */
#define PORT 4000
/* 我们一次所能够接收的最大字节数 */
#define MAXDATASIZE 100
int main(int argc, char *argv[])
{
	int sockfd;
	int len;
	struct sockaddr_in address;
	int result;
	char ch = 'A';
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(9734);
	len = sizeof(address);
	result = connect(sockfd, (struct sockaddr *) &address, len);
	if (result == -1) {
		perror("oops: client1");
		exit(1);
	}
	write(sockfd, &ch, 1);
	read(sockfd, &ch, 1);
	printf("char from server = %c\n", ch);
	close(sockfd);
	return 0;
}
