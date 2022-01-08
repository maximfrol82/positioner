/*
 * File:   eepromI2C.c
 * Author: maxim
 *
 * Created on September 16, 2021, 9:43 PM
 */

#include "xc.h"
#include "eepromI2C.h"
#include "mcc_generated_files/delay.h"
 
void I2CInit(void)
{
    I2C2BRG = EEPROMbaud;           // @400kHz; (FCY/FSCL - FCY/1e7) - 1
    I2C2CONbits.I2CEN = 0;  // Disable I2C
    I2C2CONbits.DISSLW = 1; // Disable slew rate control
    I2C2CONbits.A10M = 0;   // 7-bit slave addr
    I2C2CONbits.SCLREL = 1; // SCL release control
    I2C2CONbits.I2CEN = 1;  // Enable I2C
    IEC3bits.MI2C2IE = 0;   // Master I2C interrupt
    IFS3bits.MI2C2IF = 0;   // MI2C Flag
}
 
void I2CAck(void)
{
    I2C2CONbits.ACKDT = 0;      // Send ACK
    I2C2CONbits.ACKEN = 1;      // Initiate Acknowledge and transmit ACKDT
    while(I2C2CONbits.ACKEN);
}
 
void I2CNack(void)
{
    I2C2CONbits.ACKDT = 1;      // Send NACK
    I2C2CONbits.ACKEN = 1;      // Initiate Acknowledge and transmit ACKDT
    while(I2C2CONbits.ACKEN);  
}
 
void I2CStop(void)
{
    I2C2CONbits.RCEN = 0;       // receive mode not in progress
    I2C2CONbits.PEN = 1;        // Stop condition
    while(I2C2CONbits.PEN);
}
 
void I2CStart(void)
{
    I2C2CONbits.ACKDT = 0;      // Reset any ACK
    I2C2CONbits.SEN = 1;        // Start
    while(I2C2CONbits.SEN);
}
 
void I2CRestart(void)
{
    I2C2CONbits.RSEN = 1;       // Repeated Start Condition
    while(I2C2CONbits.RSEN);
    I2C2CONbits.ACKDT = 0;      // Send ACK
    I2C2STATbits.TBF = 0;       // I2C2TRN is empty
}
 
void I2CWaitACK(void)
{
    while(I2C2STATbits.ACKSTAT);
}
 
void I2CIdle(void)
{
    while(I2C2STATbits.TRSTAT);
}
 
void I2CWrite(unsigned char c)
{
    I2C2TRN = c;
    while(I2C2STATbits.TBF);
}
 
void I2CRead(void)
{
    I2C2CONbits.RCEN = 1;
    Nop();
    while(!I2C2STATbits.RBF);
}
////////////////////////////////////////////////////////////////////////////////
void I2CWriteReg(unsigned int addr, char value)
{
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((EEPROMaddr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite((char)addr>>8);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(addr & 0x00FF);
    I2CIdle();
    // Value
    I2CWrite(value);
    I2CIdle();
    // Stop
    I2CStop();
}
////////////////////////////////////////////////////////////////////////////////
unsigned char I2CReadReg(unsigned int addr)
{
    char temp;
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((EEPROMaddr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite(addr >> 8);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(addr & 0x00FF);
    I2CIdle();
    // Restart
    I2CRestart();
    I2CWrite((EEPROMaddr<<1)|0x01);
    I2CIdle();  
    I2CRead();
    I2CNack();
    I2CStop();
    temp = I2C2RCV;
    return temp;
}
 
void I2CSequentialWriteReg(char addr, char byteHigh, char byteLow, char* value, int length)
{
    int j;
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite(byteHigh);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(byteLow);
    I2CIdle();
    // Value
    for(j = 0; j < length; j++)
    {
        I2CWrite(value[j]);
        I2CIdle();
    }
    // Stop
    I2CStop();    
}
 
void I2CSequentialReadReg(char addr, char byteHigh, char byteLow, char* buff, int length)
{
    int j;
    // Start Condition
    I2CStart();
    // EEPROM Addr
    I2CWrite((addr<<1)&0xFE);
    I2CIdle();
    // Addr High Byte
    I2CWrite(byteHigh);
    I2CIdle();
    // Addr Low Byte
    I2CWrite(byteLow);
    I2CIdle();
    // Restart
    I2CRestart();
    I2CWrite((addr<<1)|0x01);
    I2CIdle();  
    for(j = 0; j < length; j++)
    {
        I2CRead();
        buff[j] = I2C2RCV;  
        I2CAck();
    }
    buff[j] = '\0';
    I2CNack();
    I2CStop();  
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Write_INT_EE(unsigned int Adr, int cx){
char hi, lo;
hi = cx >> 8;
lo = cx & 0x00FF;
I2CWriteReg(Adr, hi);
DELAY_milliseconds(5);
I2CWriteReg(Adr + 1, lo);
DELAY_milliseconds(5);
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int Read_INT_EE(unsigned int Adr){
int c3;
char hi, lo;
hi = I2CReadReg(Adr);
DELAY_milliseconds(5);
lo = I2CReadReg(Adr + 1);
DELAY_milliseconds(5);
c3 = (hi<<8) + lo;
return c3;
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////