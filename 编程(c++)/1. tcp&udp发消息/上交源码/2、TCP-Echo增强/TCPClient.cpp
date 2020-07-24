#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>

#define	BUFLEN		2000                
#define WSVERS		MAKEWORD(2, 0)      
#pragma comment(lib,"ws2_32.lib")  

int main(int argc, char* argv[]) {
	struct	sockaddr_in fsin;
	char* host = "127.0.0.1";
	char* service = "50500";
	struct sockaddr_in sin;
	char	buf[BUFLEN + 1];
	SOCKET	sock;
	int	cc;

	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);

	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(host);
	sin.sin_port = htons((u_short)atoi(service));
	;
	int ret = connect(sock, (struct sockaddr*) & sin, sizeof(sin));

	printf("������Ҫ���͵�����:\n");
	scanf("%s", buf);
	cc = send(sock, buf, strlen(buf), 0);	//�������������Ϣ
	memset(buf, '\0', BUFLEN);
	cc = recv(sock, buf, BUFLEN, 0);    //�ӷ�����������Ϣ
	if (cc == SOCKET_ERROR)
		printf("Error: %d.\n", GetLastError());
	else if (cc == 0) {
		printf("Server closed!\n");
	}
	else if (cc > 0) {
		buf[cc] = '\0';
		printf("�յ�����ϢΪ:\n%s\n", buf);
	}
	closesocket(sock);
	WSACleanup();

	printf("���س�������...");
	getchar();
	getchar();


}

