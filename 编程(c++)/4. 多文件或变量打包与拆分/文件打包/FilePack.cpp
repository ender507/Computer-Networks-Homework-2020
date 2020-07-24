#include<stdio.h>
#include<string.h>

struct FileStruct{
    char fileName[300];
    __int64 fileSize;
    FileStruct(char name[], __int64 size){
    	strcpy(fileName,name);
    	fileSize = size;
	}
};

__int64 getFileSize(FILE* fp){
	fseek(fp, 0, SEEK_END);			//�ҵ��ļ�β 
	__int64 size = ftell(fp);		//�ļ�βλ�ü�Ϊ�ļ�ͷ 
	fseek(fp,0,SEEK_SET);			//���ļ�ָ�뻹ԭΪָ���ļ�ͷ
	return size; 
}
void getFileName(char* pathName, char fileName[]){
	int pos = strlen(pathName) - 1;
	//�Ӻ���ǰ�ҵ�һ����Ϊ'\\'��λ��
	//��pos = -1˵��Ϊ���·�� 
	while(pos!=-1&&pathName[pos]!='\\')pos--; 
	pos++;							//posΪ�ļ�����ʼλ�� 
	int i = 0;
	while(pos!=strlen(pathName)){
		fileName[i++] = pathName[pos++];
	}
}
bool packFile(char *srcFile, FILE* &fp){
	FILE *srcfp = fopen(srcFile,"rb");
	if(srcfp == NULL)return false;				//�ļ������� 
	char fileName[300];
	getFileName(srcFile,fileName);
	//�Ƚ����ļ���Ϣ 
	struct FileStruct filestrc(fileName, getFileSize(srcfp));
	//���ļ���Ϣд�� 
	if(fwrite(&filestrc, sizeof(struct FileStruct), 1, fp)!=1){
		fclose(srcfp);
		return false;
	}
	//д���ļ����� 
	char buf[20];
	int len = 0;
	while ((len = fread(buf, 1, 20, srcfp)) >= 20) { 
		fwrite(buf, 1, 20, fp);
	}
	fwrite(buf, 1, len, fp);
	fclose(srcfp);
	return true;
}

int main(){
	char objFile[300];
	printf("������Ŀ���ļ�������·����:");
	scanf("%s",objFile);
	FILE *fp = fopen(objFile,"wb");
	if(fp == NULL){
		printf("cannot open the file!\n");
		getchar();
		return 0; 
	}
	int num = 1;
	while(1){
		char srcFile[300];
		printf("������Ҫ������ļ�#%d����·����:",num++);
		scanf("%s",srcFile);
		if(strcmp(srcFile,"exit\0")==0)break;
		if(!packFile(srcFile,fp)){
			printf("write failed!\n");
			num--;
		}
	}
	fclose(fp);
	return 0;
}
