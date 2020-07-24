#include "P2PChatFunc.hpp"				//���пͻ��˺ͷ��������õõ��ĺ��� 

#define WSVERS		MAKEWORD(2, 0)        // ָ���汾2.0 
#pragma comment(lib,"ws2_32.lib")         // ʹ��winsock 2.0 Llibrary

int main(int argc, char *argv[]){
	char	*host = "127.0.0.1";	    /* server IP to connect         */
	char	*service = "50500";  	    /* server port to connect       */
	struct sockaddr_in sin;	            /* an Internet endpoint address	*/
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
	hThread = (HANDLE)_beginthreadex(NULL, 0, &reciveMes, (void*)&sock, 0, NULL); 
	
	cmd(sock);			//����ָ�� 
	
	CloseHandle(hThread);							//�ر����߳� 
	shutdown(sock,SD_BOTH);
    closesocket(sock);                             // �رռ����׽���
    WSACleanup();                                  // ж��winsock library
    
    printf("���س�������...");
	getchar();										// �ȴ����ⰴ��
}

