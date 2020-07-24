#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h> 

#define	BUFLEN		200000              		// ��������С
#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

char MES[BUFLEN+1];

void sendMes(SOCKET	sock){
	printf("%s",MES);
	send(sock, MES, strlen(MES), 0);
	memset(MES, '\0', BUFLEN+1);
}

void receMes(SOCKET sock, bool write, bool show){
	char buf[BUFLEN+1];
	memset(buf, '\0', BUFLEN+1);
	int cc = recv(sock, buf, BUFLEN, 0);
	if(cc == SOCKET_ERROR){
  		int error = GetLastError();
  		printf("Error: %d.\n",error);     	//����������ر��׽���sock��
		closesocket(sock);                          // �رռ����׽���
		WSACleanup();                              // ж��winsock library
		getchar();
		exit(0);
	}
	if(show)printf("%s", buf);
	if(write){
		memset(MES, '\0', BUFLEN+1);
		strcpy(MES, buf);
	}
}

void getPort(char port[5]){
	int num = 0;
	int pos = 0;
	while(MES[pos] != ',') pos++;
	pos++;
	while(MES[pos] != ',') pos++;
	pos++;
	while(MES[pos] != ',') pos++;
	pos++;
	while(MES[pos] != ',') pos++;
	pos++;
	while(MES[pos] != ','){
		num *= 10;
		num += (MES[pos]-'0');
		pos++;
	}
	pos++;
	num *= 256;
	int num2 = 0;
	while(MES[pos] != ')'){
		num2 *= 10;
		num2 += (MES[pos]-'0');
		pos++;
	}
	num += num2;
	for(int i=4; i>=0 ; i--){
		port[i] = (num%10)+'0';
		num /= 10;
	}
	memset(MES, '\0', BUFLEN);
}

void saveFile(SOCKET sock2, char localPos[]){
	memset(MES, '\0', BUFLEN+1);
	FILE* pFile = fopen(localPos,"wb");
	if(pFile == NULL){
		printf("cannot open the file!\n");
		getchar();
		exit(0);
	}
	while(1){
		memset(MES, '\0', BUFLEN);
		int cc = recv(sock2, MES, BUFLEN, 0);
		if( cc==SOCKET_ERROR || cc == 0)break;
		fwrite(MES, 1, cc, pFile);
	}
	fclose(pFile);
}

int main(int argc, char *argv[]){
	char	buf[BUFLEN+1];
	memset(buf, '\0', BUFLEN+1);
	
	printf("������ip��ַ:");
	scanf("%s",buf);
	char	host[15];						/* server IP to connect         */
	strcpy(host, buf);
	memset(buf, '\0', BUFLEN);
	printf("������˿ں�:");
	scanf("%s",buf);
	char	service[5];				    	/* server port to connect       */
	strcpy(service,buf);
	
	char	netPos[BUFLEN+1], localPos[BUFLEN+1];
	memset(netPos, '\0', BUFLEN+1);
	memset(localPos, '\0', BUFLEN+1);
	printf("�����������ļ���λ�ã�");
	scanf("%s",netPos);
	printf("�����뱾�ش洢��λ��:");
	scanf("%s",localPos);
	
	struct sockaddr_in sin;	        	    /* an Internet endpoint address	*/
	SOCKET	sock;		  	           		/* socket descriptor	    	*/
	WSADATA wsadata;
	WSAStartup(WSVERS, &wsadata);						  //����winsock library��WSVERSΪ����İ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾
  
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	  //�����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														  //���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET
	
    memset(&sin, 0, sizeof(sin));						  // ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
    sin.sin_family = AF_INET;							  // ��������ַ��
    sin.sin_addr.s_addr = inet_addr(host);                // ������IP��ַ(32λ)
    sin.sin_port = htons((u_short)atoi(service));         // �������˿ں�  
    connect(sock, (struct sockaddr *)&sin, sizeof(sin));  // ���ӵ�������

	gets(buf);										//��ջ�����
	
	//��¼���ȡ�������Ӷ˿� 
	memset(MES, '\0', BUFLEN+1);
	receMes(sock, false, true);
	strcpy(MES, "user net\r\n");
	sendMes(sock);
	receMes(sock, false, true);
	strcpy(MES, "pass 123456\r\n");
	sendMes(sock);
	receMes(sock, false, true);
	strcpy(MES, "type I\r\n");
	sendMes(sock);
	receMes(sock, false, true);
	strcpy(MES, "pasv\r\n");
	sendMes(sock);
	receMes(sock, true, true);
	
	//������������ 
	char service2[5];
	getPort(service2);
	SOCKET	sock2 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sin.sin_port = htons((u_short)atoi(service2));
	connect(sock2, (struct sockaddr *)&sin, sizeof(sin));

	//�����ļ�
	strcpy(MES, "retr ");
	strcat(MES, netPos);
	strcat(MES, "\r\n");
	sendMes(sock);
	receMes(sock, false, true);
	saveFile(sock2, localPos);
	receMes(sock, false, true);
	
	strcpy(MES, "quit\r\n");
	sendMes(sock);
	receMes(sock, false, true);
	
	shutdown(sock,SD_BOTH);
	shutdown(sock2,SD_BOTH);
    closesocket(sock);                             // �رռ����׽���
    closesocket(sock2);
    WSACleanup();                                  // ж��winsock library
    
    printf("���س�������...");
	getchar();										// �ȴ����ⰴ��
}

