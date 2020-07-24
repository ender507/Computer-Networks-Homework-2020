#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h> 

#define	BUFLEN		200000              		// 缓冲区大小
#define WSVERS		MAKEWORD(2, 0)        // 指明版本2.0 
#pragma comment(lib,"ws2_32.lib")         // 使用winsock 2.0 Llibrary

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
  		printf("Error: %d.\n",error);     	//出错。其后必须关闭套接字sock。
		closesocket(sock);                          // 关闭监听套接字
		WSACleanup();                              // 卸载winsock library
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
	
	printf("请输入ip地址:");
	scanf("%s",buf);
	char	host[15];						/* server IP to connect         */
	strcpy(host, buf);
	memset(buf, '\0', BUFLEN);
	printf("请输入端口号:");
	scanf("%s",buf);
	char	service[5];				    	/* server port to connect       */
	strcpy(service,buf);
	
	char	netPos[BUFLEN+1], localPos[BUFLEN+1];
	memset(netPos, '\0', BUFLEN+1);
	memset(localPos, '\0', BUFLEN+1);
	printf("请输入下载文件的位置：");
	scanf("%s",netPos);
	printf("请输入本地存储的位置:");
	scanf("%s",localPos);
	
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

	gets(buf);										//清空缓冲区
	
	//登录与获取数据连接端口 
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
	
	//建立数据连接 
	char service2[5];
	getPort(service2);
	SOCKET	sock2 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	sin.sin_port = htons((u_short)atoi(service2));
	connect(sock2, (struct sockaddr *)&sin, sizeof(sin));

	//下载文件
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
    closesocket(sock);                             // 关闭监听套接字
    closesocket(sock2);
    WSACleanup();                                  // 卸载winsock library
    
    printf("按回车键继续...");
	getchar();										// 等待任意按键
}

