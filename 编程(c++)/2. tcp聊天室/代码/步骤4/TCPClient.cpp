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
	printf(">>");
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
	        printf("\n�յ���Ϣ��\n%s\n",buf);                         // ��ʾ�����յ��ַ���
	        printf(">>");
	    }
	}
	return 0;
}


int main(int argc, char *argv[]){
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
	char	buf[BUFLEN+1];   		        /* buffer for one line of text	*/
	SOCKET	sock;		  	            /* socket descriptor	    	*/

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
	while(1){	
		memset(buf,'\0',BUFLEN);
		gets(buf);
		if(buf[0]=='e'&&buf[1]=='x'&&buf[2]=='i'&&buf[3]=='t'&&buf[4]=='\0')break;//����exit�˳�
		send(sock, buf, BUFLEN, 0);
	}
	CloseHandle(hThread);							//�ر����߳� 
    closesocket(sock);                             // �رռ����׽���
    WSACleanup();                                  // ж��winsock library
    
    printf("���س�������...");
	getchar();										// �ȴ����ⰴ��
}

