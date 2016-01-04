//
//  main.c
//  socket
//
//  Created by mapengfei on 15/12/7.
//  Copyright © 2015年 mapengfei. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define SERVER_PORT 9500

void *f_connect_thread(void *pvParam);

int main()
{
    int sockdf;
    int new_conn;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    int sin_size;
    char send_buffer[10240]="hello world hello world";
    send_buffer[4223] = 0;
    pthread_t	hThread;
    if((sockdf = socket(AF_INET,SOCK_STREAM,0))<=0)
    {
        printf("获取socket失败\n");
        exit(0);
    }
    printf("获取socket\n");
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(SERVER_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    
    bzero(&(my_addr.sin_zero),0);
    
    if(bind(sockdf,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
    {
        printf("绑定端口失败\n");
        exit(1);
    }
    printf("绑定端口\n");
    
    listen(sockdf,5);
    printf("监听开始\n");
    
    sin_size = sizeof(struct sockaddr_in);
    
    while(1)
    {
        new_conn = accept(sockdf,(struct sockaddr *)&their_addr,(unsigned int *)&sin_size);
        printf("here comes a new connection!\n");
        
        if (pthread_create(&hThread ,NULL ,f_connect_thread ,(int *)&new_conn))
        {
            printf("error on create new thread \n");
            close(new_conn);
            continue;
        }
        else
        {
            pthread_detach(hThread);
        }
        
       // printf("got connect from %l⧵n",inte_ntoa(their_addr.sin_addr));
        
        

    
    }
    
}


void *f_connect_thread(void *pvParam)
{
    int	li_len=0,li_fileno;
    char send_buf[1024];
    char rcv_buf[1024];
    char command[1024];
    pthread_t	lthreadid;
    FILE *cmd;
    memset(send_buf,0,1024);
    memset(rcv_buf,0,1024);
    
    if((int *)pvParam == NULL) pthread_exit(NULL);
    li_fileno = *(int *)pvParam;
    
    lthreadid = pthread_self();
    pthread_detach(lthreadid);
    
    printf("new thread to deal with connection\n");
    
    while(1)
    {
        memset(send_buf,0,1024);
        memset(rcv_buf,0,1024);
        
        if ((li_len = recv(li_fileno,rcv_buf,1024,0))<=0)
        {
            if (li_len==-1)
                printf("error1\n");
            else
                printf("error2\n");
            close(li_fileno);
            pthread_exit(NULL);		/* ÖÐÖ¹´ËÏß³Ì */
        }
        printf("length from client is %d\n",strlen(rcv_buf));
        printf("rcv from client %s\n",rcv_buf);
        if(strcmp(rcv_buf, "\377\365")==0) continue;
        rcv_buf[strlen(rcv_buf)-2] = 0;
        
        sprintf(send_buf, "i rcv your messeg\n");
        if(strcmp(rcv_buf, "bye")==0) break;
        sprintf(command, rcv_buf);
        
        cmd = popen(command, "r");
        if (cmd==NULL) {
            printf("command fail\n");
        }
        fgets(send_buf, sizeof(send_buf), cmd);
        send_buf[sizeof(send_buf)-1] = 0;
        li_len = pclose(cmd);
        if(li_len < 0) printf("close error !!\n");
        printf("command return %s\n",send_buf);
        
        send(li_fileno, send_buf, strlen(send_buf), 0);
        printf("1111111111\n");
    }
    printf("22222222\n");
    close(li_fileno);
    pthread_exit(NULL);
}
