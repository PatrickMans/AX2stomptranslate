/*
First trial to get a midi controller
Needs:
- LCD display
- Midi interface (build)
 */

 
// include the library
#include <LiquidCrystal.h>

// initialize the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
int PATCH = 0;
int OLDPATCH;
int SYSEX1 = 0xF0;
int SYSEX2 = 0xF7;
int ID1 = 0x00;
int ID2 = 0x01;
int ID3 = 0x0C;
int IDAX = 00;
int PAREDIT = 0x04;
int CC = 0xB0;
int PP = 0xC0;
int SWITCH = 0x81; 
byte commandByte;
byte noteByte;
byte velocityByte;
int St;
int Mo;
int Dl;
int Rv;

#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
 
// the 8 arrays that form each segment of the custom numbers
byte LT[8] = 
{
 B00111,
 B01111,
 B11111,
 B11111,
 B11111,
 B11111,
 B11111,
 B11111
};
byte UB[8] =
{
 B11111,
 B11111,
 B11111,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000
};
byte RT[8] =
{
 B11100,
 B11110,
 B11111,
 B11111,
 B11111,
 B11111,
 B11111,
 B11111
};
byte LL[8] =
{
 B11111,
 B11111,
 B11111,
 B11111,
 B11111,
 B11111,
 B01111,
 B00111
};
byte LB[8] =
{
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111,
 B11111
};
byte LR[8] =
{
 B11111,
 B11111,
 B11111,
 B11111,
 B11111,
 B11111,
 B11110,
 B11100
};
byte UMB[8] =
{
 B11111,
 B11111,
 B11111,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111
};
byte LMB[8] =
{
 B11111,
 B00000,
 B00000,
 B00000,
 B00000,
 B11111,
 B11111,
 B11111
};
byte CLR[8] =
{
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000
};
// loop counter
int x = 0;

void custom0()
{ // uses segments to build the number 0
 lcd.setCursor(x, 0); // set cursor to column 0, line 0 (first row)
 lcd.write(byte(0));  // call each segment to create
 lcd.write(1);  // top half of the number
 lcd.write(2);
 lcd.setCursor(x, 1); // set cursor to colum 0, line 1 (second row)
 lcd.write(3);  // call each segment to create
 lcd.write(4);  // bottom half of the number
 lcd.write(5);
}

void custom1()
{
 lcd.setCursor(x,0);
 lcd.write(1);
 lcd.write(2);
 lcd.setCursor(x+1,1);
 lcd.write(5);
}

void custom2()
{
 lcd.setCursor(x,0);
 lcd.write(6);
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(x, 1);
 lcd.write(3);
 lcd.write(7);
 lcd.write(7);
}

void custom3()
{
 lcd.setCursor(x,0);
 lcd.write(6);
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(x, 1);
 lcd.write(7);
 lcd.write(7);
 lcd.write(5); 
}

void custom4()
{
 lcd.setCursor(x,0);
 lcd.write(3);
 lcd.write(4);
 lcd.write(2);
 lcd.setCursor(x+2, 1);
 lcd.write(5);
}

void custom5()
{
 lcd.setCursor(x,0);
 lcd.write(byte(0));
 lcd.write(6);
 lcd.write(6);
 lcd.setCursor(x, 1);
 lcd.write(7);
 lcd.write(7);
 lcd.write(5);
}

void custom6()
{
 lcd.setCursor(x,0);
 lcd.write(byte(0));
 lcd.write(6);
 lcd.write(6);
 lcd.setCursor(x, 1);
 lcd.write(3);
 lcd.write(7);
 lcd.write(5);
}

void custom7()
{
 lcd.setCursor(x,0);
 lcd.write(1);
 lcd.write(1);
 lcd.write(2);
 lcd.setCursor(x+2, 1);
 lcd.write(2);
}

void custom8()
{
 lcd.setCursor(x,0);
 lcd.write(byte(0));
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(x, 1);
 lcd.write(3);
 lcd.write(7);
 lcd.write(5);
}

void custom9()
{
 lcd.setCursor(x,0);
 lcd.write(byte(0));
 lcd.write(6);
 lcd.write(2);
 lcd.setCursor(x+2, 1);
 lcd.write(5);
}

void WritePatchNR(int PATCH) {
int High;
int Mid;
int Low;
x = 0;
lcd.setCursor(0,0);
lcd.write("      ");
lcd.setCursor(0,1);
lcd.write("      ");

if (PATCH < 10)
{
  High = 0;
  Mid = 0;
}
if (PATCH < 100 && PATCH > 10)
{ 
  High = 0; 
  Mid = int(PATCH/10);
};
if (PATCH > 99)
{
  High = 1;
  Mid = int((PATCH - 100)/10);
}

switch(High){
  case 0:
     custom0();
  break;
  case 1:
     custom1();
  break;
}
x = x + 3;

switch(Mid){
  case 0:
     custom0();
  break;
  case 1:
     custom1();
  break;
  case 2:
     custom2();
  break;
  case 3:
     custom3();
  break;
  case 4:
     custom4();
  break;
  case 5:
     custom5();
  break;
  case 6:
     custom6();
  break;
  case 7:
     custom7();
  break;
  case 8:
     custom8();
  break;
  case 9:
     custom9();
  break;
  }
x = x + 3;

Low = int(PATCH - (Mid * 10));
switch(Low){
  case 0:
     custom0();
  break;
  case 1:
     custom1();
  break;
  case 2:
     custom2();
  break;
  case 3:
     custom3();
  break;
  case 4:
     custom4();
  break;
  case 5:
     custom5();
  break;
  case 6:
     custom6();
  break;
  case 7:
     custom7();
  break;
  case 8:
     custom8();
  break;
  case 9:
     custom9();
  break;
  }  
}

void WriteTopLine(String TEXT) {
lcd.setCursor(10,0);
lcd.print("             ");
lcd.print(TEXT);  
}

void WriteBotLine(String TEXT) {
lcd.setCursor(10,1);
lcd.print("             ");
lcd.print(TEXT);  
}

//  Sends the CC command via sysex to AX2
void SendSysex(int STMP, int STATUS) {
  Serial.write(SYSEX1);
  Serial.write(ID1);
  Serial.write(ID2);
  Serial.write(ID3);
  Serial.write(IDAX);
  Serial.write(PAREDIT);
  Serial.write(STMP);
  Serial.write(STATUS);
  Serial.write(0x00);
  Serial.write(SYSEX2);
}

// sends CC value other than Sysex ones to AX2
void SendCC(int NR,int VAL) {
  Serial.write(CC);
  Serial.write(NR);
  Serial.write(VAL);
}

// Sends progchange command to AX
void SendPP(int PRG) {
  Serial.write(PP);
  Serial.write(PRG);
}

void SendMidi(int DATA) {
  Serial.write(DATA);
}

int checkMIDI(){
  do{
    if (Serial.available()){
      commandByte = Serial.read();//read first byte
    }
  }
  while (Serial.available() > 2);//when at least three bytes available
}

void DecodeSwitch(int CCCODE, int VAL) {
int SWCODE = 0;
int SENDSYSEX = 0;

      switch(CCCODE){
      case 0x20:{
            SWCODE = 0x09;
            SENDSYSEX = 1;
       };
       case 0x21:{
            SWCODE = 0x19;
            SENDSYSEX = 1;
       };
       case 0x22:{
            SWCODE = 0x29;
            SENDSYSEX = 1;
       };
       case 0x23:{
            SWCODE = 0x39;
            SENDSYSEX = 1;
       };
       case 0x24:{
            SWCODE = 0x49;
            SENDSYSEX = 1;
       };
       case 0x25:{
            SWCODE = 0x59;
            SENDSYSEX = 1;
       };
       case 0x26:{
            SWCODE = 0x32;
            SENDSYSEX = 1;
       };
      }
      if (SENDSYSEX = 1) {
          SendSysex(SWCODE, VAL);
      }
      else
      {
          SendCC(CCCODE, VAL);
      };
      DispCode(CCCODE, VAL);
}

void DispCode(int CODE, int VAL){
x = 10;
String SWTCHNAME = "CC";
switch(CODE){
  case 0x20:{
      SWTCHNAME = "Gain";
  };
  case 0x21:{
      SWTCHNAME = "Stomp";
  };
  case 0x22:{
      SWTCHNAME = "EQ";
  };
  case 0x23:{
      SWTCHNAME = "Chorus";
  };
  case 0x24:{
      SWTCHNAME = "Delay";
  };
  case 0x25:{
      SWTCHNAME = "Reverb";
  };
  case 0x26:{
      SWTCHNAME = "WAH";
  };
 }
// WriteTopLine("midi in");
 WriteBotLine("SWITCHNAME");

}
  
void setup() {
 // assignes each segment a write number
 lcd.createChar(0,LT);
 lcd.createChar(1,UB);
 lcd.createChar(2,RT);
 lcd.createChar(3,LL);
 lcd.createChar(4,LB);
 lcd.createChar(5,LR);
 lcd.createChar(6,UMB);
 lcd.createChar(7,LMB);
  
 // sets the LCD's rows and colums:
 lcd.begin(16, 2);
 PATCH = 0;
 OLDPATCH = 3;
 WritePatchNR(PATCH); 
 //  Set MIDI baud rate:
 Serial.begin(31250);
}

void loop() {

int CNTRL;
int VALUE;
int PROGNR;

    if (Serial.available()){
      commandByte = Serial.read();//read first byte
      WriteTopLine("byte in "+commandByte);
    }
       
   switch (commandByte){
       case 0xB0:{
             CNTRL = Serial.read();
             VALUE = Serial.read();
             DecodeSwitch(CNTRL, VALUE);
            break;
       }
       case 0xC0:{
             PROGNR = Serial.read();
             SendPP(PROGNR);
             WritePatchNR(PROGNR);
             break;
       }    
       default:{
             SendMidi(commandByte);
             break;
       }
     }
   }
