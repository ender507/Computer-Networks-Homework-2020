#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
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
};
void sendToAll(char *buf){
	for(int i=0; i<5; i++){
		if(hThread[i] != NULL){	
	       	send(ssocks[i], buf, strlen(buf), 0);                // 把缓冲区(指针,长度)的数据发送出去
		}
	}
}
unsigned __stdcall recive_send(void *socknum){
	char buf[BUFLEN+1];
	SOCKET sock = ((sockNum*)socknum)->sock;
	int num = ((sockNum*)socknum)->num;
	int cc;
	while(1){
		cc = recv(sock, buf, BUFLEN, 0);            // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
  	  	if(cc == SOCKET_ERROR){
			closesocket(sock);                         // 关闭监听套接字
   			ssocks[num] = NULL;
    		return 0;
		}
    	else if(cc > 0) {
	        buf[cc] = '\0';	                     		// ensure null-termination
	        printf("收到消息：\n%s\n",buf);                         // 显示所接收的字符串
	        sendToAll(buf);
	    }
	}
	return 0;
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


