#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <time.h>
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
	struct	sockaddr_in fsin;
};

void sendToAll(char *buf){
	for(int i=0; i<5; i++){
		if(ssocks[i] != NULL){	
	       	send(ssocks[i], buf, strlen(buf), 0);                // �ѻ�����(ָ��,����)�����ݷ��ͳ�ȥ
		}
	}
}

unsigned __stdcall recive_send(void *socknum){
	char buf[BUFLEN+1];
	SOCKET sock = ((sockNum*)socknum)->sock;
	int num = ((sockNum*)socknum)->num;
	struct	sockaddr_in fsin = ((sockNum*)socknum)->fsin;
	int cc;
	while(1){
		cc = recv(sock, buf, BUFLEN, 0);            // ccΪ���յ����ַ��ĸ���(>0)��Է��ѹر�(=0)�����ӳ���(<0)
  	  	if(cc == SOCKET_ERROR){						//SOCKET_ERROR˵���ͻ����Ѿ��ر�
			closesocket(sock);                         // �رռ����׽���
   			ssocks[num] = NULL;
   			time_t	now;
      	 	time(&now);                                      // ȡ��ϵͳʱ��
       		char *pts = ctime(&now);                                      // ��ʱ��ת��Ϊ�ַ���
			char tmp[BUFLEN+1];
			strcat(tmp,"A Client Exit!\nʱ��:");
			strcat(tmp,pts);
			strcat(tmp,"ip��ַ:");
			strcat(tmp,inet_ntoa(fsin.sin_addr));
			strcat(tmp,"\n�˿ں�:");
			char tmp2[BUFLEN+1];
			itoa((u_short)ntohs(fsin.sin_port),tmp2,10);
			strcat(tmp,tmp2);
			printf("%s\n\n",tmp);
	        sendToAll(tmp);
	        memset(tmp,'\0',BUFLEN);
    		return 0;
		}
    	else if(cc > 0) {
    		time_t	now;
      	 	time(&now);                                      // ȡ��ϵͳʱ��
       		char *pts = ctime(&now);                                      // ��ʱ��ת��Ϊ�ַ���
			char tmp[BUFLEN+1];
			strcat(tmp,"ʱ��:");
			strcat(tmp,pts);
			strcat(tmp,"ip��ַ:");
			strcat(tmp,inet_ntoa(fsin.sin_addr));
			strcat(tmp,"\n�˿ں�:");
			char tmp2[BUFLEN+1];
			itoa((u_short)ntohs(fsin.sin_port),tmp2,10);
			strcat(tmp,tmp2);
			strcat(tmp,"\n��Ϣ:\n");
			strcat(tmp,buf);
			printf("%s\n\n",tmp);
	        sendToAll(tmp);
	        memset(tmp,'\0',BUFLEN);
	    }
	}
	return 0;
}

void enter(struct sockaddr_in fsin){
	char tmp[BUFLEN+1];
	time_t	now;
 	time(&now);                                      // ȡ��ϵͳʱ��
	char *pts = ctime(&now);                                      // ��ʱ��ת��Ϊ�ַ���
	strcat(tmp,"New Client entered!\nʱ��:");
	strcat(tmp,pts);
	strcat(tmp,"ip��ַ:");
	strcat(tmp,inet_ntoa(fsin.sin_addr));
	strcat(tmp,"\n�˿ں�:");
	char tmp2[BUFLEN+1];
	itoa((u_short)ntohs(fsin.sin_port),tmp2,10);
	strcat(tmp,tmp2);
	strcat(tmp,"\n");
	printf("%s\n\n",tmp);
    sendToAll(tmp);
    memset(tmp,'\0',BUFLEN);
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
				tmp.fsin = fsin;
				enter(fsin);
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

