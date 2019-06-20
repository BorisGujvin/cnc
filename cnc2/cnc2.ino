#include <LiquidCrystal.h>
#include <SD.h>
#include <String.h>
#include <stdio.h>
#define shp 41
#define LCD_LIGHT 10
int popr=100;  // фигня - поправка - корректировка скорости
char* filepath = "123456789012345678901234567890";
char* tmpstr = "1234567890123456789012345678901234567890";

const int analogInPin = A8;
const int HH=650;
unsigned long sleep;
byte cc[3]={23,35,29},
     en[3]={25,37,31},
     st[3]={27,39,33},
     k[3] ={133,133,133}, // сколько шагов на 1 мм
     del[3] ={2,2,2},
     nol[3] ={A3, A1, A2};
int xt,yt,zt;
long lineP;
File pf,filelog; 
byte g1, g2, No;
float Xcur,Ycur,Zcur;
char coordstr[50];
long steps,steps_,pause_g01, pause;
float KKK1,KKK2,KKK3;
  byte i;
  int feedrate;
  float deltax,deltay,deltaz,newx,newy,newz;
  char  s1[10], s2[10],s3[10],c[10];
  long dx,dy,dz,madex,madey,madez;
boolean xplus,yplus,zplus;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
 
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
char* menu[] = {"Menu Item 1    -","Menu Item 2    -", "Menu Item 3    -",
                   "Menu Item 4    -","Menu Item 5    -","Menu Item 6    -", 
                   "Menu Item 7    -","Menu Item 8    -","Menu Item 9    -","Menu Item 10    -" };
void copyStr(char* goal, char* data){
  int i=0;
  while (data[i]!=0) {
     goal[i]=data[i];
     i++;
  }
  goal[i]=0;
}
void appStr(char* goal, char* data){
  int i,j;
   i=0;j=0;
  while (goal[i]!=0) { i++;};
  while (data[j]!=0) {goal[i+j]=data[j];j++;};
  goal[i+j]=0;
  
}

void Release(){
    digitalWrite(en[0],LOW);digitalWrite(en[1],LOW);digitalWrite(en[2],LOW);digitalWrite(13,LOW);
}
void ShowMenu(char line1[], char line2[],int pos){
     lcd.clear();
     lcd.setCursor(0,pos);
     lcd.print('>');
     lcd.setCursor(0,1-pos);
     lcd.print(' ');
 
     lcd.setCursor(1,0);
     lcd.print(line1);
     lcd.setCursor(1,1);
     lcd.print(line2);
}
int SelectMenuItem (int CountMenuItem, int preSelect){
  int currentItem = preSelect;
  int posCurrent=0;
  int key;
     do {
       if (posCurrent==0) {ShowMenu(menu[currentItem],menu[currentItem+1],posCurrent);} else {ShowMenu(menu[currentItem-1],menu[currentItem],posCurrent);};
       key = my_Key();
       if (key==btnUP) { if (posCurrent==1) posCurrent=0;
                   if(currentItem>0) currentItem--;    }            
       if (key==btnDOWN){if (posCurrent==0)  posCurrent=1; 
                   if (currentItem<CountMenuItem) currentItem++; }
     }while ((key != btnSELECT) &(key != btnLEFT));  
     if (key==btnLEFT) return -1;
     return currentItem;
}  
int my_Key(){
  int readed;
 // digitalWrite(LCD_LIGHT,HIGH);
  while (read_LCD_buttons()== btnNONE) {}
  readed = read_LCD_buttons();
  while (read_LCD_buttons()!= btnNONE) {}
  return readed;
}
int read_LCD_buttons(){
unsigned long delta;
 adc_key_in = analogRead(0);
 if (adc_key_in > 1000) {
   delta=millis()-sleep;
 //  if (delta>13000){ analogWrite (LCD_LIGHT,0);} else {if ((millis()-sleep)>10000) analogWrite (LCD_LIGHT,50);};
   return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 }
 // For V1.1 us this threshold
 //digitalWrite(LCD_LIGHT,HIGH);
 sleep=millis();

 if (adc_key_in < 50)  return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}
void Step(byte i){
  digitalWrite(st[i],HIGH);
  delayMicroseconds(100);// 100
  digitalWrite(st[i],LOW);
}
char a[70];
unsigned long MyPause(int i){
 if(read_LCD_buttons()!=btnNONE){
 Setpopr();
 }
 if(i<HH) i=HH;
 return (long(i)*100/popr);
}
void serP(){
 int key;
  copyStr(menu[3],"Pause work");
  copyStr(menu[1],"Shpindel OFF");
  copyStr(menu[2],"Shpindel ON");
  copyStr(menu[0],"Return to Pr");
rep1:
  key=SelectMenuItem(3,0);
   if (key==3){
          SaveWork();
          digitalWrite(shp,LOW);
          lcd.clear();
          lcd.print("Switch OFF");
          do {} while(true);
    }
    if (key==1) digitalWrite(shp,LOW );
    if (key==2) digitalWrite(shp,HIGH);
    if (key==0) return;
  goto rep1;
}
void SaveWork(){
  long nedo;
  float nedodo;
  Serial.print(" X= ");Serial.print(Xcur);Serial.print(" Y= ");Serial.print(Ycur);Serial.print(" Z= ");Serial.println(Zcur);
  filelog.println("stopped by user");
  filelog.print("Current X=");
  nedo = dx-madex;nedodo = nedo/(float(k[0])*float(del[0]));
  if (xplus) Xcur=Xcur-nedodo; else Xcur=Xcur+nedodo;
  nedo = dy-madey;nedodo = nedo/(float(k[1])*float(del[1]));
  if (yplus) Ycur=Ycur-nedodo; else Ycur=Ycur+nedodo;
  nedo = dz-madez;nedodo = nedo/(float(k[2])*float(del[2]));
  if (zplus) Zcur=Zcur-nedodo; else Zcur=Zcur+nedodo;
  Serial.print(" X= ");Serial.print(Xcur);Serial.print(" Y= ");Serial.print(Ycur);Serial.print(" Z= ");Serial.println(Zcur);
  filelog.print(Xcur);
  filelog.print("   Y=");
  filelog.print(Ycur);
  filelog.print("   Z=");
  filelog.print(Zcur);
  filelog.print("   feedrate=");
  filelog.println(feedrate);
  filelog.close();
  SD.remove("stop.txt");      
  filelog = SD.open("stop.txt",FILE_WRITE);
  filelog.println(filepath);
  filelog.println(lineP);
  filelog.println(Xcur);
  filelog.println(Ycur);
  filelog.println(Zcur);
  filelog.println(feedrate);
  filelog.close();
  
}
void Setpopr(){
  int key;
  lcd.clear();
  lcd.print("Current speed: >");
  key=my_Key();
  do {
    lcd.setCursor(6,1);
    lcd.print(popr);
    lcd.print("%   ");
    
    key=my_Key();
    switch (key) {
      case btnUP:     popr+=5;break;
      case btnDOWN:   popr-=5;break;
      case btnRIGHT:  serP();
//      case bCtnRIGHT:  digitalWrite(shp,HIGH);break;
//      case btnLEFT:   digitalWrite(shp,LOW);break;
    }
   }while (key!=btnSELECT);
 lcd.clear();
 lcd.print("Continuing...");
}
boolean MyReadCom(){
 if (Serial.available()>0) {
    delay(100);
    i=0;
    while(Serial.available()>0) {
       a[i] = Serial.read(); i++; a[i]=0;}
       return true;
    } else {read_LCD_buttons(); return false;};
}
void MyRun(float newx, float newy, float newz, int fr){
    digitalWrite(en[0],HIGH); digitalWrite(en[1],HIGH); digitalWrite(en[2],HIGH);
    deltax = newx-Xcur;deltay = newy-Ycur;deltaz = newz-Zcur;
    dx = deltax*k[0];dy = deltay*k[1];dz = deltaz*k[2];
    Xcur = Xcur+float(dx)/float(k[0]);Ycur = Ycur+float(dy)/float(k[1]);Zcur = Zcur+float(dz)/float(k[2]);
    dx = dx*del[0];dy = dy*del[1]; dz = dz*del[2];
    if (dx<0){
        dx = dx*(-1);
        xplus = false;
      digitalWrite(cc[0],LOW); 
    } else {digitalWrite(cc[0],HIGH);xplus=true;};
      if (dy<0){
        dy = dy*(-1);
        yplus = false;
      digitalWrite(cc[1],LOW); 
      } else {digitalWrite(cc[1],HIGH);yplus=true;};
      if (dz<0){
        dz = dz*(-1);
        zplus = false;
      digitalWrite(cc[2],LOW); 
      } else {digitalWrite(cc[2],HIGH);zplus=true;};
      madex = 0;madey = 0; madez = 0;
      if ( dx>=dy & dx>=dz){//   DX
         No=0;
         steps= dx;
         KKK1 = 1;
         KKK2 = float(dy)/float(dx);
         KKK3 = float(dz)/float(dx);} 
      if (dy>=dx & dy>=dz){//    DY
           steps= dy;
           No=1;
           KKK1 = float(dx)/float(dy);
           KKK2 = 1;
           KKK3 = float(dz)/float(dy);}
      if (dz>=dx & dz>=dy){//    DZ
           steps= dz;
           No=2;
           KKK1 = float(dx)/float(dz);
           KKK2 = float(dy)/float(dz);
           KKK3 = 1;}  
  

  //  for (i=0;i<3;i++){digitalWrite(en[i],HIGH);};
  i=0;
 // Serial.print(" Wait ");
 // Serial.print((steps*(MyPause(fr)+308)/1000000));
 // Serial.print(" sec..\r");
  for  (steps_ = 0; steps_ < steps ; steps_++){
   if (KKK1==1){    //XXX
          madex++;  Step(0);
          if (((steps_ * KKK2 ) - madey) > 1){
            madey++; Step(1);}
        if (((steps_ * KKK3 ) - madez) > 1){
            madez++;  Step(2);}  
       }
      if (KKK2==1){    // YYY
          madey++; Step(1);
          if (((steps_ * KKK1 ) - madex) > 1){
            madex++; Step(0); }
          if (((steps_ * KKK3 ) - madez) > 1){
            madez++; Step(2);}  
       } 
       if (KKK3==1){    // ZZZ
          madez++; Step(2);
          if (((steps_ * KKK1 ) - madex) > 1){
            madex++;Step(0); }
          if (((steps_ * KKK2 ) - madey) > 1){
            madey++; Step(1);}  
       }
    delayMicroseconds(MyPause(fr));   
 /*  if (fr !=0){ delayMicroseconds(MyPause(fr));} else
                       {delayMicroseconds(MyPause(HH));};*/
    if (steps_ == 0) {delayMicroseconds(MyPause(fr));} // медленный старт
};
     if(madex < dx) Step(0);
     if(madey < dy) Step(1);
     if(madez < dz) Step(2);
}

void ShowPCoord(){
       lcd.clear();lcd.setCursor(0,0);lcd.print(lineP);lcd.setCursor(8,0);lcd.print("Z");lcd.setCursor(0,1);lcd.print("X");lcd.setCursor(8,1);lcd.print("Y");lcd.setCursor(1,1);
       lcd.print(Xcur);lcd.setCursor(9,1);lcd.print(Ycur);lcd.setCursor(9,0);lcd.print(Zcur);
}

void ShowCoord(){
       lcd.clear();lcd.setCursor(0,0);lcd.print("PC");lcd.setCursor(8,0);lcd.print("Z");lcd.setCursor(0,1);lcd.print("X");lcd.setCursor(8,1);lcd.print("Y");lcd.setCursor(1,1);
       lcd.print(Xcur);lcd.setCursor(9,1);lcd.print(Ycur);lcd.setCursor(9,0);lcd.print(Zcur);
}
//----------------------------------------------------------------------

void WorkString(){
    float kkk, X_, Y_, Z_; 
    char kk[10];
     X_=Xcur;
     Y_=Ycur;
     Z_=Zcur;
     int currentfeedrate=feedrate;
     byte k,i1,i;
     i=0;
     if (a[0]=='T') {
         digitalWrite(13,LOW);
         Serial.print("Ok.\r");
         return;}
     while (a[i]!=0) {
         switch (a[i]){
           case 'S':             k=6;             break;
           case 'G':             k=0;             break;
           case 'X':             k=1;             break;
           case 'Y':             k=2;             break;
           case 'Z':             k=3;             break;
           case 'F':             k=4;             break;
           case 'M':             k=5;             break;
           default:             break;}
       i++;
       while (a[i]!=0) {
                   if(a[i] < 60){ kk[i1] = a[i]; i++; i1++;}
             else {kk[i1]=0; break;}}
       kk[i1]=0;
       kkk=atof(kk);
       switch (k){
         case 0:            if (kkk==0) {currentfeedrate=HH;};            break;
         case 1:           X_=kkk;                                        break;
         case 2:           Y_=kkk;                                        break;
         case 3:           Z_=kkk;                                        break;
         case 4:           feedrate=2000000 / kkk +230;
                    if (currentfeedrate!=HH) {currentfeedrate=feedrate;}; break;
         case 5:
             if (kkk==3) {digitalWrite(shp, HIGH);Serial.println ("Shpintel ON");};
             if (kkk==30){digitalWrite(shp, LOW);Serial.println ("Shpintel OFF");}; break;
         case 6:        break;       }
       kk[0]=0;
       i1=0;
  }
 // Serial.print("X->");Serial.print(X_);Serial.print("   Y->");Serial.print(Y_);Serial.print("  Z->");Serial.print(Z_);Serial.print("  feed ");Serial.println(currentfeedrate);
  MyRun(X_, Y_, Z_,currentfeedrate);
     digitalWrite(13,LOW);
     

}

void ConnectPC(){
  float Xmem,Ymem,Zmem;
  signed int edit[3];
  a[0]=0;
  ShowCoord();
  while (true){ 
    if (MyReadCom()){
       digitalWrite(13, HIGH);
//       if (a[0]=='f' & a[1]=='i' & a[2]=='l' & a[3]=='e'){
 //         reciveFile(); continue;}
       if (a[0]=='y') return;
       if (a[0]=='w'){                         // вверх
          Serial.println("Y+5");MyRun(Xcur,Ycur+5,Zcur,HH);ShowCoord(); continue;}
       if (a[0]=='s'){                         // вниз 
          Serial.println("Y-5");MyRun(Xcur,Ycur-5,Zcur,HH);ShowCoord();continue;}
       if (a[0]=='a'){                         // влево
          Serial.println("X-5");MyRun(Xcur-5,Ycur,Zcur,HH);ShowCoord();continue;}
       if (a[0]=='d'){                         // вправо 
          Serial.println("X+5");MyRun(Xcur+5,Ycur,Zcur,HH);ShowCoord();continue;}       
       if (a[0]=='q'){                         // поднять
          Serial.println("Z+5");MyRun(Xcur,Ycur,Zcur+5,HH);ShowCoord();continue;}
       if (a[0]=='z'){                         // опустить
          Serial.println("Z-5");MyRun(Xcur,Ycur,Zcur-5,HH);ShowCoord();continue;}
       if (a[0]=='e'){                         // обнулить
          sprintf(a,"g09 x 0 y 0 z 0");       };
       if(a[0]=='r'){
              Serial.print ("Release Steppers\r");Release();Serial.print("Ok.\r");continue;}
       i = sscanf(a,"%s x %s y %s z %s ",&c, &s1, &s2, &s3 );
    if(a[0]=='g' & a[1]=='0' & a[2]=='9'){  //    отсебятина считать, что текущие координаты следующие
        Xcur = atof(s1);  Ycur = atof(s2); Zcur = atof(s3);digitalWrite(13,LOW); Serial.print("New koord X=");Serial.print(Xcur); Serial.print(" Y="); Serial.print(Ycur); Serial.print(" Z="); Serial.print(Zcur); Serial.print("\rOk.\r");ShowCoord();continue;  };
    if(a[20]=='g'& a[1]=='0' & a[2]=='5'){ //      отсебятина относительный переход ))))
      MyRun(Xcur+atof(s1),Ycur+atof(s2),Zcur+atof(s3),feedrate);digitalWrite(13,LOW); Serial.print("Xcur=");Serial.print(Xcur);Serial.print(" Ycur=");Serial.print(Ycur);Serial.print(" Zcur=");Serial.print(Zcur);Serial.print("\r");ShowCoord();continue;}
     WorkString();ShowCoord();Serial.print("Ok.\r");   
  }
 }
}

//---------------------------------------------------------------------- 
void setup()
{
 lcd.begin(16, 2);              // start the library
   byte i;
   pinMode(LCD_LIGHT,OUTPUT);
   analogWrite(LCD_LIGHT,40);
   pinMode(53, OUTPUT);
   pinMode(48, OUTPUT);
   digitalWrite(48,HIGH);
  for(i=0; i <3; i++){
    
    pinMode(cc[i],OUTPUT);
    pinMode(en[i],OUTPUT);
    pinMode(st[i],OUTPUT);
    digitalWrite(st[i],LOW);
    digitalWrite(en[i],LOW);
    digitalWrite(cc[i],HIGH);}
  Serial.begin(19200);
  pinMode(shp,OUTPUT);
  digitalWrite(shp,LOW);
  pause = 0;
  Xcur = 0 ; Ycur = 0; Zcur = 0; 
  feedrate = 500;

}
void MoveXY(){
  int key;
  do {
     lcd.clear();
     lcd.print("Moving XY.");
     lcd.setCursor(0,1);
     lcd.print("SELECT to finish");
     key=my_Key();
     switch (key){
       case btnUP:MyRun(Xcur,Ycur-5,Zcur,HH);Release();break;
       case btnDOWN:MyRun(Xcur,Ycur+5,Zcur,HH);Release();break;
       case btnLEFT:MyRun(Xcur+5,Ycur,Zcur,HH);Release();break;
       case btnRIGHT:MyRun(Xcur-5,Ycur,Zcur,HH);Release();break;}
  } while (key!= btnSELECT);
}
void MoveZ(){
  int key;
  do {
    lcd.clear();
    lcd.print("Moving Z.");
    lcd.setCursor(0,1);
    lcd.print("SELECT to finish");
    key=my_Key();
     switch (key){
       case btnUP:MyRun(Xcur,Ycur,Zcur+5,HH);Release();break;
       case btnDOWN:MyRun(Xcur,Ycur,Zcur-5,HH);Release();break;}
  } while (key!= btnSELECT);
}
void SetCoord(){
  copyStr(menu[0],"Set(0,0,0)");
  copyStr(menu[1],"Set(0,-10,0)");
  copyStr(menu[2],"Select customer");
  switch (SelectMenuItem(2,0)){
    case -1:   break;
    case 0:    Xcur=0;Ycur=0;Zcur=0;break;
    case 1:    Xcur=0;Ycur=-10;Zcur=0;break;
    case 2:    if(SelectCoord(true)==true){Xcur=xt;Ycur=yt;Zcur=zt;};  break;    
  }; 
}
boolean SelectCoord(boolean Cur){
  int key,curp,i;
  char st[]="X 000;Y 000;Z 00";
  if(Cur)sprintf(st,"X% 04d;Y% 04d;Z% 03d",int(Xcur),int(Ycur),int(Zcur)); 
  lcd.clear();
  lcd.print("Select position");
  lcd.blink();
  curp=2;
  do {
  lcd.setCursor(0,1);
  lcd.print(st);
    lcd.setCursor(curp,1);
    key=my_Key();
     switch (key){
       case btnUP:
            if(st[curp]==' ') break;
            if (st[curp]=='-') {st[curp]=' ';} else {if (st[curp]!='9'){st[curp]++;} else st[curp]='0';};   break;
       case btnDOWN:
            if(st[curp]=='-') break;
            if (st[curp]==' ') {st[curp]='-';} else {if (st[curp]!='0'){st[curp]--;} else st[curp]='9';};   break;
       case btnRIGHT:
            if (curp<15) do{ curp++;} while (st[curp]=='X' | st[curp]=='Y' | st[curp]=='Z' | st[curp]==';');    break;
       case btnLEFT:
            if (curp>1) do{ curp--;} while (st[curp]=='X' | st[curp]=='Y' | st[curp]=='Z' | st[curp]==';');     break;
     }
  }while (key!=btnSELECT);
  lcd.clear();
  i=sscanf(st,"X%d;Y%d;Z%d", &xt, &yt, &zt );
  lcd.noBlink();
  lcd.print("Sel=No,R=Yes");
  sprintf(st,"x%d y%d z%d",xt,yt,zt);
  lcd.setCursor(0,1);
  lcd.print(st);
  key=my_Key();
  if (key==btnRIGHT){return true;} else {return false;};
}



int skipped; 

void readln(){
    int  i=0;
      do{ tmpstr[i] = pf.read();
          i++;} while (tmpstr[i-1]!=10);
      tmpstr[i-2]=0;
}
void RunSD(){
  File root;
  long tmplong;
  boolean JustReset;
  float Xtemp,Ytemp,Ztemp;
  lcd.clear();
  lcd.print("Initialise SD...");
  lcd.setCursor(0,1);
  if (SD.begin(53)){
    lcd.print("Ok");
    if(SD.exists("stop.txt")){
      if (Xcur==0 & Ycur==0 & Zcur==0)JustReset=true; else JustReset=false;
      pf = SD.open("stop.txt");
      readln();
      copyStr(filepath,tmpstr);
      lcd.clear();
      lcd.print(filepath);
      readln();
      tmplong = atof(tmpstr);
      lcd.setCursor(0,1);
      lcd.print("Line:");
      lcd.print(tmplong);
      readln();
      Xtemp=atof(tmpstr);
      readln();
      Ytemp=atof(tmpstr);
      readln();
      Ztemp=atof(tmpstr);
      readln();
      feedrate=atof(tmpstr);
      if (JustReset) {Xcur=Xtemp; Ycur=Ytemp; Zcur=Ztemp;} else
                      MyRun(Xtemp,Ytemp,Ztemp,feedrate);
      pf.close();
      SD.remove("stop.txt");
      lcd.clear();
      lcd.print("Cont. at ");
      lcd.print(tmplong);
      lcd.setCursor(0,1);
      lcd.print("File:");
      lcd.print(filepath);
      my_Key(); 
      filelog = SD.open("log.txt",FILE_WRITE);
      filelog.println("Continued...");
      lcd.clear();
      lcd.print("Skipping...");
      lcd.setCursor(7,1);
      lcd.print("of ");lcd.print(tmplong);
      pf=SD.open(filepath);
      for(lineP=1;lineP<tmplong;lineP++){
        if((lineP%100)==0){
          lcd.setCursor(0,1);
          lcd.print(lineP);}
      readln();};
      digitalWrite(shp,HIGH);
      delay(1000);
      RunP();
      return;
    }
    filepath="/";
    root = SD.open("/");
    printDirectory(root);} 
  else{lcd.print("Failed(");
    my_Key();
  return;};
}


void printDirectory(File dir) {
 int count = 0,choise,i;
 char* tmp = "00000000.000";
 File entry;
 skipped=0;
 dir.rewindDirectory();
rest:
 count = 0;
  while(true) {
     entry =  dir.openNextFile();
  //   Serial.println (entry.name());
     if (! entry) { break;     }
     tmp = entry.name();
     Serial.println(tmp);
     //   if ((tmp[0]!='L' | tmp[1]!='O' |tmp[2]!='G' |tmp[3]!='.' |tmp[4]!='T' |tmp[5]!='X' |tmp[6]!='T')&(tmp[0]!='S' | tmp[1]!='T' |tmp[2]!='P' |tmp[3]!='.' |tmp[4]!='T' |tmp[5]!='X' |tmp[6]!='T')){
        copyStr(menu[count],entry.name());
        count++;//}
     if(count==9) {copyStr(menu[count],"More..."); count++; break;}
   }
   if (count==1) {copyStr(menu[1]," ");};
   choise=SelectMenuItem(count-1,0);
   if (choise==9) {skipped+=9; goto rest;};
   dir.rewindDirectory();
   entry =  dir.openNextFile();
   for (i=0;i < (choise+skipped);i++) {entry =  dir.openNextFile(); }
   if (entry.isDirectory()){
            appStr(filepath,entry.name());
            appStr(filepath,"/\0");
            printDirectory(entry);
            return;
   } else
   {        //Serial.println(entry.name());
            lcd.clear();
            lcd.print(entry.name());
            lcd.setCursor(0,1);
            lcd.print("Cancel      Ok");
            if(my_Key()!=btnRIGHT) return;
            appStr(filepath,entry.name());
            Serial.print(filepath);
            SD.remove("log.txt");
            filelog = SD.open("log.txt",FILE_WRITE);
            if(!filelog) {Serial.println("Error log file");
                          return;}
            filelog.print("Filename:");
            filelog.println(filepath);
            pf = entry;//SD.open(menu[choise]);   
            SetCoord();
            ShowCoord();
            lcd.setCursor(0,0);
            Release();
            lcd.print("Go?");
            filelog.print("Start position:X=");
            filelog.print(Xcur);
            filelog.print(" Y=");
            filelog.print(Ycur);
            filelog.print(" Z=");
            filelog.println(Zcur);
            lineP=1;
            RunP();
            pf.close();
            filelog.close();
            Release();
           // digitalWrite(LCD_LIGHT,LOW);
   }
}


void RunP(){
  int i=0;
  File logfile;
  while (pf.available()) {
     
    	a[i]=pf.read();
        if (a[i]==10){
              a[i-1]=0;
              filelog.println(a);
              filelog.flush(); 
              WorkString();
              lineP++;
              Serial.println(a);
              ShowPCoord();
              i=0;} else {i++;}
        
    }
}
void FreeMove(){
   if(SelectCoord(true)) {MyRun(xt,yt,zt,HH);Release();}
}
void RelMove(){
  if(SelectCoord(false)) {MyRun(Xcur+xt,Ycur+yt,Zcur+zt,HH);Release();}
}
void loop(){ 
  copyStr(menu[0],"Connect PC");
  copyStr(menu[1],"Move X,Y");
  copyStr(menu[2],"Move Z");
  copyStr(menu[3],"Run Program");
  copyStr(menu[4],"Relative move");
  copyStr(menu[5],"Absolute move");
  copyStr(menu[6],"Set coord.");
  switch (SelectMenuItem(6,0)){
    case -1:   break;
    case 0:    ConnectPC();break;
    case 1:    MoveXY();break;
    case 2:    MoveZ();break;
    case 3:    RunSD(); break;
    case 4:    RelMove();break;
    case 5:    FreeMove();break;
    case 6:    SetCoord();break;
    
  }; 
}
 
