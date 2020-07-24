#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <algorithm>

#define	BUFLEN		2000
#define WSVERS		MAKEWORD(2, 2) 
#pragma comment(lib,"ws2_32.lib")

int main(int argc, char *argv[]){
	char   *host = "127.0.0.1";	
	char   *service = "50500";  
	struct sockaddr_in sin;	  
	struct sockaddr_in from;  
	int    fromsize = sizeof(from);
	char   buf[BUFLEN+1];   
	char* pts;
	SOCKET	sock;		  	
	int	cc;	
	time_t	now;

	WSADATA wsadata;
    WSAStartup(WSVERS, &wsadata);     		
    sock = socket(PF_INET, SOCK_DGRAM,IPPROTO_UDP);
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;   
	sin.sin_port = htons((u_short)atoi(service));
	bind(sock, (struct sockaddr *)&sin, sizeof(sin));

 	while(!_kbhit()){ 
	    cc = recvfrom(sock, buf, BUFLEN, 0, (SOCKADDR *)&from, &fromsize);
		if (cc == SOCKET_ERROR){
            printf("recvfrom() failed; %d\n", WSAGetLastError());
            break;
        }
        else if (cc == 0)
            break;
		else{
			(void)time(&now);
			pts = ctime(&now);
			buf[cc] = '\0';
			char message[BUFLEN + 1];
			memset(message, '\0', BUFLEN);
			printf("�ͻ�����Ϣ��%s", buf);
			strcat(message, "�ͻ�����Ϣ��");
			strcat(message, buf);
			printf("\nʱ��:%s",pts);
			strcat(message, "\nʱ��:");
			strcat(message, pts);
			char* tmp = inet_ntoa(from.sin_addr);
			printf("�ͻ���ip��ַ:%s", tmp);
			strcat(message, "�ͻ���ip��ַ:");
			strcat(message, tmp);
			memset(tmp, '\0', BUFLEN);
			int len = 0, num = from.sin_port;
			num = ((num << 8) & 0xff00) + ((num >> 8) & 0xff);
			while(num) {
				tmp[len++] = ((num % 10) + '0');
				num /= 10;
			}
			std::reverse(tmp, tmp + len);
			printf("\n�ͻ��˶˿ں�:%s\n\n", tmp);
			strcat(message, "\n�ͻ��˶˿ں�:");
			strcat(message, tmp);
			cc = sendto(sock, message,BUFLEN, 0, (SOCKADDR*)& from, sizeof(from));
			if (cc == SOCKET_ERROR) {
				printf("����ʧ�ܣ�����ţ�%d\n", WSAGetLastError());
			}
		}
	}
	closesocket(sock);
	WSACleanup();
	getchar();
}



