/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/watchdog.h"
#include "oled.h"
#include "mcc_generated_files/delay.h"
#include "eepromI2C.h"
#include "mcc_generated_files/tmr5.h"
#include <stdlib.h>
#include <p33EP256GP506.h>
#include "adc1.h"
#include "analog.h"
#include "eepromI2C.h"

uint16_t ADCValue;
uint16_t count;

#define SAMP_BUFF_SIZE  16           // Size of the input buffer per analog input
#define NUM_CHS2SCAN    4           // Number of channels enabled for channel scan

// Buffer declaration to collect data from various ADC buffers
int ain0Buff[SAMP_BUFF_SIZE];
int ain1Buff[SAMP_BUFF_SIZE];
int ain9Buff[SAMP_BUFF_SIZE];
int ain10Buff[SAMP_BUFF_SIZE];
int scanCounter = 0;
int sampleCounter = 0;

double pww;
int rt, tr;
int opp;
////////////////////////////////////////////////////////////////////////////////
int ib, fi;
char flag_DI_change, flag_BTN_change;
char ALARM, hand_alarm;
////////////////////////////////////////////////////////////////////////////////
char old_btn_L, old_btn_C, old_btn_R;
char btn_L, btn_C, btn_R;
char tune, upd_value;
char Menu, old_menu, cnt_Timer_screen, olds_menu;
char Menu_changE,  Timer_screen, tune_oper;
char hand_run_bind;
char generator;
int ZPG1_set, ZPG2_set;
char cnt_bind, status_DO;
int EN;
int cnt_main;
unsigned int cnt_menu, cik_dcdc;
unsigned int dat_Iset, Iset, kis_Iset;
unsigned int int_test, dat_Upower, Upower;
unsigned int dat_FB1, Angle1;
char kis_FB1;
unsigned int dat_FB2, Angle2;
char kis_FB2;
char DO_K1, DO_K2, DO_K3, DO_K4;
char DO_alarm;
char err_R1,err_R2,err_sum, err_minmax, find_err_R;
unsigned int pwm1, pwm2, cnt_pwm;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef struct T{
int Thoda;

int q_ZPGV;
int type_DRIVE;
int deadBAND;

int R1_min;
int R1_max;
int R2_min;
int R2_max;
int mem_LRC;
}param;
//------------------------------------------------------------------------------
int Thoda;

int q_ZPGV;
int type_DRIVE;
int deadBAND;

int R1_min;
int R1_max;
int R2_min;
int R2_max;
int mem_LRC;
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
enum addr_EE {
addr_Thoda = 0,
addr_q_ZPGV = 2,
addr_type_DRIVE = 4,
addr_deadBAND = 6,
addr_R1_min = 8,
addr_R1_max = 10,
addr_R2_min = 12,
addr_R2_max = 14,
addr_mem_LRC = 16
};
/******************************************************************************/
struct TON_timer{
char runs;
unsigned int PT;
unsigned int wait_ON;
char Q;
}TONS[20];
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//******************************************************************************
void ISR_T5 (void){
 if(Menu >= 20){cnt_menu++;}else{cnt_menu = 0;}
 if(cnt_menu > 9200){tune_oper = 0; flag_BTN_change = 1;}

 cnt_Timer_screen++;
 if(cnt_Timer_screen > 40){Timer_screen = 1; cnt_Timer_screen = 0;}
 
    for(fi=0;fi<=16;fi++){
         if(TONS[fi].runs == 0){TONS[fi].Q = 0; TONS[fi].wait_ON = 0;}
            if(TONS[fi].Q == 0 && TONS[fi].runs > 0){
            TONS[fi].wait_ON++;
            if(TONS[fi].wait_ON >= TONS[fi].PT){TONS[fi].Q = 1;}
            }
         }   
}//
//******************************************************************************
//******************************************************************************
void ISR_CN(void){
flag_BTN_change = 1;
btn_L = btnL_GetValue()?1:0;
btn_C = btnC_GetValue()?1:0;
btn_R = btnR_GetValue()?1:0;  
}//
//******************************************************************************
//******************************************************************************
void PWM1 (uint8_t percent){
    if(percent > 100)percent = 100;
    if(percent < 0)percent = 0;
uint32_t pwmu = percent;
    pwmu = (pwmu * OC1RS)/100;
    OC1R = (uint16_t)pwmu;
}//
//******************************************************************************
void PWM2 (uint8_t percent){
    if(percent > 100)percent = 100;
    if(percent < 0)percent = 0;
uint32_t pwmu = percent;
    pwmu = (pwmu * OC1RS)/100;
    OC2R = (uint16_t)pwmu;
}//
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////ReadALL_EE();
void ReadALL_EE(void){
Thoda = Read_INT_EE(addr_Thoda);
q_ZPGV = Read_INT_EE(addr_q_ZPGV);;
type_DRIVE = Read_INT_EE(addr_type_DRIVE);;
deadBAND = Read_INT_EE(addr_deadBAND);
R1_min = Read_INT_EE(addr_R1_min);
R1_max = Read_INT_EE(addr_R1_max);
R2_min = Read_INT_EE(addr_R2_min);
R2_max = Read_INT_EE(addr_R2_max);
mem_LRC = Read_INT_EE(addr_mem_LRC);

if(mem_LRC != 12345){
Thoda = 33;
q_ZPGV = 2;
type_DRIVE = 1;
deadBAND = 3;
R1_min = 100;
R1_max = 1000;
R2_min = 100;
R2_max = 1000;
}

}//
////////////////////////////////////////////////////////////////////////////////
void WriteAll_EE(void){
Write_INT_EE(addr_Thoda, Thoda);
Write_INT_EE(addr_q_ZPGV, q_ZPGV);
Write_INT_EE(addr_type_DRIVE, type_DRIVE);
Write_INT_EE(addr_deadBAND, deadBAND);
//Write_INT_EE(addr_R1_min, R1_min);
//Write_INT_EE(addr_R1_max, R1_max);
//Write_INT_EE(addr_R2_min, R2_min);
//Write_INT_EE(addr_R2_max, R2_max);
Write_INT_EE(addr_mem_LRC,12345);
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int Posit_avtomat(int FB, int set_posit, int DB){
int delta, way;

delta = abs(FB - set_posit);
if(set_posit > 0 && set_posit < 100){
           if(delta > DB){
           if(FB > set_posit)way = -1;
           if(FB < set_posit)way = 1;
           }else{way = 0;}
}
if(set_posit == 0){
if(FB > 0){way = -1;}else{way = 0;}
}
if(set_posit == 100){
if(FB < 100){way = 1;}else{way = 0;}
}
return way;
}//
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
void Discret_OUTPUT(void){

//******************************
if(DO_K1){k1_SetHigh();}else{k1_SetLow();}
if(DO_K2){k2_SetHigh();}else{k2_SetLow();}
if(DO_K3){k3_SetHigh();}else{k3_SetLow();}
if(DO_K4){k4_SetHigh();}else{k4_SetLow();}
if(DO_alarm){k5_SetHigh();}else{k5_SetLow();}
if(DO_alarm){HL_red_SetHigh();}else{HL_red_SetLow();}

status_DO = 0x00;
status_DO |= (DO_K1&1)<<0;
status_DO |= (DO_K2&1)<<1;
status_DO |= (DO_K3&1)<<2;
status_DO |= (DO_K4&1)<<3;
status_DO |= (DO_alarm&1)<<4;

if(((status_DO & 0x07)>0 && q_ZPGV==2) || ((status_DO & 0x03)>0 && q_ZPGV==1)){
    if(generator){HL_yel_SetHigh();}else{HL_yel_SetLow();}
}else{HL_yel_SetLow();}
}//
//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Binding(void){
static int tmp_Rmax, tmp_Rmin, tmp_Rmax2, tmp_Rmin2;
int band_D, band_D2;

EN++;
if(EN > 20){
//******************************************************************************
EN = 0;
DELAY_milliseconds(25);

if(q_ZPGV == 2){
//--
if(cnt_bind == 0 && TONS[15].wait_ON == 0){
TONS[15].PT = 200;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 1  && TONS[15].wait_ON == 0){
TONS[15].PT = Thoda * 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 2 && TONS[15].wait_ON == 0){
TONS[15].PT = 200;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 3 && TONS[15].wait_ON == 0){
TONS[15].PT = Thoda * 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 4 && TONS[15].wait_ON == 0){
TONS[15].PT = 200;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 5 && TONS[15].wait_ON == 0){
TONS[15].PT = Thoda * 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 6 && TONS[15].wait_ON == 0){
TONS[15].PT = 200;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 7 && TONS[15].wait_ON == 0){
TONS[15].PT = Thoda * 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 8 && TONS[15].wait_ON == 0){
TONS[15].PT = 200;
TONS[15].runs = 1;
}
//--
if(cnt_bind == 9 && TONS[15].wait_ON == 0){
TONS[15].PT = Thoda * 100;
TONS[15].runs = 1;
}
//--
if(TONS[15].Q > 0){TONS[15].runs = 0;cnt_bind++;}

//******************************************************************************
if(cnt_bind == 0)Direct_driver(0, 0);
if(cnt_bind == 1)Direct_driver(1, 1);
if(cnt_bind == 2){Direct_driver(0, 0);tmp_Rmax = dat_FB1;tmp_Rmax2 = dat_FB2;}
if(cnt_bind == 3)Direct_driver(-1, 1);
if(cnt_bind == 4){Direct_driver(0, 0);tmp_Rmin = dat_FB1;}
if(cnt_bind == 5)Direct_driver(1, 1);
if(cnt_bind == 6)Direct_driver(0, 0);
if(cnt_bind == 7)Direct_driver(1, -1);
if(cnt_bind == 8){Direct_driver(0, 0);tmp_Rmin2 = dat_FB2;}
if(cnt_bind == 9)Direct_driver(-1, 1);
//------------------------------
if(cnt_bind >= 10){
Direct_driver(0, 0);
hand_run_bind = 0;
cnt_bind = 0;
band_D  = abs(tmp_Rmin - tmp_Rmax);
band_D2 = abs(tmp_Rmin2 - tmp_Rmax2);
if(tmp_Rmin>tmp_Rmax || band_D<100 || tmp_Rmin2>tmp_Rmax2 || band_D2<100){
       err_minmax = 1;
       R1_min = 12;
       R1_max = 1001;
       R2_min = 12;
       R2_max = 1001;
       }else{
       R1_min = tmp_Rmin;
       R1_max = tmp_Rmax;
       R2_min = tmp_Rmin2;
       R2_max = tmp_Rmax2;
       Write_INT_EE(addr_R1_min, R1_min);
       Write_INT_EE(addr_R1_max, R1_max);
       Write_INT_EE(addr_R2_min, R2_min);
       Write_INT_EE(addr_R2_max, R2_max);
       }
//------------------------------
}
}//if(q_ZPGV == 2)

if(q_ZPGV == 1){
//--
if(cnt_bind==0 && TONS[15].wait_ON == 0){
TONS[15].PT = 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind==1 && TONS[15].wait_ON == 0){
TONS[15].PT = Thoda * 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind==2 && TONS[15].wait_ON == 0){
TONS[15].PT = 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind==3 && TONS[15].wait_ON == 0){
TONS[15].PT = Thoda * 100;
TONS[15].runs = 1;
}
//--
if(cnt_bind==4 && TONS[15].wait_ON == 0){
TONS[15].PT = 1000;
TONS[15].runs = 1;
}
//--
if(TONS[15].Q > 0){TONS[15].runs = 0;cnt_bind++;}
//------------------------------
if(cnt_bind == 0)Direct_driver(0, 0);
if(cnt_bind == 1)Direct_driver(1, 1);
if(cnt_bind == 2){Direct_driver(0, 0);tmp_Rmax = dat_FB1;}
if(cnt_bind == 3)Direct_driver(-1, 0);
if(cnt_bind == 4){Direct_driver(0, 0);tmp_Rmin = dat_FB1;}
//------------------------------
if(cnt_bind == 5){
hand_run_bind = 0;
cnt_bind = 0;
band_D = abs(tmp_Rmin-tmp_Rmax);
       if(tmp_Rmin > tmp_Rmax || band_D < 100){
       err_minmax = 1;
       R1_min = 12;
       R1_max = 1001;
       R1_min = 12;
       R1_max = 1001;
       }else{
       R1_min = tmp_Rmin;
       R1_max = tmp_Rmax;
       R2_min = 12;
       R2_max = 1001;
       Write_INT_EE(addr_R1_min, R1_min);
       Write_INT_EE(addr_R1_max, R1_max);
       Write_INT_EE(addr_R2_min, R2_min);
       Write_INT_EE(addr_R2_max, R2_max);
       }
//------------------------------
}
}//if(q_ZPGV == 1)

//******************************************************************************
}//EN
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Direct_driver(int way1, int way2){
if(way1 == 0) {DO_K1 = 0; DO_K2 = 0;}
if(way1 == 1) {DO_K1 = 1; DO_K2 = 0;}
if(way1 == -1){DO_K1 = 1; DO_K2 = 1;}

if(way2 == 0) {DO_K3 = 0; DO_K4 = 0;}
if(way2 == 1) {DO_K3 = 1; DO_K4 = 0;}
if(way2 == -1){DO_K3 = 1; DO_K4 = 1;}
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void AVTOMAT(void){
static int cnt_a;
int A_set;

A_set = Iset;

if(q_ZPGV == 2 && type_DRIVE == 1){ZPG1_set = A_set; ZPG2_set = 100 - A_set;}
//****************************************
if(q_ZPGV == 2 && type_DRIVE == 2){
              if(A_set <= 50){
              ZPG1_set = A_set * 2;
              ZPG2_set = 100;}
              if(A_set > 50){
              ZPG1_set = 100;
              ZPG2_set = (-2 * A_set) + 200;}
}

if(q_ZPGV == 1){
ZPG1_set = A_set;
ZPG2_set = 0;
}
//****************************************

if(!ALARM){
Driver_1(ZPG1_set);
if(q_ZPGV != 1)Driver_2(ZPG2_set);
}
if(ALARM){Direct_driver(-1, 1);}
//*******************************************
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Driver_1 (int set_A){
int posit;
static char flag_stop, flag_open, flag_close;

posit = Posit_avtomat(Angle1, set_A, deadBAND);
//-----------------------------------------------------------
if((posit == 0 && !flag_close && !flag_open) || flag_stop) {
DO_K1 = 0;
DO_K2 = 0;
flag_stop = 1;
}
//-----------------------------------------------------------
if(flag_stop){TONS[9].runs = 1;}
if(TONS[9].Q > 0){
TONS[9].runs = 0;
TONS[10].runs = 0;
TONS[11].runs = 0;
flag_stop = 0;
flag_open = 0;
flag_close = 0;
}
//-----------------------------------------------------------
if((posit == 1 && !flag_stop && !flag_close)||flag_open){
DO_K1 = 1;
DO_K2 = 0;
flag_open = 1;
}
if((posit == -1 && !flag_stop && !flag_open)||flag_close){
DO_K1 = 1;
DO_K2 = 1;
flag_close = 1;
}
//-----------------------------------------------------------
if(flag_open && posit!=1){TONS[10].runs = 1;}
if(TONS[10].Q > 0){TONS[10].runs = 0; flag_open = 0;flag_stop = 1;}

if(flag_close && posit!=-1){TONS[11].runs = 1;}
if(TONS[11].Q > 0){TONS[11].runs = 0; flag_close = 0;flag_stop = 1;}
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Driver_2 (int set_A){
int posit;
static char flag_stop, flag_open, flag_close;

posit = Posit_avtomat(Angle2, set_A, deadBAND);
//-----------------------------------------------------------
if((posit == 0 && !flag_close && !flag_open) || flag_stop) {
DO_K3 = 0;
DO_K4 = 0;
flag_stop = 1;
}
//-----------------------------------------------------------
if(flag_stop){TONS[12].runs = 1;}
if(TONS[12].Q > 0){
TONS[12].runs = 0;
TONS[13].runs = 0;
TONS[14].runs = 0;
flag_stop = 0;
flag_open = 0;
flag_close = 0;
}
//-----------------------------------------------------------
if((posit == 1 && !flag_stop && !flag_close)||flag_open){
DO_K3 = 1;
DO_K4 = 0;
flag_open = 1;
}
if((posit == -1 && !flag_stop && !flag_open)||flag_close){
DO_K3 = 1;
DO_K4 = 1;
flag_close = 1;
}
//-----------------------------------------------------------
if(flag_open && posit!=1){TONS[13].runs = 1;}
if(TONS[13].Q > 0){TONS[13].runs = 0; flag_open = 0;flag_stop = 1;}

if(flag_close && posit!=-1){TONS[14].runs = 1;}
if(TONS[14].Q > 0){TONS[14].runs = 0; flag_close = 0;flag_stop = 1;}
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
void Draw_data (void){
static char *syswrng1, *syswrng2, *syswrng3;

////////////////////////////////////////////////////////////////////////////////
if((Menu >= 20 && Menu <= 29) || Menu == 30){
if(upd_value == 1){CursorLCD(1,0);lcd_dat(220);} 
if(upd_value == 0){CursorLCD(1,0);lcd_dat(32);}  
}
////////////////////////////////////////////////////////////////////////////////
if (Menu == 1){
print_KIS_TE(Iset,kis_Iset,1,2);
//pprint4(dat_Iset, 1,2);
}//
if (Menu == 2){
pprint3(ZPG1_set,1,0);
print_A(Angle1, kis_FB1, 1, 5 );
}//
if (Menu == 3){
   if(q_ZPGV == 2){
   pprint3(ZPG2_set,1,0);
   print_A(Angle2, kis_FB2, 1, 5 );
}
   if(q_ZPGV == 1){
   pprint3(0,1,0);
   pprint3(0,1,5);
}
}//
if (Menu == 4){
pprint8bin(status_DO, 1,0);
}//
if (Menu == 6){

}//

if (Menu == 5){
syswrng3 = "---OK---";

if(hand_alarm){syswrng3 = "!АВАРИЯ!";}
if(err_sum){syswrng3 = "угол мал";}
if(err_R1){syswrng3 = "err R1->";}
if(err_R2){syswrng3 = "err R2->";}
if(err_minmax){syswrng3 = "min>=max";}

if(hand_run_bind){
syswrng3 = "а.прив";
pprint2(cnt_bind,1,6);
}
CursorLCD(1,0);
StringLCD(syswrng3);
}//
////////////////////////////////////////////////////////////////////////////////
if (Menu == 22){
pprint2(q_ZPGV,1,2);
}//
if (Menu == 21){
syswrng1 = "-------";

if(type_DRIVE == 1){syswrng1 = "пропор. ";}
if(type_DRIVE == 2){syswrng1 = "исключ. ";}

CursorLCD(1,1);
StringLCD(syswrng1);
}//
if (Menu == 20){
pprint2(Thoda,1,2);
}//
//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
if (Menu == 23){
pprint2(deadBAND,1,3);
}
//------------------------------------------------------------------------------
if (Menu == 30){
syswrng1 = "-------";

if(hand_run_bind == 0){syswrng1 = "-------";}
if(hand_run_bind == 1){syswrng1 = ">go run";}

CursorLCD(1,1);
StringLCD(syswrng1);
}
//------------------------------------------------------------------------------

if (Menu == 24){
pprint4(dat_FB1,1,1);
}
if (Menu == 27){
pprint4(dat_FB2,1,1);
}
if (Menu == 25){
pprint4(R1_min,1,1);
}
if (Menu == 26){
pprint4(R1_max,1,1);
}
if (Menu == 28){
pprint4(R2_min,1,1);
}
if (Menu == 29){
pprint4(R2_max,1,1);
}
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void screen1(void){
ClearLCD();
CursorLCD(0,0);
StringLCD("SET PSTN");
CursorLCD(1,7);
StringLCD("%");
}
void screen2(void){
ClearLCD();
CursorLCD(0,0);
StringLCD("S -Z1-FB");
CursorLCD(1,3);
StringLCD("<>");
}
void screen3(void){
ClearLCD();
CursorLCD(0,0);
StringLCD("S -Z2-FB");
CursorLCD(1,3);
StringLCD("<>");
}
void screen4(void){
ClearLCD();
CursorLCD(0,0);
StringLCD("-stat_DO");
}
void screen5(void){
ClearLCD();
CursorLCD(0,0);
StringLCD("-STATUS-");
}
void screen6(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(">SERVICE");
CursorLCD(1,0);
StringLCD("  MENU <");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void screen20(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":T hoda ");
CursorLCD(1,5);
StringLCD("sec");
}
void screen21(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":t.drive");
}
void screen22(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":q.ZPGV ");
}
void screen23(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":DB zone");
CursorLCD(1,6);
StringLCD("oe");
}
void screen24(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":R_FB1->");
CursorLCD(1,6);
StringLCD("mV");
}
void screen25(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":R1 min ");
CursorLCD(1,6);
StringLCD("mV");
}
void screen26(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":R1 max ");
CursorLCD(1,6);
StringLCD("mV");
}
void screen27(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":R_FB2->");
CursorLCD(1,6);
StringLCD("mV");
}
void screen28(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":R2 min ");
CursorLCD(1,6);
StringLCD("mV");
}
void screen29(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":R2 max ");
CursorLCD(1,6);
StringLCD("mV");
}
void screen30(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":А.прив.");
}
void screen31(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":EXIT  -");
CursorLCD(1,0);
StringLCD(":------>");
}
void screen32(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(":SAVE  &");
CursorLCD(1,0);
StringLCD(":EXIT-->");
}
void screen33(void){
ClearLCD();
CursorLCD(0,0);
StringLCD(" POSIT  ");
CursorLCD(1,0);
StringLCD("tar.1.01");
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void static_MENU(void){
    
static char cnt_R;
static char h_run_bind, iwashere;

//---------------------------------
//---------------------------------
if(!tune_oper){tune = 0;}else{tune = 2;}

if(!btn_L && old_btn_L && upd_value==0 && btn_C)Menu--;
if(!btn_R && old_btn_R && upd_value==0 && btn_C)Menu++;
//---------------------------------------------
if(tune == 0){
if(Menu > 6){Menu = 1;}
if(Menu < 1){Menu = 6;}
}
if(tune_oper){
if(Menu > 33){Menu = 20;}
if(Menu < 20){Menu = 33;}
}
//---------------------------------------------
if(Menu != 6){cnt_R = 0;}

if(Menu == 6 && !btn_C && !btn_R && old_btn_R){cnt_R++;}
if(Menu == 6 && cnt_R >= 7 && btn_C){
tune_oper = 1;
flag_BTN_change = 1;
cnt_R = 0;
Menu = 77;}
//************************
if(Menu == 32){
        if(!btn_C && old_btn_C){
        Menu = 1;
        tune_oper = 0;
        WriteAll_EE();
        }
}//
//************************
if(Menu == 31){
        if(!btn_C && old_btn_C){
        Menu = 1;
        tune_oper = 0;
        }
}//
//******************************************************************************
if(Menu == 5){
if(!btn_C){hand_alarm = 1;}else{hand_alarm = 0;}
}else{hand_alarm = 0;}
//------------------------------------------------------------------------------
if ((Menu >= 20 && Menu <= 29) || Menu == 30){
if(!btn_C && old_btn_C)upd_value++;
if(upd_value>1)upd_value=0;
}
if(olds_menu != Menu)upd_value = 0;
if(Menu <= 6)upd_value = 0;
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
if(Menu == 23 && upd_value==1){
if(!btn_L && old_btn_L)deadBAND--;
if(!btn_R && old_btn_R)deadBAND++;

if(deadBAND > 10){deadBAND = 10;}
if(deadBAND < 1){deadBAND = 1;}
}
///////////////////////////////////////////////
if(Menu == 30 && upd_value == 1){
if(!btn_L && old_btn_L){hand_run_bind = 0;}
if(!btn_R && old_btn_R){hand_run_bind = 1;}
h_run_bind = hand_run_bind;
cnt_bind = 0;
iwashere = 1;
}
if(Menu == 30 && h_run_bind && upd_value == 0 && iwashere){
tune_oper = 0;
flag_BTN_change = 1;
Menu = 5;
h_run_bind = 0;
}
///////////////////////////////////////////////
if(Menu == 22 && upd_value == 1){
if(!btn_L && old_btn_L)q_ZPGV = 1;
if(!btn_R && old_btn_R)q_ZPGV = 2;
}
///////////////////////////////////////////////
if(Menu == 21 && upd_value == 1){
if(!btn_L && old_btn_L)type_DRIVE = 1;
if(!btn_R && old_btn_R)type_DRIVE = 2;
}
///////////////////////////////////////////////
if(Menu == 20 && upd_value == 1){
if(!btn_L && old_btn_L)Thoda--;
if(!btn_R && old_btn_R)Thoda++;

if(Thoda > 95){Thoda = 95;}
if(Thoda < 20){Thoda = 20;}
}
///////////////////////////////////////////////
olds_menu = Menu;
old_btn_L = btn_L;
old_btn_C = btn_C;
old_btn_R = btn_R;
///////////////////////////////////////////////
if(!btn_L || !btn_C || !btn_R)cnt_menu = 0;
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void cikl_MENU(void){
if (old_menu != Menu){Menu_changE = 1;}else{Menu_changE = 0;}
old_menu = Menu;
if(Menu_changE){
if (Menu == 1){screen1();}
if (Menu == 2){screen2();}
if (Menu == 3){screen3();}
if (Menu == 4){screen4();}
if (Menu == 5){screen5();}
if (Menu == 6){screen6();}

if (Menu == 20){screen20();}
if (Menu == 21){screen21();}
if (Menu == 22){screen22();}
if (Menu == 23){screen23();}
if (Menu == 24){screen24();}
if (Menu == 25){screen25();}
if (Menu == 26){screen26();}
if (Menu == 27){screen27();}
if (Menu == 28){screen28();}
if (Menu == 29){screen29();}
if (Menu == 30){screen30();}
if (Menu == 31){screen31();}
if (Menu == 32){screen32();}
if (Menu == 33){screen33();}
}
}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void read_AI(void){
static char cnt_adc;
double tmp;
cnt_adc++;
//------------------------------------------------------------------------------
if(cnt_adc == 1){
kis_Iset = FIND_KIS(dat_Iset);
Iset = adc_to_int(dat_Iset, 100, kis_Iset);
}
//------------------------------------------------------------------------------
if(cnt_adc == 2){
kis_FB1 = 0;//FIND_KIS_FB(dat_FB1);
Angle1 = define_ANGLE(dat_FB1, kis_FB1, R1_max, R1_min);
//Angle1 = define_ANGLE(dat_FB1, kis_FB1, 2500, 120);
}
//------------------------------------------------------------------------------
if(cnt_adc == 3){
kis_FB2 = FIND_KIS_FB(dat_FB2);
Angle2 = define_ANGLE(dat_FB2, kis_FB2, R2_max, R2_min);
//Angle2 = define_ANGLE(dat_FB2, kis_FB2, 2500, 120);
}
//------------------------------------------------------------------------------
if(cnt_adc >= 4){
    tmp = (double)dat_Upower * 0.163 + 6.601;
    Upower = (int)tmp;
cnt_adc = 0;
}
//------------------------------------------------------------------------------

}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void ISR_ADC (void){
    switch( scanCounter )
    {
        case 0:
            ain0Buff[sampleCounter] = ADC1BUF0;
            break;

        case 1:
            ain1Buff[sampleCounter] = ADC1BUF0;
            break;

        case 2:
            ain9Buff[sampleCounter] = ADC1BUF0;
            break;

        case 3:
            ain10Buff[sampleCounter] = ADC1BUF0;
            break;

        default:
            break;
    }

    scanCounter++;
    if( scanCounter == NUM_CHS2SCAN )
    {
        scanCounter = 0;
        sampleCounter++;
    }

    if( sampleCounter == SAMP_BUFF_SIZE )
    {
        sampleCounter = 0;
    }

ADCValue = 0;	
    for (count = 0; count < 16; count++){ADCValue += ain0Buff[count];} 
	ADCValue = ADCValue >> 4;   
    dat_Upower = ADCValue;
    ADCValue = 0;	
    for (count = 0; count < 16; count++){ADCValue += ain1Buff[count];} 
	ADCValue = ADCValue >> 4;   
    dat_Iset = ADCValue;
    ADCValue = 0;	
    for (count = 0; count < 16; count++){ADCValue += ain9Buff[count];} 
	ADCValue = ADCValue >> 4;   
    dat_FB2 = ADCValue;
    ADCValue = 0;	
    for (count = 0; count < 16; count++){ADCValue += ain10Buff[count];} 
	ADCValue = ADCValue >> 4;   
    dat_FB1 = ADCValue;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//******************************************************************************
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void find_ERR(void){
static char werr;

werr = 0x00;
if((q_ZPGV == 2) && ((Angle1 + Angle2) < 30))err_sum = 1;
if(kis_FB1 > 0){err_R1 = 1;}else{err_R1 = 0;}
if((q_ZPGV == 2) && (kis_FB2 > 0)){err_R2 = 1;}else{err_R2 = 0;}

werr = 0x00;
werr |= (err_sum&1)<<0;
werr |= (err_minmax&1)<<1;
werr |= (err_R1&1)<<2;
werr |= (err_R2&1)<<3;
//werr = 0x00;

if(werr == 0){ALARM = 0;TONS[8].runs = 0;}
if(werr > 0){TONS[8].runs = 1;}
if(TONS[8].Q > 0){ALARM = 1;}

if(ALARM || hand_alarm){DO_alarm = 1;}else{DO_alarm = 0;}

}//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
                         Main application
 */
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
int main(void){
    // initialize the device
    SYSTEM_Initialize();
    CN_SetInterruptHandler(ISR_CN);
    TMR5_SetInterruptHandler(ISR_T5);
    ADC1_SetInterruptHandler(ISR_ADC);  
    //if(Menu != 1)__asm__ volatile ("reset");
//******************************************************************************
Menu = 1;
old_menu = 0;
_lcd_init();                       
tune_oper = 0;
//--------------------
ReadALL_EE();
//--------------------
TONS[6].PT = 1000;     //10 sec wait for errors parameters
TONS[7].PT = 10;       //period for generator
TONS[8].PT = 500;      //wait for ALARM
TONS[9].PT = 300;      //wait for reverse
TONS[10].PT = 20;      //min time open
TONS[11].PT = 20;      //min time close
TONS[12].PT = 300;     //wait for reverse
TONS[13].PT = 20;      //min time open
TONS[14].PT = 20;      //min time close

for(ib = 0;ib <= 16;ib++){
TONS[ib].runs = 0;
TONS[ib].Q = 0;
}
//////////////////////////////
flag_DI_change = 0;
hand_run_bind = 0;
cnt_bind = 0;
ALARM = 0;

EN = 100;
err_R1 = 0;
err_R2 = 0;
err_sum = 0;
err_minmax = 0;
//////////////////////////////
DO_K1 = 0;
DO_K2 = 0;
DO_K3 = 0;
DO_K4 = 0;
DO_alarm = 0;
Direct_driver(-1, 1);
Discret_OUTPUT();
////////////////////////////////////////////////////////////////////////////////
DELAY_milliseconds(200);
WATCHDOG_TimerSoftwareEnable();
//////////////////////////////
//******************************************************************************
while (1){
cikl_MENU();
read_AI();
//******************************************************************************
if(!hand_run_bind){AVTOMAT();cnt_bind = 0; EN = 100; TONS[15].runs = 0;}
if(hand_run_bind){Binding();}
Discret_OUTPUT();
find_ERR();
//******************************************************************************
//******************************************************************************
if(Timer_screen){
Draw_data();
Timer_screen = 0;
//*******************
PWM1((int8_t)Angle1);
PWM2((int8_t)Angle2);
//*******************
if(TONS[7].wait_ON == 0)TONS[7].runs = 1;
if(TONS[7].Q > 0){TONS[7].runs = 0;generator = ~generator;}
}//
//******************************************************************************
WATCHDOG_TimerClear();
//------------------------------------------------------------------------------
if(flag_BTN_change){
flag_BTN_change = 0;
static_MENU();
}//
//------------------------------------------------------------------------------
    }//while
return 1; 
}//main
/**
 End of File
*/

