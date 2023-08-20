#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#define PORT 9000
#define BUF_SIZE 1024
#define FILEPTH "/var/tmp/aesdsocketdata"

int sockfd;

void signal_handler(int signo)
{
    close(sockfd);
    remove(FILEPTH);
    exit(0);
}

int main(int argc, char **argv)
{
    char buf[BUF_SIZE];

    int connfd, len;
    struct sockaddr_in servaddr_in, cli_in;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error when create socket");
        return -1;
    }

    memset(&servaddr_in, 0, sizeof(struct sockaddr_in));
    memset(&cli_in, 0, sizeof(struct sockaddr_in));

    int opt = 1;
    int rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    servaddr_in.sin_family = AF_INET;
    servaddr_in.sin_addr.s_addr = INADDR_ANY;
    servaddr_in.sin_port = htons(PORT);

    if ((bind(sockfd, (struct sockaddr *)&servaddr_in, sizeof(servaddr_in))) !=
        0)
    {
        perror("Error when binding socket");
        return -1;
    }

    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        if (daemon(0, 1) < 0)
        {
            perror("Failed to create daemon");
            return -1;
        }
    }

    struct sigaction sigint;
    memset(&sigint, 0, sizeof(struct sigaction));
    sigint.sa_handler = signal_handler;
    sigaction(SIGINT, &sigint, NULL);
    sigaction(SIGTERM, &sigint, NULL);

    for (;;)
    {

        if ((listen(sockfd, 5)) != 0)
        {
            perror("Listen failed");
            return -1;
        }
        len = sizeof(cli_in);

        connfd = accept(sockfd, (struct sockaddr *)&cli_in, &len);
        if (connfd < 0)
        {
            perror("server accept failed");
            return -1;
        }
        else
        {
            // https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c
            char str[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &cli_in.sin_addr, str, INET_ADDRSTRLEN);
            syslog(LOG_USER || LOG_INFO, "Accepted connection from %s", str);
        }

        FILE *fi = fopen(FILEPTH, "a+");
        if (fi == NULL)
        {
            perror("Could not open file");
            return -1;
        }

        for (;;)
        {
            memset(buf, 0, BUF_SIZE);

            int ret;
            ret = recv(connfd, &buf, BUF_SIZE, 0);
            if (ret < 0)
            {
                perror("Error when reading socket data");
                return -1;
            }

            fwrite(buf, 1, ret, fi);

            if (buf[ret - 1] == '\n')
            {
                break;
            }
        }

        fseek(fi, 0, SEEK_END);
        size_t fsize = ftell(fi);
        if (fseek(fi, 0, SEEK_SET) == -1)
        {
            perror("Failed to set fd to beginning of file");
            return -1;
        }
        memset(buf, 0, BUF_SIZE);

        char *fbuf[fsize];
        memset(fbuf, 0, fsize);
        int ret = fread(fbuf, 1, fsize, fi);

        if (ret < 0)
        {
            perror("Failed reading file to send back to client");
            return -1;
        }
        else
        {
            send(connfd, fbuf, fsize, 0);
            memset(buf, 0, BUF_SIZE);
            memset(fbuf, 0, fsize);
        }

        fflush(fi);
        if (fclose(fi) < 0)
        {
            perror("Error when closing file");
            return -1;
        }

        if (close(connfd) < 0)
        {
            perror("Error when closing connection");
            return -1;
        }
    }

    return 0;
}