#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#define	BUFLEN		2000 
#define WSVERS		MAKEWORD(2, 2)
#pragma comment(lib,"ws2_32.lib")

int main(int argc, char *argv[]){
	char	*host = "127.0.0.1";	
	char	*service = "50500"; 
	struct sockaddr_in toAddr;
	char	buf[BUFLEN+1];
	SOCKET	sock;
	int	cc;	
	char	*pts;

	WSADATA wsadata;
    WSAStartup(WSVERS, &wsadata);

    sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP);

	memset(&toAddr, 0, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
	toAddr.sin_port = htons((u_short)atoi(service));  
	toAddr.sin_addr.s_addr = inet_addr(host);  

	printf("请输入要发送的消息:\n");
	scanf("%s", buf);

	cc = sendto(sock, buf, BUFLEN, 0,(SOCKADDR *)&toAddr, sizeof(toAddr));
    if (cc == SOCKET_ERROR){
		printf("发送失败，错误号：%d\n", WSAGetLastError());
	}
	int tosize = sizeof(toAddr);
	cc = recvfrom(sock, buf, BUFLEN, 0, (SOCKADDR*)& toAddr, &tosize);
	if (cc == SOCKET_ERROR) {
		printf("recvfrom() failed; %d\n", WSAGetLastError());
		printf("按任意键退出...\n");
		getchar();
		getchar();
		return 0;
	}
	printf("\n%s\n", buf);
	closesocket(sock);
	WSACleanup();

	printf("按任意键退出...");
	getchar();
	getchar();
}
