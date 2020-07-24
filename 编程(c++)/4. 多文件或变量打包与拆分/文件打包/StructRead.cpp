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
   
   void print(){
   		printf("name:%s\n",username);
   		printf("level:%d\n",level);
   		printf("e-mail:%s\n",email);
   		time_t tmp = (time_t)sendtime;
   		printf("send time:%s",ctime(&tmp));
   		printf("reg time:%s\n",ctime(&regtime));
   }
}; 

int main(){
	FILE* fp = fopen("Persons.stru","rb");
	
	if (fp == NULL) {
		getchar();
		printf("cannot open the file!\n");
		return 0;
	}
	
	struct Person tmp;
	while (fread(&tmp, sizeof(Person), 1, fp) == 1) {
		tmp.print();
	}
	printf("struct read finished!\n");
	fclose(fp);
}
