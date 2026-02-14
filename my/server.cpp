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
    errif(sockfd == -1, "socket create error");
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    socklen_t clnt_addr_len = sizeof(client_addr);
    bzero(&client_addr, sizeof(client_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    int bind_result = bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    errif(bind_result == -1, "socket bind error");

    int listen_result = listen(sockfd, SOMAXCONN);
    errif(listen_result == -1, "socket listen error");

    int client_sockfd = accept(sockfd, (sockaddr *)&client_addr, &clnt_addr_len);
    errif(client_sockfd == -1, "socket accept error");
    printf("new client fd %d! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (true)
    {
        char buffer[1024];
        bzero(buffer, sizeof(buffer));
        ssize_t read_bytes = read(client_sockfd, buffer, sizeof(buffer));
        if (read_bytes == 0)
        {
            printf("client fd %d closed\n", client_sockfd);
            close(client_sockfd);
            break;
        }
        else if (read_bytes < 0)
        {
            perror("read error");
            close(client_sockfd);
            break;
        }
        write(client_sockfd, buffer, strlen(buffer));
    }

    return 0;
}