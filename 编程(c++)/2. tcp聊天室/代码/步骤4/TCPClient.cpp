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
	printf(">>");
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
	        printf("\n收到消息：\n%s\n",buf);                         // 显示所接收的字符串
	        printf(">>");
	    }
	}
	return 0;
}


int main(int argc, char *argv[]){
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
	char	buf[BUFLEN+1];   		        /* buffer for one line of text	*/
	SOCKET	sock;		  	            /* socket descriptor	    	*/

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
	while(1){	
		memset(buf,'\0',BUFLEN);
		gets(buf);
		if(buf[0]=='e'&&buf[1]=='x'&&buf[2]=='i'&&buf[3]=='t'&&buf[4]=='\0')break;//输入exit退出
		send(sock, buf, BUFLEN, 0);
	}
	CloseHandle(hThread);							//关闭子线程 
    closesocket(sock);                             // 关闭监听套接字
    WSACleanup();                                  // 卸载winsock library
    
    printf("按回车键继续...");
	getchar();										// 等待任意按键
}

