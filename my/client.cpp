#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <unistd.h>

void errif(bool condition, const char *errmsg)
{
    if (condition)
    {
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));

    while (true)
    {
        char buffer[1024];
        bzero(buffer, sizeof(buffer));
        scanf("%s", buffer);
        ssize_t write_bytes = write(sockfd, buffer, strlen(buffer));
        if (write_bytes < 0)
        {
            printf("write error\n");
            close(sockfd);
            break;
        }
        bzero(buffer, sizeof(buffer));                             // 清空缓冲区
        ssize_t read_bytes = read(sockfd, buffer, sizeof(buffer)); // 从服务器socket读到缓冲区，返回已读数据大小
        if (read_bytes > 0)
        {
            printf("message from server: %s\n", buffer);
        }
        else if (read_bytes == 0)
        { // read返回0，表示EOF，通常是服务器断开链接，等会儿进行测试
            printf("server socket disconnected!\n");
            break;
        }
        else if (read_bytes == -1)
        { // read返回-1，表示发生错误，按照上文方法进行错误处理
            close(sockfd);
            errif(true, "socket read error");
        }
    }

    return 0;
}