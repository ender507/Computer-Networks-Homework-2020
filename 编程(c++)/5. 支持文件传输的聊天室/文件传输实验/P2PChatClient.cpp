#include "P2PChatFunc.hpp"				//含有客户端和服务器都用得到的函数 

#define WSVERS		MAKEWORD(2, 0)        // 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")         // 使用winsock 2.0 Llibrary

int main(int argc, char *argv[]){
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
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
	hThread = (HANDLE)_beginthreadex(NULL, 0, &reciveMes, (void*)&sock, 0, NULL); 
	
	cmd(sock);			//输入指令 
	
	CloseHandle(hThread);							//关闭子线程 
	shutdown(sock,SD_BOTH);
    closesocket(sock);                             // 关闭监听套接字
    WSACleanup();                                  // 卸载winsock library
    
    printf("按回车键继续...");
	getchar();										// 等待任意按键
}

