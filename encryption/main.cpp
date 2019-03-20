#include<stdio.h>
#include <stdlib.h>
#include<iostream>
#include<string.h>
#include <ctime>
using namespace std;


//
int merge(char *infile1,char *infile2)
{
    FILE * rfid1 = fopen(infile1,"rb");
    FILE * rfid2 = fopen(infile2,"rb");
    FILE * wfid = fopen("./SV7.1_448.model","wb");

    if((rfid1 == NULL)||(rfid2 == NULL))
    {
        cout<<"read file error"<<endl;
        return 0;
    }

    fseek(rfid1,0,SEEK_END);
    int num1=ftell(rfid1);
    fseek(rfid1,0,SEEK_SET);
    char c;
    for(int i=0;i<num1;i++)
    {
        c = fgetc(rfid1);
        fputc(c^'x',wfid);
    }

    fseek(rfid2,0,SEEK_END);
    int num2=ftell(rfid2);
    fseek(rfid2,0,SEEK_SET);
    for(int j=0;j<num2;j++)
    {
        c = fgetc(rfid2);
        fputc(c^'x',wfid);
    }

    char snum1[20];
    sprintf(snum1,"%010d",num1);
    cout<<snum1<<endl;
    fputs(snum1,wfid);

    char snum2[20];
    sprintf(snum2,"%010d",num2);
    cout<<snum2<<endl;
    fputs(snum2,wfid);

    //fputs("yanglin",wfid);
    fclose(rfid1);
    fclose(rfid2);
    fclose(wfid);
    return 0;
}

FILE * separate(char *infile)
{
   char* buffer_p;
   char* buffer_b;
   size_t result;
   FILE * rfid = fopen(infile,"rb");
   FILE * wfid1 = fopen("./head-shoulder.param","wb+");
   FILE * wfid2 = fopen("./head-shoulder.bin","wb+");
   fseek(rfid,-20,SEEK_END);
   char s1[11],s2[11];
   fgets(s1,11,rfid);
   fgets(s2,11,rfid);
   cout<<s1<<endl;
   cout<<s2<<endl;
   int num1,num2;
   sscanf(s1,"%d",&num1);
   sscanf(s2,"%d",&num2);
   cout<<num1<<endl;
   cout<<num2<<endl;
   fseek(rfid,0,SEEK_SET);
   buffer_p = (char*)malloc(sizeof(char)*num1);
   buffer_b = (char*)malloc(sizeof(char)*num2);
   result = fread(buffer_p,1,num1,rfid);
   printf("Read buffer_p Done\n");
   if(result!=num1)
   {
       fputs ("Reading error",stderr); 
       exit(1);
   }
   for(int i=0;i<num1;i++)
   {
       buffer_p[i]=buffer_p[i]^'x';
       //cout<<buffer_p[i]<<endl;
   }
   //printf("Param Done\n");
   fwrite (buffer_p , sizeof(char), sizeof(char)*num1, wfid1);
   fclose(wfid1);



   fseek(rfid,num1,SEEK_SET);
   result = fread(buffer_b,1,num2,rfid);
   //printf("Read buffer_b Done\n");
   if(result!=num2)
   {
       fputs ("Reading error",stderr); 
       exit(2);
   }
   for(int i=0;i<num2;i++)
   {
       buffer_b[i]=buffer_b[i]^'x';
   }
   printf("Bin Done\n");
   fwrite (buffer_b , sizeof(char), sizeof(char)*num2, wfid2);
   fclose(wfid2);
   fclose(rfid);
   free(buffer_b);
   free(buffer_p);
   return wfid1;


//    char c;
//    for(int i=0;i<num1;i++)
//    {
//         c = getc(rfid);
//         putc(c^'x',wfid1);
//    }
//    for(int j=0;j<num2;j++)
//    {
//         c = getc(rfid);
//         putc(c^'x',wfid2);
//    }
//    fclose(rfid);
//    return wfid1;
}


int encryption(char *infile)
{
    FILE * rfid = fopen(infile,"rb");
    if(rfid == NULL)
    {
        cout<<"read file error"<<endl;
        return 0;
    }
    FILE * wfid = fopen("./detect.bin","wb");
    char c;
    while((c = fgetc(rfid)) != EOF)
    {
        fputc(c^'x',wfid);
    }
    fputs("yanglin",wfid);
    fclose(rfid);
    fclose(wfid);
    return 0;
}

int decryption(char *infile)
{
    FILE * rfid = fopen(infile,"rb");
    if(rfid == NULL)
    {
        cout<<"read file error"<<endl;
        return 0;
    }
    fseek(rfid,-7,SEEK_END);
    char s[8];
    fgets(s,8,rfid);
    if(0==strcmp(s,"yanglin"))
    {
        cout<<"Encrypted file"<<endl;
    }

    fseek(rfid,0,SEEK_SET);
    FILE * wfid = fopen("./detect.cfg","wb");
    char c;
    while((c = fgetc(rfid)) != EOF)
    {
        fputc(c^'x',wfid);
    }
    fseek(wfid,-7,SEEK_END);
    for(int i=0;i<7;i++)
    {
        fputc(' ',wfid);
    }
    fclose(rfid);
    fclose(wfid);
    return 0;
}



int main(void)
{
    clock_t start, end;
    start = clock();
    //encryption("../m.cfg");
    //decryption("./detect.bin");
    //merge("/home/cwj1994/Desktop/AC_gesture/build/gesture_classifier_quant.param","/home/cwj1994/Desktop/AC_gesture/build/gesture_classifier_quant.bin");
    merge("/Users/winnie/Desktop/SV7.1_448.param","/Users/winnie/Desktop/SV7.1_448.bin");
    //FILE *file = separate("/home/cwj1994/Desktop/encryption/SV4.model");
    //char a[10];
    //fseek(file,0,SEEK_SET);
    //fgets(a, 5, file);
    //cout<<a<<endl;
    end = clock();
    double time = double(end - start)/ CLOCKS_PER_SEC;
    printf("\t-Seperate Finish! Use Time: %f s\n",time);//debug only
    return 0;
}
