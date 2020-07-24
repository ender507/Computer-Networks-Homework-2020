#ifndef P2PChatFunc_hpp
#define P2PChatFunc_hpp

#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <process.h> 
#include <algorithm>
#define	BUFLEN		200000               // 缓冲区大小

char	PATH[BUFLEN+1];					//文件存储路径
char	BUF[BUFLEN+1];					//输入的指令 

void findName(char path[], char name[]){
	int i = 0;	//记录当前路径下有多少重名文件 
	char firstName[100],lastName[100];		//分别记录文件本名和文件后缀名(分隔的点加入后缀名)
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
	//尝试以只读方法读文件，失败则说明没有重名 
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
		//给文件加上(i) 
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
	fseek(fp, 0, SEEK_END);			//找到文件尾 
	__int64 size = ftell(fp);		//文件尾位置即为文件头 
	fseek(fp,0,SEEK_SET);			//将文件指针还原为指向文件头
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
			//收到聊天消息 
			if(mes[0]=='a'){
				memset(mes, '\0', BUFLEN+1);
				//接收字符串长度 
				recv(sock, mes, BUFLEN, 0);
				int len = atoi(mes);
				//接收文本并打印 
				memset(mes, '\0', BUFLEN+1);
				recv(sock, mes, len, 0);
				printf("\n收到消息:%s\n>>",mes);
			}
			//收到文件 
			else{
				memset(mes, '\0', BUFLEN+1);
				//接收文件大小和文件名 
				recv(sock, mes, BUFLEN, 0);
				__int64 size = atoi(mes);		//因为文件大小和文件名有'\0'间隔，可以直接转换文件大小 
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
				printf("收到大小为%d的文件%s\n>>",size, file);
				FILE* fp = fopen(file, "wb");
				if(fp==NULL){
					printf("保存文件失败\n");
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
	send(sock, "a", 1, 0);					//发送'a'表示发送的是文本
	char bufstr[BUFLEN+1];
	itoa(strlen(BUF)+1, bufstr, 10);
	send(sock, bufstr, BUFLEN, 0);	//发送字符串长度，strlen(buf+1)是为了将最后的'\0'也发送出去 
	send(sock, BUF, strlen(BUF)+1, 0);		//发送字符串本身 
}

void sendFile(SOCKET sock){
	char file[BUFLEN+1];
	memset(file, '\0', BUFLEN+1);
	//获得文件路径 
	int pos = 0;
	for(int i=5; BUF[i]!='\0'; i++){
		if(BUF[i]==' ')continue;
		file[pos++] = BUF[i];
	}
	FILE* fp = fopen(file, "rb");
	if(fp==NULL){
		printf("打开文件失败\n");
		return;
	}
	char sizeAndName[BUFLEN+1];				//记录文件大小和文件名，其中，大小和文件名用一个'\0'分隔
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
	//发送文件 
	send(sock, "i", 1, 0);					//发送'i'表示发送的是二进制文件
	send(sock, sizeAndName, BUFLEN, 0);
	fread(BUF, 1, size, fp);
	send(sock, BUF, size, 0);
	fclose(fp);
}

void cmd(SOCKET sock){
	while(1){	
		memset(BUF, '\0', BUFLEN+1);
		gets(BUF);
		//输入quit退出
		if(BUF[0]=='q'&&BUF[1]=='u'&&BUF[2]=='i'&&BUF[3]=='t'&&BUF[4]=='\0')break;
		//rdir改变文件保存路径 
		else if(BUF[0]=='r'&&BUF[1]=='d'&&BUF[2]=='i'&&BUF[3]=='r'&&BUF[4]==' '){
			memset(PATH, '\0', BUFLEN+1);
			int pos = 0;
			for(int i=5; BUF[i]!='\0'; i++){
				if(BUF[i]==' ')continue;
				PATH[pos++] = BUF[i];
			}
			printf("文件保存路径更改为:%s\n",PATH);
			strcat(PATH, "\\");
		}
		//send发送文件 
		else if(BUF[0]=='s'&&BUF[1]=='e'&&BUF[2]=='n'&&BUF[3]=='d'&&BUF[4]==' ') sendFile(sock);
		//发送聊天消息 
		else sendChatMes(sock);
		printf(">>");
	}
}

#endif
