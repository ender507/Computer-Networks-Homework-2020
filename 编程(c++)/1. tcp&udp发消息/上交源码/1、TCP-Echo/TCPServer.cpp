#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <conio.h>

#define	WSVERS	MAKEWORD(2, 0)
#define	BUFLEN		2000   

#pragma comment(lib,"ws2_32.lib") 

int main(int argc, char* argv[]) {
	struct	sockaddr_in fsin;
	SOCKET	msock, ssock;
	WSADATA wsadata;
	char* service = "50500";
	char	buf[BUFLEN + 1];
	struct  sockaddr_in sin;
	int	    alen;
	char* pts;
	time_t	now;


	WSAStartup(WSVERS, &wsadata);
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons((u_short)atoi(service));
	bind(msock, (struct sockaddr*) & sin, sizeof(sin));
	listen(msock, 5);

	while (!_kbhit()) {
		alen = sizeof(struct sockaddr);
		ssock = accept(msock, (struct sockaddr*) & fsin, &alen);

		(void)time(&now);
		pts = ctime(&now);

		memset(buf, '\0', BUFLEN);
		int cc = recv(ssock, buf, BUFLEN, 0);	//从客户端接受信息
		if (cc == SOCKET_ERROR)
			printf("Error: %d.\n", GetLastError());
		else if (cc == 0) {
			printf("Client closed!\n");
		}
		else if (cc > 0) {
			buf[cc] = '\0';
			printf("收到的信息为:%s\n", buf);
		}
		printf("收到信息的时间为%s\n", pts);
		strcat(pts, buf);
		send(ssock, pts, strlen(pts), 0);	//向客户端发送信息

		(void)closesocket(ssock);
	}
	(void)closesocket(msock);
	WSACleanup();
	printf("按任意键继续...\n");
	getchar();
	getchar();
}