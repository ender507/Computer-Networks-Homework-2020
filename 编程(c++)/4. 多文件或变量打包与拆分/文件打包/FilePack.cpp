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
	fseek(fp, 0, SEEK_END);			//找到文件尾 
	__int64 size = ftell(fp);		//文件尾位置即为文件头 
	fseek(fp,0,SEEK_SET);			//将文件指针还原为指向文件头
	return size; 
}
void getFileName(char* pathName, char fileName[]){
	int pos = strlen(pathName) - 1;
	//从后往前找第一个不为'\\'的位置
	//若pos = -1说明为相对路径 
	while(pos!=-1&&pathName[pos]!='\\')pos--; 
	pos++;							//pos为文件名起始位置 
	int i = 0;
	while(pos!=strlen(pathName)){
		fileName[i++] = pathName[pos++];
	}
}
bool packFile(char *srcFile, FILE* &fp){
	FILE *srcfp = fopen(srcFile,"rb");
	if(srcfp == NULL)return false;				//文件不存在 
	char fileName[300];
	getFileName(srcFile,fileName);
	//先建立文件信息 
	struct FileStruct filestrc(fileName, getFileSize(srcfp));
	//将文件信息写入 
	if(fwrite(&filestrc, sizeof(struct FileStruct), 1, fp)!=1){
		fclose(srcfp);
		return false;
	}
	//写入文件数据 
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
	printf("请输入目标文件名（含路径）:");
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
		printf("请输入要打包的文件#%d（含路径）:",num++);
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
