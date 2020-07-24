#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <process.h>
#include "conio.h"

#define	WSVERS	MAKEWORD(2, 0)
#define BUFLEN 2000

#pragma comment(lib,"ws2_32.lib")

HANDLE hThread[5];		//���ɽ���5���ͻ���
SOCKET ssocks[5];		//5��ssock���ֱ��Ӧ����ͻ��� 
struct sockNum{
	SOCKET sock;
	int num;
};
void sendToAll(char *buf){
	for(int i=0; i<5; i++){
		if(hThread[i] != NULL){	
	       	send(ssocks[i], buf, strlen(buf), 0);                // �ѻ�����(ָ��,����)�����ݷ��ͳ�ȥ
		}
	}
}
unsigned __stdcall recive_send(void *socknum){
	char buf[BUFLEN+1];
	SOCKET sock = ((sockNum*)socknum)->sock;
	int num = ((sockNum*)socknum)->num;
	int cc;
	while(1){
		cc = recv(sock, buf, BUFLEN, 0);            // ccΪ���յ����ַ��ĸ���(>0)��Է��ѹر�(=0)�����ӳ���(<0)
  	  	if(cc == SOCKET_ERROR){
			closesocket(sock);                         // �رռ����׽���
   			ssocks[num] = NULL;
    		return 0;
		}
    	else if(cc > 0) {
	        buf[cc] = '\0';	                     		// ensure null-termination
	        printf("�յ���Ϣ��\n%s\n",buf);                         // ��ʾ�����յ��ַ���
	        sendToAll(buf);
	    }
	}
	return 0;
}

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

	listen(msock, 5);                                   // �ȴ��������ӵĶ��г���Ϊ5
	for(int i=0; i<5; i++){
		ssocks[i] = NULL;
		hThread[i] = NULL;
	}
	
    while(!_kbhit()){ 		                             // ����Ƿ��а���
       	alen = sizeof(struct sockaddr);                   // ȡ����ַ�ṹ�ĳ���
		ssock = accept(msock, (struct sockaddr *)&fsin, &alen); // ������µ��������󣬷��������׽��֣����򣬱�������fsin�����ͻ���IP��ַ�Ͷ˿ں�
		if(ssock == NULL)continue;
		for(int i=0; i<5; i++){
	    	if(ssocks[i] == NULL){//��ĳ���߳̿��У�����ǰ�ͻ��� 
	    		if(hThread[i] != NULL)CloseHandle(hThread[i]);
				ssocks[i] = ssock;
	    		struct sockNum tmp;
	    		tmp.sock = ssock;
				tmp.num = i;
	    		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, &recive_send, (void*)&tmp, 0, NULL); 
				break;
			}
			if(i == 4){
				printf("�ͻ����Ѵﵽ������ޣ�\n");
			}
		}
    }
	closesocket(msock);                                 // �رռ����׽���
    WSACleanup();                                             // ж��winsock library
    
}


