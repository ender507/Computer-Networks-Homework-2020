#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h> 

#define	BUFLEN		2000                  // ��������С
#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

unsigned __stdcall recive_print(void *sockaddr){
	char buf[BUFLEN+1];
	SOCKET sock = *((SOCKET*)sockaddr);
	int cc;
	while(1){
		cc = recv(sock, buf, BUFLEN, 0);            // ccΪ���յ����ַ��ĸ���(>0)��Է��ѹر�(=0)�����ӳ���(<0)
  	  	if(cc == SOCKET_ERROR){
  	  		int error = GetLastError();
  	  		if(error==10053)break;				//������Ȼ�˳��������б��� 
  	  		printf("Error: %d.\n",error);     	//����������ر��׽���sock��
			closesocket(sock);                          // �رռ����׽���
   			WSACleanup();                              // ж��winsock library
			getchar();
    		exit(0);
		}
    	else if(cc > 0) {
	        buf[cc] = '\0';	                     		// ensure null-termination
	        printf("\n%s\n",buf);       			    // ��ʾ�����յ��ַ���
	    }
	}
	return 0;
}

int main(int argc, char *argv[]){
	char	buf[BUFLEN+1];
	memset(buf, '\0', BUFLEN);
	printf("����ip��ַ:");
	scanf("%s",buf);
	char	host[15];						/* server IP to connect         */
	strcpy(host, buf);
	memset(buf, '\0', BUFLEN);
	printf("����˿ں�:");
	scanf("%s",buf);
	char	service[5];				    	/* server port to connect       */
	strcpy(service,buf);
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
	
	HANDLE hThread;									//���ڽ��պʹ�ӡ���������͵���Ϣ�����߳� 
	hThread = (HANDLE)_beginthreadex(NULL, 0, &recive_print, (void*)&sock, 0, NULL); 
	
	char sendMes[BUFLEN+1];
	memset(sendMes,'\0',BUFLEN);
	gets(buf);										//��ջ����� 
	printf("���뷢������(���һ����send��ʾ����)��\n");
	while(1){
		memset(buf,'\0',BUFLEN);
		gets(buf);
		if(buf[0]=='e'&&buf[1]=='x'&&buf[2]=='i'&&buf[3]=='t'&&buf[4]=='\0')break;//����exit�˳�
		if(buf[0]=='s'&&buf[1]=='e'&&buf[2]=='n'&&buf[3]=='d'&&buf[4]=='\0'){	//����send������Ϣ 
			send(sock, sendMes, strlen(sendMes), 0);
			memset(sendMes,'\0',BUFLEN+1);
			continue;
		}
		if(buf[0]=='\0'){				//������У����ϻس� 
			strcat(sendMes, "\r\n");
		}
		else{							//����һ���ַ���׷�ӻس� 
			strcat(sendMes, buf);
			strcat(sendMes, "\r\n");
		}
	}	
	CloseHandle(hThread);							//�ر����߳� 
	shutdown(sock,SD_BOTH);
    closesocket(sock);                             // �رռ����׽���
    WSACleanup();                                  // ж��winsock library
    
    printf("���س�������...");
	getchar();										// �ȴ����ⰴ��
}

