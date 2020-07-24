#include<stdio.h>
#include<string.h>
#include<time.h>

#define USER_NAME_LEN 20
#define EMAIL_LEN 50
typedef unsigned long DWORD;

struct Person {
   char username[USER_NAME_LEN];      // 员工名
   int level;                         // 工资级别
   char email[EMAIL_LEN];             // email地址
   DWORD sendtime;                    // 发送时间
   time_t regtime;                    // 注册时间
   //构造函数
   Person(char name[],int l,char mail[]){
   		//三个项目的赋值 
		strcpy(username,name);
		level = l;
		strcpy(email,mail);
		//计算时间
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
		//输入姓名 
		char name[USER_NAME_LEN];
		printf("name:");
		scanf("%s",name);
		//若输入exit则退出程序 
		if(strcmp(name,"exit\0")==0){
			fclose(pFile); 
			return 0;
		}
		//输入 级别
		int l;
		printf("level:");
		scanf("%d",&l);
		//输入邮箱 
		char mail[EMAIL_LEN];
		printf("e-mail:");
		scanf("%s",mail);
		//创建Person并写入文件 
		struct Person tmp(name, l, mail);
		if (fwrite(&tmp, sizeof(Person), 1, pFile) != 1) {
			printf("file write error!\n");
		}
		printf("\n");
	}
}
