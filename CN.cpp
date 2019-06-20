#include<stdio.h>
#include<conio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#include <dos.h>
#include <iostream>
HANDLE Port;
FILE * handle, * tmp, *log;
char y[100];
char line[100];
char l2[10], l3[10], l4[10], l5[10];
float XCur, YCur, ZCur;
float C1,C2,C3;
int feedrate,feedratet;
char timeline[50];

void my_time(){
 dostime_t ct;
 _dos_gettime(&ct);
 sprintf(timeline," %d:%d.%d  -",ct.hour,ct.minute,ct.second);
}

BOOL InitCom(){
  COMMTIMEOUTS CommTimeouts={0xFFFFFFFF,0,0,0,1500};
  BOOL fSuccess;
  DCB dcb;
   Port = CreateFile("\\\\.\\COM5", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
   if (Port == INVALID_HANDLE_VALUE) {
      MessageBox(NULL, "Невозможно открыть последовательный порт", "Error", MB_OK);
      ExitProcess(1);
   }

 fSuccess = GetCommState(Port, &dcb);
 if (!fSuccess)
   {
   MessageBox(NULL, "Невозможно прочитать ДСБ", "Error", MB_OK);
      ExitProcess(1);
   }


   dcb.BaudRate = CBR_19200;
   dcb.ByteSize = 8;
   dcb.Parity = NOPARITY;
   dcb.StopBits = ONESTOPBIT;

   fSuccess = SetCommState(Port, &dcb);
 if (!fSuccess)
   {
   MessageBox(NULL, "Невозможно записать ДСБ", "Error", MB_OK);
      ExitProcess(1);
   }
  SetCommTimeouts(Port, &CommTimeouts);
  sleep(1);
  return true;
  }

BOOL WriteCom(char *mstr){
   unsigned long rea, i , j , k;
   for(i=0; i< 65000;i++){for(j=0;j<10;j++){k=i;};};
   WriteFile(Port,mstr,strlen(mstr), &rea , NULL) ;

   if (rea!=strlen(mstr)) return(false);
   return(true);
}
BOOL ReadCom(){
  int i,j;
  unsigned long rea;
  char R;
 i = 0;
 do {
    ReadFile(Port,&R,1, &rea,NULL);


    if (rea==0){
         //printf("%d\r",j);
         //sleep(1);
         continue;
    }
   // printf("%c_",R);
    y[i]= R;
    i++;
 } while (R!='\r');
 y[i]=0;
  return true;
};

changepoint(char *s){
  int i;
  i=0;
  while (s[i]!=0) {
     if (s[i]==',') s[i]='.';
     i++  ;
  }
}
int mc(int x){
if (x!=0) {return (2000000 / x +230);};
return 200 ;
}

bool analyse(){
 int N, count,j,k,l,m,n,o, K4;
 bool hh;
 float K1, K2, K3;
 char *l1="      ";
 int command,i;
 char coord1[15],coord2[15],coord3[15],coord4[15], line_[100],com[5]="G1";
 char *lt;
 return true;
}
bool PutCommand(){
  if (!WriteCom(line)){printf("%s", "error write");
                       return false;  };
  do {
  Sleep(700);
  if(ReadCom()) {
     fputs(y, log );};
              } while (strcmp(y,"Ok.\r")!=0)  ;
     fputs("\r\n",log);
return true;
}
void main(int argc, char *argv[]) {
 int i;
   long strN;
   char c;
   char s[10];
   char *filename="e1.tap";
   int dzz,sdzz;
   C1 = 0 ; C2 = 0; C3 = 0;
   sdzz = 0;
  if (InitCom()==false) return;

   if((tmp = fopen ( "cn.tmp", "r")) == NULL){
               printf("No  saved data\n");i=0;

               if (argc>1){filename=argv[1];} else {filename="sauna1.tap";};}
   else{
               fgets(line, 99, tmp);
               sscanf(line, "%s line %i ; X= %f Y= %f Z = %f F = %f", filename, &i, &C1, &C2, &C3, &feedrate);
               printf(line);
       //        printf("%35.35s",line);
              sprintf(line,"g09 x %f y %f z %f\r", C1, C2, C3);
              PutCommand();

          fclose(tmp);
          remove("cn.tmp") ;



     }

   if((handle = fopen ( filename, "r")) == NULL){
      printf("Error open file");
      return;}
   if((log = fopen ( "cn_log.txt", "w")) == NULL){
      printf("Error open log file");
      return;}
      printf("open ok \n");
   strN=0;
     my_time();
     fputs("Start: ",log);
     fputs(timeline,log);
     fputs(" File: ",log);
     fputs(filename,log);
     fputs(" \n",log);
     printf(timeline);

   if (i>0){
           for(strN=0;strN<i;strN++){
           fgets(line, 99, handle);
           if (feof(handle)!=0) break;  }
     }
    // sprintf(line,"m03 \n");
    // PutCommand();
     //sleep(1);
     while (true){
     fgets(line, 99, handle);
     line[strlen(line)-1]=0;
     if (feof(handle)!=0){ break;};
     strN++;
      printf("%d-%30s                               \r ",strN,line);
      sprintf(s,"%d  ",strN);
     fputs(s,log);
     fputs(line,log)  ;
     fputs("\t\t\t",log);
     if (analyse()==false){
       printf("No reaction\n");
       fputs("\n",log);
       continue;}
    // fputs(line,log);
     fputs("\n",log);
     //printf("%35s                                   ",line);
     PutCommand();
          if(kbhit()!=0){
         printf("Exit(E)/Reload(R)/Continue(C) after  line %i ? \n",strN);
         do{
          gets(s);
         }while (!(s[0] =='e' | s[0]=='r' | s[0]=='c'));
         if (s[0] =='e'){
            if((tmp = fopen ( "cn.tmp", "w")) == NULL){
               printf("Error saving data");
               printf("line %i ; X= %f Y= %f Z = %f\n",strN, C1, C2, C3);
               gets(s);
               return;}
            sprintf(line,"%s line %i ; X= %f Y= %f Z = %f F = %f\n",filename,strN, C1, C2, C3, feedrate);
            fputs(line,tmp);
            fclose(tmp) ;
            printf("Saving OK");
          gets(s);
            return;};
         if (s[0] =='r'){
           printf("Reloading ...");
           sprintf(line,"g01 x %f y %f z 1 r 450 \n",C1, C2, 1);
           PutCommand();
           sprintf(line,"g01 x 0 y 0 z 1 r 450 \n");
           PutCommand();
           //sprintf(line,"g09 x 0 y 0 z 1 r 450 \n");
           //PutCommand();
           gets(s);
           sprintf(line,"g01 x %f y %f z 1 r 450 \n",C1, C2, 1);
           PutCommand();
           sprintf(line,"g01 x %f y %f z %f r 450 \n",C1, C2, C3);
           PutCommand();

         }
   }
   }
   fclose(handle);
  my_time();
     fputs("Finish: ",log);
     fputs(timeline,log);
     fputs(" File: ",log);
     fputs(filename,log);
     fputs(" \n",log);
     printf(timeline);
   fclose(log);
  // sprintf(line,"m05 \n");
   //PutCommand();
   sprintf(line,"r \n");
   PutCommand();
   CloseHandle(Port);
  }

