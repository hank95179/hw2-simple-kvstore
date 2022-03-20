#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdlib.h>

#include "sock.h"


int main(int argc, char **argv)
{
    int opt;
    char *server_host_name = NULL, *server_port = NULL;

    /* Parsing args */
    while ((opt = getopt(argc, argv, "h:p:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            server_host_name = malloc(strlen(optarg) + 1);
            strncpy(server_host_name, optarg, strlen(optarg));
            break;
        case 'p':
            server_port = malloc(strlen(optarg) + 1);
            strncpy(server_port, optarg, strlen(optarg));
            break;
        case '?':
            fprintf(stderr, "Unknown option \"-%c\"\n", isprint(optopt) ?
                    optopt : '#');
            return 0;
        }
    }

    if (!server_host_name)
    {
        fprintf(stderr, "Error!, No host name provided!\n");
        exit(1);
    }

    if (!server_port)
    {
        fprintf(stderr, "Error!, No port number provided!\n");
        exit(1);
    }


    /* Open a client socket fd */
    int sockfd __attribute__((unused)) = open_clientfd(server_host_name, server_port);

    /* Start your coding client code here! */
    char mes[10000];
    char receive[10000];
    recv(sockfd,receive,sizeof(receive),0);
    printf("%s",receive);
    while(1)
    {
        fgets(mes,10000,stdin);
        send(sockfd,mes,sizeof(mes),0);
        int n;
        n=recv(sockfd,receive,sizeof(receive),0);
        if(n==0||n==-1)
        {
            printf(" error \nServer closed!\n");
            break;
        }
        if(strcmp(receive,"EXIT")==0)
        {
            printf("%s server",receive);
            break;
        }
        printf("%s",receive);
    }
    close(sockfd);
    return 0;
}
