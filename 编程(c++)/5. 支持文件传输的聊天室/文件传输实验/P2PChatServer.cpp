#include "P2PChatFunc.hpp"				//���пͻ��˺ͷ��������õõ��ĺ��� 

#define	WSVERS	MAKEWORD(2, 0)

#pragma comment(lib,"ws2_32.lib")

int main(int argc, char *argv[]) {
	struct	sockaddr_in fsin;	    /* the from address of a client	  */
	SOCKET	msock, ssock;		    /* master & slave sockets	      */
	WSADATA wsadata; 
	char	*service = "50500";
	struct  sockaddr_in sin;	    /* an Internet endpoint address		*/
    int	    alen;			        /* from-address length		        */

	WSAStartup(WSVERS, &wsadata);						// ����winsock library��WSVERSָ������ʹ�õİ汾��wsadata����ϵͳʵ��֧�ֵ���߰汾
	msock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);	// �����׽��֣�������������Э���(family)�����׽��֣�TCPЭ��
														// ���أ�Ҫ�����׽��ֵ���������INVALID_SOCKET

	memset(&sin, 0, sizeof(sin));						// ��&sin��ʼ�ĳ���Ϊsizeof(sin)���ڴ���0
	sin.sin_family = AF_INET;							// ��������ַ��(INET-Internet)
	sin.sin_addr.s_addr = INADDR_ANY;					// ��������(�ӿڵ�)IP��ַ��
	sin.sin_port = htons((u_short)atoi(service));		// �����Ķ˿ںš�atoi--��asciiת��Ϊint��htons--������������(16λ)
	bind(msock, (struct sockaddr *)&sin, sizeof(sin));  // �󶨼�����IP��ַ�Ͷ˿ں�

	listen(msock, 5);                                   // һ��һ������ȴ��������ӵĶ��г���Ϊ1
	alen = sizeof(struct sockaddr);
	ssock = accept(msock, (struct sockaddr *)&fsin, &alen);

	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &reciveMes, (void*)&ssock, 0, NULL); 
	
	cmd(ssock);			//����ָ�� 
	
	CloseHandle(hThread);
	closesocket(msock);                                 // �رռ����׽���
    WSACleanup();                                       // ж��winsock library
}
