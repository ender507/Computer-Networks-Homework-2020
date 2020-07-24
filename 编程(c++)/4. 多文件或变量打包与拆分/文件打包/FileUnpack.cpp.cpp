#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<algorithm>
#define LEN 100
#define BUFLEN 3000000
char buf[BUFLEN];

struct FileStruct{
    char fileName[300];
    __int64 fileSize;
};

void num2str(char buf[],int i){
	memset(buf,'\0',LEN);
	int pos = 0;
	while(i){
		buf[pos++] = '0'+ i%10;
		i/=10;
	}
	std::reverse(buf,buf+strlen(buf));
}

void findName(char path[], char name[]){
	int i = 0;	//��¼��ǰ·�����ж��������ļ� 
	char firstName[LEN],lastName[LEN];		//�ֱ��¼�ļ��������ļ���׺��(�ָ��ĵ�����׺��)
	bool flag = true;
	for(int pos=0; pos<strlen(name); pos++){
		if(name[pos]=='.'){
			flag = false;
			i = 0;
		}
		if(flag)firstName[i++] = name[pos];
		else lastName[i++] = name[pos];
	}
	char path_name[LEN];
	strcpy(path_name,path);
	strcat(path_name,name);
	//������ֻ���������ļ���ʧ����˵��û������ 
	FILE* fp = fopen(path_name,"rb");
	if(fp == NULL)return;
	i = 1;
	while(1){
		memset(path_name,'\0',LEN);
		strcpy(path_name,path);
		strcat(path_name,firstName);
		char buf[LEN];
		num2str(buf, i);
		//���ļ�����(i) 
		strcat(path_name,"(");
		strcat(path_name,buf);
		strcat(path_name,")");
		strcat(path_name,lastName);
		fp = fopen(path_name,"rb");
		if(fp == NULL){
			memset(name,'\0',LEN);
			strcat(name,firstName);
			strcat(name,"(");
			strcat(name,buf);
			strcat(name,")");
			strcat(name,lastName);
			return;
		}
		i++;
	}
}

bool unpack(char path[], char name[], __int64 size, FILE* &fp){
	memset(buf, '\0', BUFLEN);
	if(path[0]!='\0')strcat(path,"\\");		//path[0]=='\0'��ʾ��ǰ�ļ��У����ü�'\\' 
	//�ҵ���ȷ���ļ����������ظ��ļ����� 
	findName(path,name);
	strcat(path,name);						//·��+�ļ������γ�����·�� 
	FILE* outfp = fopen(path,"wb");			//������ѹ�ļ� 
	if(outfp == NULL)return false;
	//�����������ݲ�д���ѹ�ļ� 
	if(fread(buf, size, 1, fp)==0)return false;
	if(fwrite(buf,size,1,outfp)==0)return false;
	return true;
}

int main(){
	char path[LEN],originpath[LEN];
	printf("�������ѹ·��:");
	scanf("%s",path);
	strcpy(originpath,path);
	char file[LEN];
	printf("��������Ҫ��ѹ���ļ�:");
	scanf("%s",file);
	FILE* fp = fopen(file,"rb");
	if(fp == NULL){
		printf("cannot open the file!\n");
		getchar();
		return 0; 
	}
	while(!feof(fp)){
		struct FileStruct fsBuf;
		strcpy(path,originpath);
		if(fread(&fsBuf, sizeof(struct FileStruct), 1, fp)==0){
			fclose(fp);
			return 0;
		}
		if(!unpack(path, fsBuf.fileName, fsBuf.fileSize, fp)){
			printf("Unpack Error!\n");
			fclose(fp);
			return 0;
		}
	}
}
