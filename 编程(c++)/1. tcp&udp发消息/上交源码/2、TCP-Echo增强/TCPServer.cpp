#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <conio.h>
#include <algorithm>

#define	WSVERS	MAKEWORD(2, 0)
#define	BUFLEN		2000   

#pragma comment(lib,"ws2_32.lib") 

//将数字转换为字符串（倒序）
void calc(char tmp[BUFLEN + 1], int &len, int num) {
	if (num == 0) {
		tmp[len++] = '0';
	}
	else {
		while (num) {
			tmp[len++] = ((num % 10) + '0');
			num /= 10;
		}
	}
}

//由sin_addr获取客户端ip
void getip(char tmp[BUFLEN + 1], struct in_addr sin_addr) {
	memset(tmp, '\0', BUFLEN);
	int len = 0;
	//因为全部都为倒序,从4到1进行计算
	calc(tmp, len, sin_addr.S_un.S_un_b.s_b4);
	tmp[len++] = '.';
	calc(tmp, len, sin_addr.S_un.S_un_b.s_b3);
	tmp[len++] = '.';
	calc(tmp, len, sin_addr.S_un.S_un_b.s_b2);
	tmp[len++] = '.';
	calc(tmp, len, sin_addr.S_un.S_un_b.s_b1);
	//将结果翻转以得到正序结果
	std::reverse(tmp, tmp + len);
}

//由sin_port值获取客户端端口号
void getPort(char tmp[BUFLEN + 1], u_short sin_port) {
	memset(tmp, '\0', BUFLEN);
	int len = 0;
	sin_port = ((sin_port << 8) & 0xff00) + ((sin_port >> 8) & 0xff);
	calc(tmp, len, sin_port);
	std::reverse(tmp, tmp + len);
}

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
		int cc = recv(ssock, buf, BUFLEN, 0);	//从客户端接收信息
		if (cc == SOCKET_ERROR)
			printf("Error: %d.\n", GetLastError());
		else if (cc == 0) {
			printf("Client closed!\n");
		}
		else if (cc > 0) {
			buf[cc] = '\0';
			char message[BUFLEN + 1] = "内容:";
			printf("%s%s", message,buf);
			strcat(message, buf);
			strcat(message, "\n时间:");
			strcat(message, pts);
			printf("\n时间:%s", pts);
			printf("客户端ip地址:");
			strcat(message, "客户端ip地址:");
			char tmp[BUFLEN + 1];
			getip(tmp, fsin.sin_addr);
			printf("%s", tmp);
			strcat(message, tmp);
			strcat(message, "\n客户端端口号:");
			printf("\n客户端端口号:");
			getPort(tmp, fsin.sin_port);
			printf("%s\n\n", tmp);
			strcat(message, tmp);
			send(ssock, message, strlen(message), 0);	//向客户端发送信息
		}
		(void)closesocket(ssock);
	}
	(void)closesocket(msock);
	WSACleanup();
	printf("按回车键继续...\n");
	getchar();
	getchar();
}