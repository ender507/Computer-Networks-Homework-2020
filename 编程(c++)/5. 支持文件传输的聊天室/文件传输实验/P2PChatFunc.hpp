#ifndef P2PChatFunc_hpp
#define P2PChatFunc_hpp

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h> 
#include <algorithm>
#define	BUFLEN		200000               // ��������С

char	PATH[BUFLEN+1];					//�ļ��洢·��
char	BUF[BUFLEN+1];					//�����ָ�� 

void findName(char path[], char name[]){
	int i = 0;	//��¼��ǰ·�����ж��������ļ� 
	char firstName[100],lastName[100];		//�ֱ��¼�ļ��������ļ���׺��(�ָ��ĵ�����׺��)
	memset(firstName, '\0', 100);
	memset(lastName, '\0', 100);
	bool flag = true;
	for(int pos=0; pos<strlen(name); pos++){
		if(name[pos]=='.'){
			flag = false;
			i = 0;
		}
		if(flag)firstName[i++] = name[pos];
		else lastName[i++] = name[pos];
	}
	char path_name[100];
	memset(path_name, '\0', 100);
	strcpy(path_name,path);
	strcat(path_name,name);
	//������ֻ���������ļ���ʧ����˵��û������ 
	FILE* fp = fopen(path_name,"rb");
	if(fp == NULL)return;
	i = 2;
	while(1){
		memset(path_name,'\0',100);
		strcpy(path_name,path);
		strcat(path_name,firstName);
		char buf[100];
		memset(buf, '\0', 100);
		itoa(i, buf, 10);
		//���ļ�����(i) 
		strcat(path_name,"(");
		strcat(path_name,buf);
		strcat(path_name,")");
		strcat(path_name,lastName);
		fp = fopen(path_name,"rb");
		if(fp == NULL){
			memset(name,'\0',100);
			strcat(name,firstName);
			strcat(name,"(");
			strcat(name,buf);
			strcat(name,")");
			strcat(name,lastName);
			return;
		}
		fclose(fp);
		i++;
	}
}

__int64 getFileSize(FILE* fp){
	fseek(fp, 0, SEEK_END);			//�ҵ��ļ�β 
	__int64 size = ftell(fp);		//�ļ�βλ�ü�Ϊ�ļ�ͷ 
	fseek(fp,0,SEEK_SET);			//���ļ�ָ�뻹ԭΪָ���ļ�ͷ
	return size; 
}

unsigned __stdcall reciveMes(void *sockaddr){
	char mes[BUFLEN+1];
	SOCKET sock = *((SOCKET*)sockaddr);
	int cc;
	printf(">>");
	while(1){
		cc = recv(sock, mes, 1, 0);
		if(cc == SOCKET_ERROR || cc==0)continue;
		else{
			//�յ�������Ϣ 
			if(mes[0]=='a'){
				memset(mes, '\0', BUFLEN+1);
				//�����ַ������� 
				recv(sock, mes, BUFLEN, 0);
				int len = atoi(mes);
				//�����ı�����ӡ 
				memset(mes, '\0', BUFLEN+1);
				recv(sock, mes, len, 0);
				printf("\n�յ���Ϣ:%s\n>>",mes);
			}
			//�յ��ļ� 
			else{
				memset(mes, '\0', BUFLEN+1);
				//�����ļ���С���ļ��� 
				recv(sock, mes, BUFLEN, 0);
				__int64 size = atoi(mes);		//��Ϊ�ļ���С���ļ�����'\0'���������ֱ��ת���ļ���С 
				char file[BUFLEN+1];
				memset(file, '\0', BUFLEN+1);
				strcpy(file, PATH);
				char name[BUFLEN+1];
				memset(name, '\0', BUFLEN+1);
				int pos = 0;
				for(int i=strlen(mes)+1; mes[i]!='\0'; i++)name[pos++] = mes[i];
				findName(file, name);
				strcat(file, name);
				memset(mes, '\0', BUFLEN+1);
				recv(sock, mes, size, 0);
				printf("�յ���СΪ%d���ļ�%s\n>>",size, file);
				FILE* fp = fopen(file, "wb");
				if(fp==NULL){
					printf("�����ļ�ʧ��\n");
					return 0;
				}
				fwrite(mes, 1, size, fp);
				fclose(fp);
			}
		}
	}
	return 0;
}

void sendChatMes(SOCKET sock){
	send(sock, "a", 1, 0);					//����'a'��ʾ���͵����ı�
	char bufstr[BUFLEN+1];
	itoa(strlen(BUF)+1, bufstr, 10);
	send(sock, bufstr, BUFLEN, 0);	//�����ַ������ȣ�strlen(buf+1)��Ϊ�˽�����'\0'Ҳ���ͳ�ȥ 
	send(sock, BUF, strlen(BUF)+1, 0);		//�����ַ������� 
}

void sendFile(SOCKET sock){
	char file[BUFLEN+1];
	memset(file, '\0', BUFLEN+1);
	//����ļ�·�� 
	int pos = 0;
	for(int i=5; BUF[i]!='\0'; i++){
		if(BUF[i]==' ')continue;
		file[pos++] = BUF[i];
	}
	FILE* fp = fopen(file, "rb");
	if(fp==NULL){
		printf("���ļ�ʧ��\n");
		return;
	}
	char sizeAndName[BUFLEN+1];				//��¼�ļ���С���ļ��������У���С���ļ�����һ��'\0'�ָ�
	memset(sizeAndName, '\0', BUFLEN+1);
	__int64 size = getFileSize(fp);
	itoa(size, sizeAndName, 10);
	pos =  0;
	char name[BUFLEN+1];
	memset(name, '\0', BUFLEN+1);
	for(int i=strlen(BUF)-1; BUF[i]!=' '&&BUF[i]!='\\'; i--)name[pos++] = BUF[i];
	std::reverse(name, name+strlen(name));
	pos = strlen(sizeAndName)+1;
	for(int i=0; i!= strlen(name); i++)sizeAndName[pos++] = name[i];
	sizeAndName[pos++] = '\0';
	//�����ļ� 
	send(sock, "i", 1, 0);					//����'i'��ʾ���͵��Ƕ������ļ�
	send(sock, sizeAndName, BUFLEN, 0);
	fread(BUF, 1, size, fp);
	send(sock, BUF, size, 0);
	fclose(fp);
}

void cmd(SOCKET sock){
	while(1){	
		memset(BUF, '\0', BUFLEN+1);
		gets(BUF);
		//����quit�˳�
		if(BUF[0]=='q'&&BUF[1]=='u'&&BUF[2]=='i'&&BUF[3]=='t'&&BUF[4]=='\0')break;
		//rdir�ı��ļ�����·�� 
		else if(BUF[0]=='r'&&BUF[1]=='d'&&BUF[2]=='i'&&BUF[3]=='r'&&BUF[4]==' '){
			memset(PATH, '\0', BUFLEN+1);
			int pos = 0;
			for(int i=5; BUF[i]!='\0'; i++){
				if(BUF[i]==' ')continue;
				PATH[pos++] = BUF[i];
			}
			printf("�ļ�����·������Ϊ:%s\n",PATH);
			strcat(PATH, "\\");
		}
		//send�����ļ� 
		else if(BUF[0]=='s'&&BUF[1]=='e'&&BUF[2]=='n'&&BUF[3]=='d'&&BUF[4]==' ') sendFile(sock);
		//����������Ϣ 
		else sendChatMes(sock);
		printf(">>");
	}
}

#endif
