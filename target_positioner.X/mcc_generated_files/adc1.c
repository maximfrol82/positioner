/**
  ADC1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    adc1.c

  @Summary
    This is the generated driver implementation file for the ADC1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for ADC1.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  dsPIC33EP256GP506      
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include "adc1.h"

/**
  Section: File Specific Functions
*/

// ADC1 Default Interrupt Handler
static void (*ADC1_DefaultInterruptHandler)(void) = NULL;

/**
  Section: Driver Interface
*/

void ADC1_Initialize (void)
{
    // ASAM disabled; ADDMABM disabled; ADSIDL disabled; DONE disabled; SIMSAM Sequential; FORM Absolute decimal result, unsigned, right-justified; SAMP disabled; SSRC TMR3; AD12B 12-bit; ADON enabled; SSRCG disabled; 
    AD1CON1 = 0x8440;
    // CSCNA disabled; VCFG0 AVDD; VCFG1 AVSS; ALTS disabled; BUFM disabled; SMPI Generates interrupt after completion of every sample/conversion operation; CHPS 1 Channel; 
    AD1CON2 = 0x00;
    // SAMC 2; ADRC FOSC/2; ADCS 9; 
    AD1CON3 = 0x209;
    // CH0SA AN0; CH0SB AN0; CH0NB AVSS; CH0NA AVSS; 
    AD1CHS0 = 0x00;
    // CSS26 disabled; CSS25 disabled; CSS24 disabled; CSS31 enabled; CSS30 enabled; 
    AD1CSSH = 0xC000;
    // CSS9 enabled; CSS8 disabled; CSS7 disabled; CSS6 disabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; CSS2 disabled; CSS15 disabled; CSS1 enabled; CSS14 disabled; CSS0 enabled; CSS13 disabled; CSS12 disabled; CSS11 disabled; CSS10 enabled; 
    AD1CSSL = 0x603;
    // DMABL Allocates 16 words of buffer to each analog input; ADDMAEN enabled; 
    AD1CON4 = 0x104;
    // CH123SA disabled; CH123SB CH1=OA2/AN0,CH2=AN1,CH3=AN2; CH123NA disabled; CH123NB CH1=VREF-,CH2=VREF-,CH3=VREF-; 
    AD1CHS123 = 0x00;
    
    //Assign Default Callbacks
    ADC1_SetInterruptHandler(&ADC1_CallBack);
   
}

void __attribute__ ((weak)) ADC1_CallBack(void)
{

}

void ADC1_SetInterruptHandler(void* handler)
{
    ADC1_DefaultInterruptHandler = handler;
}

void __attribute__ ((weak)) ADC1_Tasks ( void )
{
    if(IFS0bits.AD1IF)
    {
        if(ADC1_DefaultInterruptHandler) 
        { 
            ADC1_DefaultInterruptHandler(); 
        }

        // clear the ADC interrupt flag
        IFS0bits.AD1IF = false;
    }
}

/**
  End of File
*/

