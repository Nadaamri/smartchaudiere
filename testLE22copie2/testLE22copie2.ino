#include <Keypad.h> 
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define Relai1  7
#define LED1    A3

char keys[4][4] = {
  
{'1', '2', '3', 'A'},

{'4', '5', '6', 'B'},

{'7', '8', '9', 'C'},

{'*', '0', '#', 'D'}

};

//byte rowPins[4] = {2,3,4,5}   ;
//byte colPins[4] = {6,A0,A1,A2};
byte rowPins[4] = {6,A0,A1,A2}   ;
byte colPins[4] = {2,3,4,5};

int  i1,i2;
char c1,c2;

String cmd;
char keypressed;

String Mstatus ;
String Astatus;

tmElements_t tm;
int mode = 1;

int A_hour = 11 ; 
int A_minute = 30;

int B_hour = 11 ; 
int B_minute = 32;

String stat = "Off";

String Hon  ;
String Mon  ;

String Hoff  ;
String Moff  ;

Keypad myKeypad= Keypad(makeKeymap(keys), rowPins, colPins,4,4);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// serialport //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void TraitementPortSerie(){
cmd = Serial.readString();


if(cmd[0]=='T' && cmd[11]=='.'){   // format de reglage de temps = T-H15M03S25.
              tm.Hour = (cmd[3]-48)*10+(cmd[4]-48);
              tm.Minute = (cmd[6]-48)*10+(cmd[7]-48);
              tm.Second = (cmd[9]-48)*10+(cmd[10]-48);
              RTC.write(tm);
}
if(cmd[0]=='D' && cmd[11]=='.'){   // format de reglage de temps = D-Y20M03D25.
              tm.Year = (cmd[3]-48)*10+(cmd[4]-48)+2000-18;
              tm.Month = (cmd[6]-48)*10+(cmd[7]-48);
              tm.Day = (cmd[9]-48)*10+(cmd[10]-48);
              RTC.write(tm);
}
if(cmd[0]=='R' && cmd[18]=='.'){   // format de reglage de temps = R-NH20NM33FH21FM33.
            int  A_h = (cmd[4]-48)*10+(cmd[5]-48);
            int  A_m = (cmd[8]-48)*10+(cmd[9]-48);
            int  B_h = (cmd[12]-48)*10+(cmd[13]-48);
            int  B_m = (cmd[16]-48)*10+(cmd[17]-48);
            A_hour = A_h;
            A_minute = A_m;
            B_hour = B_h;
            B_minute = B_m;
            EEPROM.write(10,A_hour);
            EEPROM.write(11,A_minute);
            EEPROM.write(12,B_hour);
            EEPROM.write(13,B_minute);
            
            mode = 1;
}
if(cmd[0]=='A' ){
              digitalWrite(Relai1,HIGH);
              digitalWrite(LED1,HIGH);
              Mstatus = "On ";
              mode = 0;
              }
if(cmd[0]=='E' ){
              digitalWrite(Relai1,LOW);
              digitalWrite(LED1,LOW);
              Mstatus = "Off";
              mode = 0;
              }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// setup //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void setup() {
 pinMode(7, OUTPUT);
 pinMode(LED1, OUTPUT);

//if( EEPROM.read(10) != 0xFF && EEPROM.read(11) !=0xFF && EEPROM.read(12) !=0xFF && EEPROM.read(13) !=0xFF ){
         A_hour   =EEPROM.read(10);
         A_minute =EEPROM.read(11);
         B_hour   =EEPROM.read(12);
         B_minute =EEPROM.read(13);
//}
 lcd.begin(20, 4); 
  Serial.begin(9600);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// loop //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void loop()
{ 
  while(((keypressed = myKeypad.getKey()) == NO_KEY) ){
  
  if(Serial.available() > 0){ TraitementPortSerie(); }

    RTC.read(tm);
    lcd.setCursor(0,0);lcd.print("Time   ");print2digits(tm.Hour);lcd.write(':');print2digits(tm.Minute);lcd.write(':');print2digits(tm.Second);
    lcd.setCursor(0,1);lcd.print("Date   ");print2digits(tm.Day);lcd.write('/');print2digits(tm.Month);lcd.write('/');lcd.print(tmYearToCalendar(tm.Year));
    lcd.setCursor(0,2);lcd.print("Mode : ");
    lcd.setCursor(9,2);lcd.print("stat : ");

if (mode == 1){
    lcd.setCursor(7,2);lcd.print("A");lcd.setCursor(0,3);lcd.print("Ton");lcd.setCursor(4,3);print2digits(A_hour);lcd.setCursor(6,3);lcd.print('h');lcd.setCursor(7,3);print2digits(A_minute);lcd.setCursor(10,3);lcd.print("Toff");lcd.setCursor(15,3);print2digits(B_hour);lcd.setCursor(17,3);lcd.print('h');lcd.setCursor(18,3);print2digits(B_minute);
    if(tm.Hour>=A_hour && tm.Minute>=A_minute && tm.Hour<=B_hour && tm.Minute<B_minute && mode==1){
    lcd.setCursor(16,2);lcd.print("On ");digitalWrite(7, HIGH);digitalWrite(A3, HIGH);Astatus = "On ";
    }
    if((tm.Hour<=A_hour || tm.Hour>=B_hour) && (tm.Minute<A_minute || tm.Minute>=B_minute) && mode==1 && tm.Second>0){
     lcd.setCursor(16,2);lcd.print("off");digitalWrite(7, LOW);digitalWrite(A3, LOW);Astatus = "Off";
   }
   print2digit(tm.Hour);print2digit(tm.Minute);print2digit(tm.Second);print2digit(tm.Day);print2digit(tm.Month);Serial.print(tmYearToCalendar(tm.Year));Serial.print("A");print2digit(A_hour);print2digit(A_minute);print2digit(B_hour);print2digit(B_minute);Serial.print(Astatus);
   Serial.println("");delay(1000);
}

if (mode == 0){
  lcd.setCursor(7,2);lcd.print("M");lcd.setCursor(16,2);lcd.print(Mstatus);lcd.setCursor(0,3);lcd.print("                    ");
print2digit(tm.Hour);print2digit(tm.Minute);print2digit(tm.Second);print2digit(tm.Day);print2digit(tm.Month);Serial.print(tmYearToCalendar(tm.Year));Serial.print("M");print2digit(A_hour);print2digit(A_minute);print2digit(B_hour);print2digit(B_minute);Serial.print(Mstatus);
Serial.println("");delay(1000);
}

}
  if (keypressed == 'A'){ ConfigTime()      ;    }
  if (keypressed == 'B'){ ConfigDate()      ;    }
  if (keypressed == 'C'){ ConfigModeAuto()  ;    }
  if (keypressed == 'D'){ ManualMode()      ;    }
  
  
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ConfigDate //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void ConfigDate(){
lcd.clear();
lcd.print("     Setup");
delay(1000);
lcd.clear();
///////////////////////////////////////////////////////Setup year
lcd.print("Setup year");
  
     char keypressed2 = myKeypad.waitForKey();  
          while(keypressed2 == NO_KEY || keypressed2 =='*' || keypressed2 =='#' || keypressed2 =='A' || keypressed2 =='B' || keypressed2 =='C' || keypressed2 =='D' )
              { keypressed2 = myKeypad.waitForKey(); }
          c1 = keypressed2;
          lcd.setCursor(0, 1);
          lcd.print(c1);
  
     char keypressed3 = myKeypad.waitForKey();
          while(keypressed3 == NO_KEY || keypressed3 =='*' || keypressed3 =='#' || keypressed3 =='A' || keypressed3 =='B' || keypressed3 =='C' || keypressed3 =='D' )
              { keypressed3 = myKeypad.waitForKey(); }
          c2 = keypressed3;
          lcd.setCursor(1, 1);
          lcd.print(c2);
i1=(c1-48)*10;   
i2=(c2-48);
int N_year=i1+i2+2000-18;
delay(500);
lcd.clear();
///////////////////////////////////////////////////////Setup month
lcd.print("Setup month");
 
     char keypressed4 = myKeypad.waitForKey();  
           while(keypressed4 == NO_KEY || keypressed4 =='*' || keypressed4 =='#' || keypressed4 =='A' || keypressed4 =='B' || keypressed4 =='C' || keypressed4 =='D' || keypressed4 =='2' || keypressed4 =='3' || keypressed4 =='4' || keypressed4 =='5' || keypressed4 =='6' || keypressed4 =='7' || keypressed4 =='8' || keypressed4 =='9')
               { keypressed4 = myKeypad.waitForKey();}
           c1 = keypressed4;
           lcd.setCursor(0, 1);
           lcd.print(c1);

     char keypressed5 = myKeypad.waitForKey();      
if (c1 == '0') {
          while (keypressed5 == NO_KEY || keypressed5 =='*' || keypressed5 =='#' || keypressed5 =='A' || keypressed5 =='B' || keypressed5 =='C' || keypressed5 =='D' )
              { keypressed5 = myKeypad.waitForKey();}
          c2 = keypressed5;
          lcd.setCursor(1, 1);
          lcd.print(c2);
           }
else {
           while (keypressed5 == NO_KEY || keypressed5 =='*' || keypressed5 =='#' || keypressed5 =='A' || keypressed5 =='B' || keypressed5 =='C' || keypressed5 =='D' || keypressed5 =='3' || keypressed5 =='4' || keypressed5 =='5' || keypressed5 =='6' || keypressed5 =='7' || keypressed5 =='8' || keypressed5 =='9' )
              { keypressed5 = myKeypad.waitForKey();}
          c2 = keypressed5;
          lcd.setCursor(1, 1);
          lcd.print(c2);
          }
i1=(c1-48)*10;
i2=c2-48;
int N_month=i1+i2;
delay(500);
lcd.clear();
///////////////////////////////////////////////////////Setup Day
lcd.print("Setup Day");

     char keypressed6 = myKeypad.waitForKey();
          while (keypressed6 == NO_KEY || keypressed6 =='*' || keypressed6 =='#' || keypressed6 =='A' || keypressed6 =='B' || keypressed6 =='C' || keypressed6 =='D'|| keypressed6 =='4' || keypressed6 =='5' || keypressed6 =='6' || keypressed6 =='7' || keypressed6 =='8'|| keypressed6 =='9' )
              { keypressed6 = myKeypad.waitForKey();} 
          c1 = keypressed6;
          lcd.setCursor(0, 1);
          lcd.print(c1);

               char keypressed7 = myKeypad.waitForKey();      
if (c1 == '0' || c1 == '1' || c1 == '2') {
  
          while (keypressed7 == NO_KEY || keypressed7 =='*' || keypressed7 =='#' || keypressed7 =='A' || keypressed7 =='B' || keypressed7 =='C' || keypressed7 =='D' )
              { keypressed7 = myKeypad.waitForKey();}
          c2 = keypressed7;
          lcd.setCursor(1, 1);
          lcd.print(c2);
           }
else {
          while (keypressed7 == NO_KEY || keypressed7 =='*' || keypressed7 =='#' || keypressed7 =='A' || keypressed7 =='B' || keypressed7 =='C' || keypressed7 =='D'|| keypressed7 =='2' || keypressed7 =='3' || keypressed7 =='4' || keypressed7 =='5' || keypressed7 =='6' || keypressed7 =='7' || keypressed7 =='8' || keypressed7 =='9' )
              { keypressed7 = myKeypad.waitForKey();}
          c2 = keypressed7;
          lcd.setCursor(1, 1);
          lcd.print(c2);
          }
i1=(c1-48)*10;
i2=c2-48;
int N_day=i1+i2;
delay(500);
tm.Year = N_year;
tm.Month = N_month;
tm.Day = N_day;
RTC.write(tm);
keypressed = NO_KEY; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ConfigTime //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void ConfigTime(){
lcd.clear();
lcd.print("     Setup");
delay(1000);
lcd.clear();

///////////////////////////////////////////////////////Setup hour
lcd.print("Setup hour");                    
     char keypressed8 = myKeypad.waitForKey();  
          while(keypressed8 == NO_KEY || keypressed8 =='*' || keypressed8 =='#' || keypressed8 =='A' || keypressed8 =='B' || keypressed8 =='C' || keypressed8 =='D' || keypressed8 =='3' || keypressed8 =='4' || keypressed8 =='5' || keypressed8 =='6' || keypressed8 =='7' || keypressed8 =='8' || keypressed8 =='9' )
              { keypressed8 = myKeypad.waitForKey(); }
          c1 = keypressed8;
          lcd.setCursor(0, 1);
          lcd.print(c1);
  
     char   keypressed9 = myKeypad.waitForKey();
if (c1 =='2'){
           while(keypressed9 == NO_KEY || keypressed9 =='*' || keypressed9 =='#' || keypressed9 =='A' || keypressed9 =='B' || keypressed9 =='C' || keypressed9 =='D' || keypressed9 =='4' || keypressed9 =='5' || keypressed9 =='6' || keypressed9 =='7' || keypressed9 =='8' || keypressed9 =='9' )
                { keypressed9 = myKeypad.waitForKey(); }
           c2 = keypressed9;
           lcd.setCursor(1, 1);
           lcd.print(c2);
          }
else{
           while(keypressed9 == NO_KEY || keypressed9 =='*' || keypressed9 =='#' || keypressed9 =='A' || keypressed9 =='B' || keypressed9 =='C' || keypressed9 =='D')
                { keypressed9 = myKeypad.waitForKey(); }
           c2 = keypressed9;
           lcd.setCursor(1, 1);
           lcd.print(c2);
          }
i1=(c1-48)*10;
i2=c2-48;
int N_hour=i1+i2;
delay(500);
lcd.clear();
///////////////////////////////////////////////////////Setup minutes
lcd.print("Setup minutes");

      char keypressed10 = myKeypad.waitForKey(); 
      
         while(keypressed10 == NO_KEY || keypressed10 =='*' || keypressed10 =='#' || keypressed10 =='A' || keypressed10 =='B' || keypressed10 =='C' || keypressed10 =='D' || keypressed10 =='6' || keypressed10 =='7' || keypressed10 =='8' || keypressed10 =='9' )
              { keypressed10 = myKeypad.waitForKey(); }
              c1 = keypressed10;
              lcd.setCursor(0, 1);
              lcd.print(c1);
   
    char keypressed11 = myKeypad.waitForKey();
   
         while(keypressed11 == NO_KEY || keypressed11 =='*' || keypressed11 =='#' || keypressed11 =='A' || keypressed11 =='B' || keypressed11 =='C' || keypressed11 =='D' )
              { keypressed11 = myKeypad.waitForKey(); }
         c2 = keypressed11;
         lcd.setCursor(1, 1);
         lcd.print(c2);
i1=(c1-48)*10;
i2=c2-48;
int N_minutes=i1+i2;
delay(500);
lcd.clear();
///////////////////////////////////////////////////////Setup Second
lcd.print("Setup Second");

      char keypressed12 = myKeypad.waitForKey(); 
      
         while(keypressed12 == NO_KEY || keypressed12 =='*' || keypressed12 =='#' || keypressed12 =='A' || keypressed12 =='B' || keypressed12 =='C' || keypressed12 =='D' || keypressed12 =='6' || keypressed12 =='7' || keypressed12 =='8' || keypressed12 =='9' )
              { keypressed12 = myKeypad.waitForKey(); }
              c1 = keypressed12;
              lcd.setCursor(0, 1);
              lcd.print(c1);
   
    char keypressed13 = myKeypad.waitForKey();
   
         while(keypressed13 == NO_KEY || keypressed13 =='*' || keypressed13 =='#' || keypressed13 =='A' || keypressed13 =='B' || keypressed13 =='C' || keypressed13 =='D' )
              { keypressed13 = myKeypad.waitForKey(); }
         c2 = keypressed13;
         lcd.setCursor(1, 1);
         lcd.print(c2);
i1=(c1-48)*10;
i2=c2-48;
int N_Second=i1+i2;
delay(500);
lcd.clear();
tm.Hour = N_hour;
tm.Minute = N_minutes;
tm.Second = N_Second;
RTC.write(tm);
keypressed = NO_KEY; 
}





///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// print2digit //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void print2digit(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// print2digits //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.write('0');
  }
  lcd.print(number);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ConfigModeAuto ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigModeAuto(){
lcd.clear();
lcd.print("  setup Time On  ");
delay(1000);
lcd.clear();
lcd.print("Set hour on");
     char keypressed14 = myKeypad.waitForKey();  
          while(keypressed14 == NO_KEY || keypressed14 =='*' || keypressed14 =='#' || keypressed14 =='A' || keypressed14 =='B' || keypressed14 =='C' || keypressed14 =='D' || keypressed14 =='3' || keypressed14 =='4' || keypressed14 =='5' || keypressed14 =='6' || keypressed14 =='7' || keypressed14 =='8' || keypressed14 =='9' )
              { keypressed14 = myKeypad.waitForKey();}
          c1 = keypressed14;
          lcd.setCursor(0, 1);
          lcd.print(c1);
                       
char   keypressed15 = myKeypad.waitForKey();
if (c1 =='2'){
           while(keypressed15 == NO_KEY || keypressed15 =='*' || keypressed15 =='#' || keypressed15 =='A' || keypressed15 =='B' || keypressed15 =='C' || keypressed15 =='D' || keypressed15 =='4' || keypressed15 =='5' || keypressed15 =='6' || keypressed15 =='7' || keypressed15 =='8' || keypressed15 =='9' )
                { keypressed15 = myKeypad.waitForKey(); }
           c2 = keypressed15;
           lcd.setCursor(1, 1);
           lcd.print(c2);
          }
else{
           while(keypressed15 == NO_KEY || keypressed15 =='*' || keypressed15 =='#' || keypressed15 =='A' || keypressed15 =='B' || keypressed15 =='C' || keypressed15 =='D')
                { keypressed15 = myKeypad.waitForKey(); }
           c2 = keypressed15;
           lcd.setCursor(1, 1);
           lcd.print(c2);
          }
i1=(c1-48)*10;
i2=c2-48;
A_hour=i1+i2;
delay(500);
lcd.clear();
lcd.print("Set minutes on");

      char keypressed16 = myKeypad.waitForKey();  
           while(keypressed16 == NO_KEY || keypressed16 =='*' || keypressed16 =='#' || keypressed16 =='A' || keypressed16 =='B' || keypressed16 =='C' || keypressed16 =='D' || keypressed16 =='6' || keypressed16 =='7' || keypressed16 =='8' || keypressed16 =='9' )
               { keypressed16 = myKeypad.waitForKey();}
           c1 = keypressed16;
           lcd.setCursor(0, 1);
           lcd.print(c1);
           
      char keypressed17 = myKeypad.waitForKey();
            while(keypressed17 == NO_KEY || keypressed17 =='*' || keypressed17 =='#' || keypressed17 =='A' || keypressed17 =='B' || keypressed17 =='C' || keypressed17 =='D' )
                {keypressed17 = myKeypad.waitForKey();}
            c2 = keypressed17;
            lcd.setCursor(1, 1);
            lcd.print(c2);
i1=(c1-48)*10;
i2=c2-48;
A_minute=i1+i2;
delay(500);
lcd.clear();
lcd.print("  setup Time Off  ");
delay(1000);
lcd.clear();
lcd.print("Set hour off");

  char keypressed18 = myKeypad.waitForKey();  
          while(keypressed18 == NO_KEY || keypressed18 =='*' || keypressed18 =='#' || keypressed18 =='A' || keypressed18 =='B' || keypressed18 =='C' || keypressed18 =='D' || keypressed18 =='3' || keypressed18 =='4' || keypressed18 =='5' || keypressed18 =='6' || keypressed18 =='7' || keypressed18 =='8' || keypressed18 =='9' )
              { keypressed18 = myKeypad.waitForKey();}
          c1 = keypressed18;
          lcd.setCursor(0, 1);
          lcd.print(c1);
                       
char   keypressed19 = myKeypad.waitForKey();
if (c1 =='2'){
           while(keypressed19 == NO_KEY || keypressed19 =='*' || keypressed19 =='#' || keypressed19 =='A' || keypressed19 =='B' || keypressed19 =='C' || keypressed19 =='D' || keypressed19 =='4' || keypressed19 =='5' || keypressed19 =='6' || keypressed19 =='7' || keypressed19 =='8' || keypressed19 =='9' )
                { keypressed19 = myKeypad.waitForKey(); }
           c2 = keypressed19;
           lcd.setCursor(1, 1);
           lcd.print(c2);
          }
else{
           while(keypressed19 == NO_KEY || keypressed19 =='*' || keypressed19 =='#' || keypressed19 =='A' || keypressed19 =='B' || keypressed19 =='C' || keypressed19 =='D')
                { keypressed19 = myKeypad.waitForKey(); }
           c2 = keypressed19;
           lcd.setCursor(1, 1);
           lcd.print(c2);
          }
i1=(c1-48)*10;
i2=c2-48;
B_hour=i1+i2;
delay(500);
lcd.clear();
lcd.print("Set minutes off");
     
      char keypressed20 = myKeypad.waitForKey();  
           while(keypressed20 == NO_KEY || keypressed20 =='*' || keypressed20 =='#' || keypressed20 =='A' || keypressed20 =='B' || keypressed20 =='C' || keypressed20 =='D' || keypressed20 =='6' || keypressed20 =='7' || keypressed20 =='8' || keypressed20 =='9' )
               { keypressed20 = myKeypad.waitForKey();}
           c1 = keypressed20;
           lcd.setCursor(0, 1);
           lcd.print(c1);
          
      char keypressed21 = myKeypad.waitForKey();
            while(keypressed21 == NO_KEY || keypressed21 =='*' || keypressed21 =='#' || keypressed21 =='A' || keypressed21 =='B' || keypressed21 =='C' || keypressed17 =='D' )
                {keypressed17 = myKeypad.waitForKey();}
            c2 = keypressed21;
            lcd.setCursor(1, 1);
            lcd.print(c2);
i1=(c1-48)*10;
i2=c2-48;
B_minute=i1+i2;
delay(500);
lcd.clear();
mode=1;
//String Hon =  String(A_hour);
//String Mon = String(A_minute);
//String Hoff = String(B_hour);
//String Moff = String(B_minute);
            EEPROM.write(10,A_hour);
            EEPROM.write(11,A_minute);
            EEPROM.write(12,B_hour);
            EEPROM.write(13,B_minute);
keypressed=NO_KEY;
             }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ManualMode ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////             
void ManualMode(){
mode=0;
lcd.clear();
lcd.setCursor(1, 0);
lcd.print("1 : on");
lcd.setCursor(1, 1);
lcd.print("0 : off");

char keypressed22='z';

while(1){
  keypressed22=myKeypad.waitForKey();
  if(keypressed22 =='0' ||  keypressed22 =='1' ||keypressed22 =='A'|| keypressed22 =='B'|| keypressed22 =='C' ||keypressed22 =='D'  )break;
}
if(keypressed22 =='A'|| keypressed22 =='B'|| keypressed22 =='C' ||keypressed22 =='D' )return;
if(keypressed22=='1'){
           digitalWrite(7, HIGH);  
           digitalWrite(A3, HIGH); 
           lcd.clear(); lcd.print("ACTIVATION ..");delay(1000);lcd.clear();
           lcd.setCursor(16,2);
           lcd.print("On ");
           Mstatus = "On ";
  }else
if(keypressed22=='0'){
    digitalWrite(7, LOW);  
           digitalWrite(A3, LOW); 
           lcd.clear();lcd.clear(); lcd.print("DESACTIVATION ..");delay(1000);lcd.clear();
           lcd.setCursor(16,2);
           lcd.print("OFF ");
           Mstatus = "Off";
  
  }



keypressed=NO_KEY;
  }
