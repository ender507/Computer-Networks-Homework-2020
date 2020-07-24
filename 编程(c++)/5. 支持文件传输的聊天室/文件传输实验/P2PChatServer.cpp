#include "P2PChatFunc.hpp"				//含有客户端和服务器都用得到的函数 

#define	WSVERS	MAKEWORD(2, 0)

#pragma comment(lib,"ws2_32.lib")

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

	listen(msock, 5);                                   // 一对一链接则等待建立连接的队列长度为1
	alen = sizeof(struct sockaddr);
	ssock = accept(msock, (struct sockaddr *)&fsin, &alen);

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &reciveMes, (void*)&ssock, 0, NULL); 
	
	cmd(ssock);			//输入指令 
	
	CloseHandle(hThread);
	closesocket(msock);                                 // 关闭监听套接字
    WSACleanup();                                       // 卸载winsock library
}
