#include <stdio.h>

#define MAX_OX 800
#define MAX_OY 1600
#define MAX_XN 10
#define MAX_YN 10

#define AddSize 1

int main(){
    int Memory[MAX_OX/MAX_XN]={0},size;
    FILE *fin = fopen("0x1.ChipDale","rb");
    FILE *fout = fopen("New0x1.ChipDale","wb");
    int j,k;
    for(k=0;k<AddSize;k++){
        for(j=0;j<MAX_OY/MAX_YN;j++){
            fwrite(Memory,MAX_OX/MAX_XN,sizeof(int),fout);  
        }
    }
    fseek(fin,0,SEEK_END);
    size = ftell(fin)/(MAX_OX/MAX_XN)/(MAX_OY/MAX_YN)/sizeof(int);
    fseek(fin,0,SEEK_SET);
    for(k=0;k<size;k++){
        for(j=0;j<MAX_OY/MAX_YN;j++){
            fread(Memory,MAX_OX/MAX_XN,sizeof(int),fin);  
            fwrite(Memory,MAX_OX/MAX_XN,sizeof(int),fout);  
        }  
    }
    fclose(fin);
    fclose(fout);
    system("pause");
    return 0;    
}
