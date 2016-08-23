#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

struct sockaddr_in server_address;
struct sockaddr_in client_address;

struct sockaddr_in destination_address;

/* 我们一次所能够接收的最大字节数 */
#define MAXDATASIZE 100

#define MAX_LINE 1024

/*   删除左边的空格   */
char * del_left_trim(char *str) {
    assert(str != NULL);
    for (;*str != '\0' && isblank(*str) ; ++str);
    return str;
}
/*   删除两边的空格   */
char * del_both_trim(char * str) {
    char *p;
    char * szOutput;
    szOutput = del_left_trim(str);
    for (p = szOutput + strlen(szOutput) - 1; p >= szOutput && isblank(*p);
            --p);
    *(++p) = '\0';
    return szOutput;
}


void parseServerConfig(char* filePath)
{
	char buf[MAX_LINE];  /*缓冲区*/
	FILE *fp;
	char *pos;
	int len;
	int count = 0;
	char subch[256];
	if((fp = fopen(filePath,"r")) == NULL)
	{
		perror("fail to read");
		exit (1) ;
	}

	char s[256];
    /*分割符*/
    char * delim = "=";
    char * p;
    char ch;
     while (!feof(fp)) {
        if ((p = fgets(buf, sizeof(buf), fp)) != NULL) {
            strcpy(s, p);
            ch=del_left_trim(s)[0];
           /*判断注释 空行，如果是就直接下次循环*/
            if (ch == '#' || isblank(ch) || ch=='\n')
                continue;
          /*分割字符串*/
            p=strtok(s, delim);
            while ((p = strtok(NULL, delim))){
            	if (count == 0) {
            		server_address.sin_addr.s_addr = htonl(atoi(del_both_trim(p)));
            	}
            	if (count == 1) {
            		server_address.sin_port = htons(atoi(del_both_trim(p)));
            	}
            	count++;
            }
            
        }
    }
    
}



void parseClientConfig(char* filePath)
{
	char buf[MAX_LINE];  /*缓冲区*/
	FILE *fp;
	char *pos;
	int len;
	int count = 0;
	char subch[256];
	if((fp = fopen(filePath,"r")) == NULL)
	{
		perror("fail to read");
		exit (1) ;
	}

	char s[256];
    /*分割符*/
    char * delim = "=";
    char * p;
    char ch;
     while (!feof(fp)) {
        if ((p = fgets(buf, sizeof(buf), fp)) != NULL) {
            strcpy(s, p);
            ch=del_left_trim(s)[0];
           /*判断注释 空行，如果是就直接下次循环*/
            if (ch == '#' || isblank(ch) || ch=='\n')
                continue;
          /*分割字符串*/
            p=strtok(s, delim);
            while ((p = strtok(NULL, delim))){
            	if (count == 0) {
            		destination_address.sin_addr.s_addr = inet_addr(del_both_trim(p));
            	}
            	if (count == 1) {
            		destination_address.sin_port = htons(atoi(del_both_trim(p)));
            	}
            	count++;
            }
            
        }
    }
    
}



void sendData(char ch,char *filePath) {
	int sockfd;
	int len;
	int result;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	destination_address.sin_family = AF_INET;
	parseClientConfig(filePath);
	len = sizeof(destination_address);
	result = connect(sockfd, (struct sockaddr *) &destination_address, len);
	if (result == -1) {
		perror("oops: client1");
		exit(1);
	}
	write(sockfd, &ch, 1);
	//read(sockfd, &ch, 1);
	printf("send data = %c\n", ch);
	close(sockfd);
}

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("parameter error.\nUsage:./appname serverFilePath clientFilePath\n");
		exit(1);
	}
	int server_sockfd,client_sockfd;
	int server_len,client_len;
	parseServerConfig(argv[1]);
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_address.sin_family = AF_INET;
	server_len = sizeof(server_address);
	bind(server_sockfd, (struct sockaddr *) &server_address, server_len);
	listen(server_sockfd, 5);
	while(1) {
		char ch;
		printf("server waiting\n");
		client_len = sizeof(client_address);
		client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_address, &client_len);
		read(client_sockfd, &ch, 1);
		printf("read data:%c\n", ch);
		sendData(ch, argv[2]);
		//write(client_sockfd, &ch, 1);
		close(client_sockfd);
	}

}
