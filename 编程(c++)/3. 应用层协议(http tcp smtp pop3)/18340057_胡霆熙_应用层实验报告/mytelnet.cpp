#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h> 

#define	BUFLEN		2000                  // 缓冲区大小
#define WSVERS		MAKEWORD(2, 0)        // 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")         // 使用winsock 2.0 Llibrary

unsigned __stdcall recive_print(void *sockaddr){
	char buf[BUFLEN+1];
	SOCKET sock = *((SOCKET*)sockaddr);
	int cc;
	while(1){
		cc = recv(sock, buf, BUFLEN, 0);            // cc为接收到的字符的个数(>0)或对方已关闭(=0)或连接出错(<0)
  	  	if(cc == SOCKET_ERROR){
  	  		int error = GetLastError();
  	  		if(error==10053)break;				//程序自然退出，不进行报错 
  	  		printf("Error: %d.\n",error);     	//出错。其后必须关闭套接字sock。
			closesocket(sock);                          // 关闭监听套接字
   			WSACleanup();                              // 卸载winsock library
			getchar();
    		exit(0);
		}
    	else if(cc > 0) {
	        buf[cc] = '\0';	                     		// ensure null-termination
	        printf("\n%s\n",buf);       			    // 显示所接收的字符串
	    }
	}
	return 0;
}

int main(int argc, char *argv[]){
	char	buf[BUFLEN+1];
	memset(buf, '\0', BUFLEN);
	printf("输入ip地址:");
	scanf("%s",buf);
	char	host[15];						/* server IP to connect         */
	strcpy(host, buf);
	memset(buf, '\0', BUFLEN);
	printf("输入端口号:");
	scanf("%s",buf);
	char	service[5];				    	/* server port to connect       */
	strcpy(service,buf);
	struct sockaddr_in sin;	        	    /* an Internet endpoint address	*/
	SOCKET	sock;		  	           		/* socket descriptor	    	*/
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						  //加载winsock library。WSVERS为请求的版本，wsadata返回系统实际支持的最高版本
  
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //创建套接字，参数：因特网协议簇(family)，流套接字，TCP协议
														  //返回：要监听套接字的描述符或INVALID_SOCKET
	
    memset(&sin, 0, sizeof(sin));						  // 从&sin开始的长度为sizeof(sin)的内存清0
    sin.sin_family = AF_INET;							  // 因特网地址簇
    sin.sin_addr.s_addr = inet_addr(host);                // 服务器IP地址(32位)
    sin.sin_port = htons((u_short)atoi(service));         // 服务器端口号  
    connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // 连接到服务器
	
	HANDLE hThread;									//用于接收和打印服务器发送的信息的子线程 
	hThread = (HANDLE)_beginthreadex(NULL, 0, &recive_print, (void*)&sock, 0, NULL); 
	
	char sendMes[BUFLEN+1];
	memset(sendMes,'\0',BUFLEN);
	gets(buf);										//清空缓冲区 
	printf("输入发送请求(最后一行用send表示结束)：\n");
	while(1){
		memset(buf,'\0',BUFLEN);
		gets(buf);
		if(buf[0]=='e'&&buf[1]=='x'&&buf[2]=='i'&&buf[3]=='t'&&buf[4]=='\0')break;//输入exit退出
		if(buf[0]=='s'&&buf[1]=='e'&&buf[2]=='n'&&buf[3]=='d'&&buf[4]=='\0'){	//输入send发送消息 
			send(sock, sendMes, strlen(sendMes), 0);
			memset(sendMes,'\0',BUFLEN+1);
			continue;
		}
		if(buf[0]=='\0'){				//输入空行，加上回车 
			strcat(sendMes, "\r\n");
		}
		else{							//输入一行字符，追加回车 
			strcat(sendMes, buf);
			strcat(sendMes, "\r\n");
		}
	}	
	CloseHandle(hThread);							//关闭子线程 
	shutdown(sock,SD_BOTH);
    closesocket(sock);                             // 关闭监听套接字
    WSACleanup();                                  // 卸载winsock library
    
    printf("按回车键继续...");
	getchar();										// 等待任意按键
}

