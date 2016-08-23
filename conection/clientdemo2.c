#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

struct sockaddr_in address;

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


void parseConfig(char* filePath)
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
                    address.sin_addr.s_addr = inet_addr(del_both_trim(p));
                }
                if (count == 1) {
                    address.sin_port = htons(atoi(del_both_trim(p)));
                }
                count++;
            }
            
        }
    }

    
}

int main(int argc, char *argv[])
{

    if (argc != 3) {
        printf("parameter error.\nUsage:./appname filePath lettle\n");
        exit(1);
    }

    int sockfd;
    int len;
    int result;
    char ch = *argv[2];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    parseConfig(argv[1]);
    len = sizeof(address);
    result = connect(sockfd, (struct sockaddr *) &address, len);
    if (result == -1) {
        perror("oops: client1");
        exit(1);
    }
    write(sockfd, &ch, 1);
    //read(sockfd, &ch, 1);
    printf("send data = %c\n", ch);
    close(sockfd);
    return 0;
}

