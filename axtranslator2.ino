#include <MIDI.h>
/*
* ********************************************************
* The LINE6 AX212 amp cannot receive normal ControlChanges.
* The goal is to control the amp with a Behringer FCB1010
*
* The stomp boxes are set to CC 20 - 26 for:
* 20 - Gain
* 21 - Distortion
* 22 - EQ
* 23 - Chorus/Tremolo
* 24 - Delay
* 25 - Reverb
* 26 - Wah 
*
* Needs:
* - LCD display
* - Midi interface (build using midimate schematic)
* Using the Arduino MIDI library 
* ********************************************************
*/

MIDI_CREATE_DEFAULT_INSTANCE();
 
// include the library
#include <LiquidCrystal.h>

// initialize the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
int PATCH = 0;
int OLDPATCH = 127;
int OLDCODE = 0;
byte SYSEX1 = 0xF0;
byte SYSEX2 = 0xF7;
byte ID1 = 0x00;
byte ID2 = 0x01;
byte ID3 = 0x0C;
byte IDAX = 0x00;
byte PAREDIT = 0x04;
byte CC = 0xB0;
byte PP = 0xC0;
byte SWITCH = 129; 
byte commandByte;
byte noteByte;
byte velocityByte;
int St;
int Mo;
int Dl;
int Rv;
/*
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
*/
 
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
x = 10;
String PATCHSTR = String(PATCH);
WriteTopLine("Prog    ");
WriteBotLine("      ");
WriteBotLine(PATCHSTR);

x = 0;
lcd.setCursor(x,0);
lcd.write("         ");
lcd.setCursor(x,1);
lcd.write("         ");

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
x = 0;
switch(High){
  case 0:
     custom0();
  break;
  case 1:
     custom1();
  break;
}
x = 3;
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
x=6;
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
 OLDPATCH = PATCH;
} 

void WriteTopLine(String TEXT) {
//x = 10;
lcd.setCursor(x,0);
lcd.print(TEXT);  
}

void WriteBotLine(String TEXT) {
//x = 10;
lcd.setCursor(x,1);
lcd.print(TEXT);  
}

//  Sends the CC command via sysex to AX2
void SendSysex(byte STMP, byte STATUS) {
    
  byte SYSEX[] = {ID1, ID2, ID3, IDAX, PAREDIT, STMP, STATUS, 0};
  MIDI.sendSysEx(8, SYSEX, false);
}

// sends CC value other than Sysex ones to AX2
void SendCC(byte NR,byte VAL, byte CHAN) {
   MIDI.sendControlChange(NR, VAL, CHAN);
}

// Sends progchange command to AX
void SendPP(byte CHAN, byte PRG) {
  MIDI.sendProgramChange(PRG, CHAN);
  WritePatchNR(PRG);
}

void SendMidi(byte DATA) {
  Serial.write(DATA);
}

void DecodeSwitch(byte CHAN, byte CCCODE, byte VAL) {
byte SWCODE = 0;
int SENDSYSEX = 0;
String SWTCHNAME;
String DISPVAL;
      switch(CCCODE){
      case 0x03:{
            SWCODE = 0x03;
            SENDSYSEX = 0;
            SWTCHNAME = "PDL";
            break;
       };
      case 0x07:{
            SWCODE = 0x07;
            SENDSYSEX = 0;
            SWTCHNAME = "VOL";
            break;
       };
      case 0x20:{
            SWCODE = 0x09;
            SENDSYSEX = 1;
            SWTCHNAME = "GN";
            break;
       };
       case 0x21:{
            SWCODE = 0x19;
            SENDSYSEX = 1; 
            SWTCHNAME = "DS";
            break;
       };
       case 0x22:{
            SWCODE = 0x29;
            SENDSYSEX = 1;
            SWTCHNAME = "EQ";
            break;
       };
       case 0x23:{
            SWCODE = 0x39;
            SENDSYSEX = 1;
            SWTCHNAME = "CH";
            break;
       };
       case 0x24:{
            SWCODE = 0x49;
            SENDSYSEX = 1;
            SWTCHNAME = "DL";
            break;
       };
       case 0x25:{
            SWCODE = 0x59;
            SENDSYSEX = 1;
            SWTCHNAME = "RV";
            break;
       };
       case 0x26:{
            SWCODE = 0x32;
            SENDSYSEX = 1;
            SWTCHNAME = "WH";
            break;
       };
       default:{
            SWTCHNAME = "CC";
            SENDSYSEX = 1;
            SWCODE = 0x19;
            break;
       }
      }
      x = 10;
      if (SENDSYSEX == 1) {
          SendSysex(SWCODE, VAL);
          WriteTopLine("Stomp   ");
      }
      else
      {
          SendCC(CCCODE, VAL, CHAN);
          WriteTopLine("Cntrl  ");
      };
      if (VAL == 1 && SENDSYSEX == 1) {
        DISPVAL = "On ";
      } else {
        DISPVAL = "Off";
      }
      if (SENDSYSEX == 0){
        DISPVAL = String(VAL);
      }
      x = 10;
      WriteBotLine("       ");
      WriteBotLine(SWTCHNAME);
      x = 13;
      WriteBotLine(DISPVAL);
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
 pinMode(13,OUTPUT);
 digitalWrite(13,LOW);
 // sets the LCD's rows and colums:
 lcd.begin(16, 2);
 WritePatchNR(PATCH); 
 //  Set MIDI channel:
 MIDI.setHandleControlChange(DecodeSwitch);
 MIDI.setHandleProgramChange(SendPP);
 MIDI.begin();
}

void loop() {
MIDI.read();
}
