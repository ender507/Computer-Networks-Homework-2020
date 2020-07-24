#include<stdio.h>
#include<string.h>
#include<time.h>

#define USER_NAME_LEN 20
#define EMAIL_LEN 50
typedef unsigned long DWORD;

struct Person {
   char username[USER_NAME_LEN];      // Ա����
   int level;                         // ���ʼ���
   char email[EMAIL_LEN];             // email��ַ
   DWORD sendtime;                    // ����ʱ��
   time_t regtime;                    // ע��ʱ��
   //���캯��
   Person(char name[],int l,char mail[]){
   		//������Ŀ�ĸ�ֵ 
		strcpy(username,name);
		level = l;
		strcpy(email,mail);
		//����ʱ��
		time_t now;
      	time(&now); 
      	sendtime = (DWORD)now;
		regtime = now;
   } 
}; 

int main(){
	FILE* pFile = fopen("Persons.stru","wb");
	
	if(pFile == NULL){
		printf("cannot open the file!\n");
		getchar();
		return 0;
	}
	
	while(1){
		//�������� 
		char name[USER_NAME_LEN];
		printf("name:");
		scanf("%s",name);
		//������exit���˳����� 
		if(strcmp(name,"exit\0")==0){
			fclose(pFile); 
			return 0;
		}
		//���� ����
		int l;
		printf("level:");
		scanf("%d",&l);
		//�������� 
		char mail[EMAIL_LEN];
		printf("e-mail:");
		scanf("%s",mail);
		//����Person��д���ļ� 
		struct Person tmp(name, l, mail);
		if (fwrite(&tmp, sizeof(Person), 1, pFile) != 1) {
			printf("file write error!\n");
		}
		printf("\n");
	}
}
