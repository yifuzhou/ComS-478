//
//  Client.c
//  Agent
//
//  Created by Yifu Zhou on 2017/2/2.
//  Copyright © 2017年 Yifu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>


//type of package Beacon
typedef struct BEACON {
    int ID;
    int StartUpTime;
    char IP[4];
    int CmdPort;
} Beacon_t;

  Beacon_t beacon;

#define CLIENT_IP "127.0.0.1"
#define UDP_PORT 6666

void GetLocalOS(char OS[16], int *valid)
{
    char temp[] = "Lunix";
    strcpy(OS, temp);
}

void GetLocalTime(int *time, int *valid)
{
    *time = beacon.StartUpTime;
    
}

void *beacon_sender(void * v)
{

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in addr;
    memset((char*)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(UDP_PORT);
    inet_aton(CLIENT_IP, &addr.sin_addr);
    char buf = '0';
    
    while(1) {
        sendto(s, &beacon, sizeof(beacon), 0, (struct sockaddr*)&addr, sizeof(addr));
        sleep(10);
    }
     printf("THe end of loop!\n");
    
}

void CmdAgent()
{
    //before sending beacon, we must first generate the TCP connection
    
    int serv_sock;
    int clnt_sock;
    
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;
    
    char message[50] = "";
    
    char OS[50];
    GetLocalOS(OS, 1);
    strcat(message, OS);
    strcat(message, "#");
    
    int t;
    GetLocalTime(&t, 1);
    time_t mytime = t;
    strcat(message, ctime(&mytime));
    
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        fputs("socket error!", stderr);
        fputc('\n', stderr);
        exit(1);
    }
    
    in_port_t port = beacon.CmdPort;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        fputs("Bind error!", stderr);
        fputc('\n', stderr);
        exit(1);
    }
    
    pthread_t th1;
    pthread_create(&th1, NULL, beacon_sender, NULL);
    
    if (listen(serv_sock, 5) == -1)
    {
        fputs("listen error!", stderr);
        fputc('\n', stderr);
        exit(1);
    }
    
    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
    {
        fputs("accept error!", stderr);
        fputc('\n', stderr);
        exit(1);
    }
    
    write(clnt_sock, message, sizeof(message));
    close(clnt_sock);
    close(serv_sock);
    
    while (1);

}



int main(int argc, const char * argv[]) {
    
  
//    time_t mytime;
//    mytime = time(NULL);
//    printf("%s", ctime(&mytime));
    
    if (argc != 3) return -1;
    beacon.ID = atoi(argv[1]);
    
    time_t mytime;
    mytime = time(NULL);
    beacon.StartUpTime = mytime;
    beacon.CmdPort = atoi(argv[2]);
    
    inet_pton(AF_INET, CLIENT_IP, &beacon.IP);
    
    printf("Start running the Client!");
    printf("Its ID is %d, its CMDport is %d, and its start up time is %s \n",beacon.ID, beacon.CmdPort, ctime(&mytime));
    
    CmdAgent();

    
  
}


