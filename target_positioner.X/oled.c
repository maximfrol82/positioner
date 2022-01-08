/*
 * File:   oled.c
 * Author: maximfrol82
 *
 * Created on September 11, 2021, 12:51 PM
 */


#include "oled.h"
#include <string.h>
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/clock.h"
#include <stdlib.h>
#include "mcc_generated_files/delay.h"
const char SHRIFT[];

////////////////////////////////////////////////////////////////////////////////
void write_port_oled(unsigned char port)
{
LATCbits.LATC0  = (port&(1<<0)? 1:0);
LATCbits.LATC1  = (port&(1<<1)? 1:0);
LATCbits.LATC2  = (port&(1<<2)? 1:0);
LATCbits.LATC3  = (port&(1<<3)? 1:0);
LATCbits.LATC4  = (port&(1<<4)? 1:0);
LATCbits.LATC5  = (port&(1<<5)? 1:0);
LATCbits.LATC6  = (port&(1<<6)? 1:0);
LATCbits.LATC7  = (port&(1<<7)? 1:0);
}//
//==============================================================================
void lcd_com(unsigned char p){
rs_SetLow();//LCD_RS = 0;
e_SetHigh();//LCD_EN = 1;
write_port_oled(p); // DB0...7 LCD
DELAY_microseconds(500);//DELAY_milliseconds
e_SetLow();//LCD_EN = 0;
DELAY_microseconds(500);
}
//==============================================================================
//******************************************************************************
void lcd_dat(unsigned char p){
rs_SetHigh();//LCD_RS = 1;
e_SetHigh();//LCD_EN = 1;
write_port_oled(p); // DB0...7 LCD
DELAY_microseconds(500);
e_SetLow();//LCD_EN = 0;
DELAY_microseconds(500);
}
//******************************************************************************
//------------------------------------------------------------------------------
void _lcd_init(void){
write_port_oled(0); // 
lcd_com(0x08);      // 
DELAY_microseconds(100);
lcd_com(0x3A);      // 
DELAY_microseconds(100);
lcd_com(0x3A);      // 
DELAY_microseconds(100);
lcd_com(0x3A);      // 
DELAY_microseconds(100);
lcd_com(0x01);      // 
DELAY_microseconds(100);
lcd_com(0x06);      // 
DELAY_microseconds(100);
lcd_com(0x0C);      // 
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ClearLCD(void){
lcd_com(0x01);        // send 0x01
DELAY_microseconds(200);
lcd_com(0x01);        // send 0x01
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//void CursorLCD
// * Author: Gena Chernov - http://catcatcat.d-lan.dp.ua/
// * Catcatcat
// * catcatcat@e-mail.ua
void CursorLCD(unsigned char stroka, unsigned char stolbec){
if(0b00000010 & stroka) stolbec += 20;
if(0b00000001 & stroka) stolbec |= 0b11000000;
else stolbec|= 0b10000000;
lcd_com(stolbec);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//void StringLCD
// * Author: Gena Chernov - http://catcatcat.d-lan.dp.ua/
// * Catcatcat
// * catcatcat@e-mail.ua
void StringLCD(char *str){
unsigned char simbol;
unsigned int ptr = 0;

        while(str[ptr] != 0){
        simbol = str[ptr++];
 if(simbol>191){simbol = SHRIFT[(simbol-192)];} //
        lcd_dat(simbol);// 
        }
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void pprint4(int inTXT, unsigned char pX, unsigned char pY ){
char text[] = "0000";
     if ((inTXT <= 9999)&&(inTXT >= 1000) ){

     text[0] = (inTXT/1000)%10  + 48;
     text[1] = (inTXT/100)%10  + 48;
     text[2] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[3] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 999)&&(inTXT >= 100) ){
     text[0] = 32;
     text[1] = (inTXT/100)%10  + 48;
     text[2] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[3] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 99)&&(inTXT >= 10) ){
     text[0] = 32;
     text[1] = 32;
     text[2] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[3] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 9)&&(inTXT >= 0) ){
     text[0] = 32;
     text[1] = 32;
     text[2] = 32;    // Extract tens digit
     text[3] =  inTXT%10       + 48;    // Extract ones digit
     }

CursorLCD(pX,pY);
StringLCD(text);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void pprint3(int inTXT, unsigned char pX, unsigned char pY ){
char text[] = "000";

     if ((inTXT <= 999)&&(inTXT >= 100) ){
     text[0] = (inTXT/100)%10  + 48;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 99)&&(inTXT >= 10) ){
     text[0] = 32;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 9)&&(inTXT >= 0) ){
     text[0] = 32;
     text[1] = 32;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }
CursorLCD(pX,pY);
StringLCD(text);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void PrintINT(int inINT, unsigned char pX, unsigned char pY ){
char text[] = "000.0";
unsigned int inTXT;
inTXT = inINT/10;
     if ((inTXT <= 999)&&(inTXT >= 100) ){
     text[0] = (inTXT/100)%10  + 48;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     text[4] =  inINT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 99)&&(inTXT >= 10) ){
     text[0] = 32;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     text[4] =  inINT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 9)&&(inTXT >= 0) ){
     text[0] = 32;
     text[1] = 32;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     text[4] =  inINT%10       + 48;    // Extract ones digit
     }
    if ((inINT < 0)&&(inINT > -990) ){
     text[0] = 45;
     text[1] = (abs(inTXT)/10)%10   + 48;    // Extract tens digit
     text[2] =  abs(inTXT)%10       + 48;    // Extract ones digit
     text[4] =  abs(inINT)%10       + 48;    // Extract ones digit
     }
CursorLCD(pX,pY);
StringLCD(text);
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void pprint2( unsigned int inTXT, unsigned char pX, unsigned char pY ){
char text[] = "00";

     if ((inTXT <= 99)&&(inTXT >= 10) ){
     text[0] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[1] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 9)&&(inTXT >= 0) ){
     text[0] = 32;    // Extract tens digit
     text[1] =  inTXT%10       + 48;    // Extract ones digit
     }
 CursorLCD(pX,pY);
StringLCD(text);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void pprint8bin( char inBIN, unsigned char pX, unsigned char pY ){
char text[] = "00000000";
text[7] = (inBIN&(1<<0)? 49:48);
text[6] = (inBIN&(1<<1)? 49:48);
text[5] = (inBIN&(1<<2)? 49:48);
text[4] = (inBIN&(1<<3)? 49:48);
text[3] = (inBIN&(1<<4)? 49:48);
text[2] = (inBIN&(1<<5)? 49:48);
text[1] = (inBIN&(1<<6)? 49:48);
text[0] = (inBIN&(1<<7)? 49:48);

CursorLCD(pX,pY);
StringLCD(text);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void print_KIS_TE(int inTXT, char kis, unsigned char pX, unsigned char pY ){
char text[] = "000";

if(kis==0){
     if ((inTXT <= 999)&&(inTXT >= 100) ){
     text[0] = (inTXT/100)%10  + 48;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 99)&&(inTXT >= 10) ){
     text[0] = 32;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 9)&&(inTXT >= 0) ){
     text[0] = 32;
     text[1] = 32;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }
}
if(kis==1){text[0] = 'L';text[1] = 'O';text[2] = '.';}
if(kis==2){text[0] = 'H';text[1] = 'I';text[2] = '.';}

CursorLCD(pX,pY);
StringLCD(text);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void print_A(int inTXT, char kis, unsigned char pX, unsigned char pY ){
char text[] = "000";

if(inTXT < 0)inTXT = 0;
if(inTXT > 1000)inTXT = 999;

if(kis==0){
     if ((inTXT <= 999)&&(inTXT >= 100) ){
     text[0] = (inTXT/100)%10  + 48;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 99)&&(inTXT >= 10) ){
     text[0] = 32;
     text[1] = (inTXT/10)%10   + 48;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }

     if ((inTXT <= 9)&&(inTXT >= 0) ){
     text[0] = 32;
     text[1] = 32;    // Extract tens digit
     text[2] =  inTXT%10       + 48;    // Extract ones digit
     }
}
else{text[0] = 'e';text[1] = 'r';text[2] = 'r';}

CursorLCD(pX,pY);
StringLCD(text);
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////  
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
const char SHRIFT[] = {
                0x41,        // 0xC0        ?
                0xA0,        // ?
                0x42,        // ?
                0xA1,        // ?
                0xE0,        // ?
                0x45,        // ?
                0xA3,        // 0??6 ?
                0xA4,        // ?
                0xA5,        // ?
                0xA6,        // ?
                0x4B,        // ?
                0xA7,        // ?
                0x4D,        // ?
                0x48,        // ?
                0x4F,        // ?
                0xA8,        // ?
                0x50,        // ?
                0x43,        // ?
                0x54,        // ?
                0xA9,        // ?
                0xAA,        // ?
                0x58,        // ?
                0xE1,        // ?
                0xAB,        // ?
                0xAC,        // ?
                0xE2,        // ?
                0xAD,        // ?
                0xAE,        // ?
                0x62,        // ?
                0xAF,        // ?
                0xB0,        // ?
                0xB1,        // ?

                0x61,        // ?
                0xB2,        // ?
                0xB3,        // ?
                0xB4,        // ?
                0xE3,        // ?
                0x65,        // ?
                0xB6,        // ?
                0xB7,        // ?
                0xB8,        // ?
                0xB9,        // ?
                0xBA,        // ?
                0xBB,        // ?
                0xBC,        // ?
                0xBD,        // ?
                0x6F,        // o
                0xBE,        // ?
                0x70,        // ?
                0x63,        // ?
                0xBF,        // ?
                0x79,        // ?
                0xE4,        // ?
                0x78,        // ?
                0xE5,        // ?
                0xC0,        // ?
                0xC1,        // ?
                0xE6,        // ?
                0xC2,        // ?
                0xC3,        // ?
                0xC4,        // ?
                0xC5,        // ?
                0xC6,        // ?
                0xC7,        // ?
};