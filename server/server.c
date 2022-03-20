#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <netdb.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "types.h"
#include "sock.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char txt[10] = ".txt";
void command(char*,char*);
void* threadclient(void*);

int main(int argc, char **argv)
{
    char *server_port = 0;
    int opt = 0;
    /* Parsing args */
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
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

    if (!server_port)
    {
        fprintf(stderr, "Error! No port number provided!\n");
        exit(1);
    }

    /* Open a listen socket fd */
    int sockfd __attribute__((unused)) = open_listenfd(server_port);
    printf("Listening on the port %s\n",server_port);
    struct sockaddr_in clientInfo;
    unsigned int addrlen = sizeof(clientInfo);
    mkdir("./database",0777);
    while(1)
    {
        int* clientfd =(int*)malloc(sizeof(int));
        *clientfd=accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
        pthread_t* thd=(pthread_t*)malloc(sizeof(pthread_t));
        pthread_create(thd,NULL,threadclient,(void*)clientfd);
    }
    return 0;
}
void* threadclient(void* Clientfd)
{
    pthread_detach(pthread_self());
    char input[10000];
    char message[] = {"Please enter command\n"};
    char clientmes[]= {"server recieved\nPlease enter command\n"};
    int clientfd = *(int*)Clientfd;
    printf("Thread %lu created, serving connection fd %d\n", pthread_self(), clientfd);
    send(clientfd,message,sizeof(message),0);
    while(1)
    {
        int n;
        n=recv(clientfd,input,sizeof(input),0);
        if(n==0||n==-1)
        {
            printf("Client %d disconnected!\n",clientfd);
            break;
        }
        command(input,clientmes);
        send(clientfd,clientmes,sizeof(clientmes),0);
    }
    pthread_exit(NULL);
}
void command(char* input,char* remes)
{
    pthread_mutex_lock(&mutex);
    char k[10000]="";
    char v[10000]="";
    char d[10000]="";
    char o[10000]="";
    int state = 4;
    sscanf(input,"%s",d);
    if(!strcmp(d,"EXIT")) state = 0;
    else if(!strcmp(d,"SET")) state = 1;
    else if(!strcmp(d,"GET")) state = 2;
    else if(!strcmp(d,"DELETE")) state = 3;
    switch(state)
    {
    case 0:
        sscanf(input,"%s%s",d,o);
        if(strlen(o)!=0) sprintf(remes,"wrong format\n");
        else sprintf(remes,"%s",d);
        break;
    case 1:
        sscanf(input,"%s%s%s%s",d,k,v,o);
        if(strlen(k)==0||strlen(v)==0||strlen(o)!=0) sprintf(remes,"wrong format\n");
        else
        {
            char file[10000] = "./database/";
            strcat(k,txt);
            strcat(file,k);
            FILE *ptr;
            ptr=fopen(file,"r+");
            if(ptr!=NULL)
            {
                sprintf(remes,"key repeated\n");
            }
            else
            {
                ptr=fopen(file,"w");
                fputs(v,ptr);
                sprintf(remes,"key %s stored\n",k);
            }
            fclose(ptr);
        }
        break;
    case 2:
        sscanf(input,"%s%s%s",d,k,o);
        if(strlen(k)==0||strlen(o)!=0) sprintf(remes,"wrong format\n");
        else
        {
            sprintf(remes,"%s\n",d);
            char file[10000] = "./database/";
            strcat(k,txt);
            strcat(file,k);
            FILE *ptr;
            ptr=fopen(file,"r+");
            if(ptr!=NULL)
            {
                fscanf(ptr,"%s",v);
                sprintf(remes,"value  of key %s is %s\n",k,v);
                fclose(ptr);
            }
            else sprintf(remes,"key not found\n");
        }
        break;
    case 3:
        sscanf(input,"%s%s%s",d,k,o);
        if(strlen(k)==0||strlen(o)!=0) sprintf(remes,"wrong format\n");
        else
        {
            sprintf(remes,"%s\n",d);
            char file[10000] = "./database/";
            strcat(k,txt);
            strcat(file,k);
            FILE *ptr;
            ptr=fopen(file,"r+");
            if(ptr!=NULL)
            {
                remove(file);
                sprintf(remes,"key %s removed!\n",k);
                fclose(ptr);
            }
            else sprintf(remes,"key not found\n");
        }
        break;
    default:
        sprintf(remes,"wrong format\n");
    }
    pthread_mutex_unlock(&mutex);
}
