#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <process.h>
#include "conio.h"

#define	WSVERS	MAKEWORD(2, 0)
#define BUFLEN 2000

#pragma comment(lib,"ws2_32.lib")

HANDLE hThread[5];		//最多可接入5个客户端
SOCKET ssocks[5];		//5个ssock，分别对应五个客户端 
struct sockNum{
	SOCKET sock;
	int num;
	struct	sockaddr_in fsin;
};

void sendToAll(char *buf){
	for(int i=0; i<5; i++){
		if(ssocks[i] != NULL){	
	       	send(ssocks[i], buf, strlen(buf), 0);                // 把缓冲区(指针,长度)的数据发送出去
		}
	}
}

unsigned __stdcall recive_send(void *socknum){
	char buf[BUFLEN+1];
	SOCKET sock = ((sockNum*)socknum)->sock;
	int num = ((sockNum*)socknum)->num;
	struct	sockaddr_in fsin = ((sockNum*)socknum)->fsin;
	int cc;
	while(1){
		cc = recv(sock, buf, BUFLEN, 0);            // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
  	  	if(cc == SOCKET_ERROR){						//SOCKET_ERROR说明客户端已经关闭
			closesocket(sock);                         // 关闭监听套接字
   			ssocks[num] = NULL;
   			time_t	now;
      	 	time(&now);                                      // 取得系统时间
       		char *pts = ctime(&now);                                      // 把时间转换为字符串
			char tmp[BUFLEN+1];
			strcat(tmp,"A Client Exit!\n时间:");
			strcat(tmp,pts);
			strcat(tmp,"ip地址:");
			strcat(tmp,inet_ntoa(fsin.sin_addr));
			strcat(tmp,"\n端口号:");
			char tmp2[BUFLEN+1];
			itoa((u_short)ntohs(fsin.sin_port),tmp2,10);
			strcat(tmp,tmp2);
			printf("%s\n\n",tmp);
	        sendToAll(tmp);
	        memset(tmp,'\0',BUFLEN);
    		return 0;
		}
    	else if(cc > 0) {
    		time_t	now;
      	 	time(&now);                                      // 取得系统时间
       		char *pts = ctime(&now);                                      // 把时间转换为字符串
			char tmp[BUFLEN+1];
			strcat(tmp,"时间:");
			strcat(tmp,pts);
			strcat(tmp,"ip地址:");
			strcat(tmp,inet_ntoa(fsin.sin_addr));
			strcat(tmp,"\n端口号:");
			char tmp2[BUFLEN+1];
			itoa((u_short)ntohs(fsin.sin_port),tmp2,10);
			strcat(tmp,tmp2);
			strcat(tmp,"\n消息:\n");
			strcat(tmp,buf);
			printf("%s\n\n",tmp);
	        sendToAll(tmp);
	        memset(tmp,'\0',BUFLEN);
	    }
	}
	return 0;
}

void enter(struct sockaddr_in fsin){
	char tmp[BUFLEN+1];
	time_t	now;
 	time(&now);                                      // 取得系统时间
	char *pts = ctime(&now);                                      // 把时间转换为字符串
	strcat(tmp,"New Client entered!\n时间:");
	strcat(tmp,pts);
	strcat(tmp,"ip地址:");
	strcat(tmp,inet_ntoa(fsin.sin_addr));
	strcat(tmp,"\n端口号:");
	char tmp2[BUFLEN+1];
	itoa((u_short)ntohs(fsin.sin_port),tmp2,10);
	strcat(tmp,tmp2);
	strcat(tmp,"\n");
	printf("%s\n\n",tmp);
    sendToAll(tmp);
    memset(tmp,'\0',BUFLEN);
}

int main(int argc, char *argv[]) {
	struct	sockaddr_in fsin;	    /* the from address of a client	  */
	SOCKET	msock, ssock;		    /* master & slave sockets	      */
	WSADATA wsadata; 
	char	*service = "50500";
	struct  sockaddr_in sin;	    /* an Internet endpoint address		*/
    int	    alen;			        /* from-address length		        */

	WSAStartup(WSVERS, &wsadata);						// 加载winsock library。WSVERS指明请求使用的版本。wsadata返回系统实际支持的最高版本
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// 创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														// 返回：要监听套接字的描述符或INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// 从&sin开始的长度为sizeof(sin)的内存清0
	sin.sin_family = AF_INET;							// 因特网地址簇(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// 监听所有(接口的)IP地址。
	sin.sin_port = htons((u_short)atoi(service));		// 监听的端口号。atoi--把ascii转化为int，htons--主机序到网络序(16位)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // 绑定监听的IP地址和端口号

	listen(msock, 5);                                   // 等待建立连接的队列长度为5
	for(int i=0; i<5; i++){
		ssocks[i] = NULL;
		hThread[i] = NULL;
	}
	
    while(!_kbhit()){ 		                             // 检测是否有按键
       	alen = sizeof(struct sockaddr);                   // 取到地址结构的长度
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen); // 如果有新的连接请求，返回连接套接字，否则，被阻塞。fsin包含客户端IP地址和端口号
		if(ssock == NULL)continue;
		for(int i=0; i<5; i++){
	    	if(ssocks[i] == NULL){//当某条线程空闲，则处理当前客户端 
	    		if(hThread[i] != NULL)CloseHandle(hThread[i]);
				ssocks[i] = ssock;
	    		struct sockNum tmp;
	    		tmp.sock = ssock;
				tmp.num = i;
				tmp.fsin = fsin;
				enter(fsin);
	    		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, &recive_send, (void*)&tmp, 0, NULL); 
				break;
			}
			if(i == 4){
				printf("客户端已达到最大上限！\n");
			}
		}
    }
	closesocket(msock);                                 // 关闭监听套接字
    WSACleanup();                                             // 卸载winsock library
    
}

