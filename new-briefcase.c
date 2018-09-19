/* 
 * File:   varun_4_1.c
 * Author: VARUN SAHNI
 *
 * Created on changed 19 september, 2018, 8:40 PM
 * this is final proper working in briefcase
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <pic16f1526.h>
// 'C' source line config statements

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection (HS Oscillator, High-speed crystal/resonator connected between OSC1 and OSC2 pins)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (VCAP pin function disabled)
#pragma config STVREN = OFF     // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will not cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
// Since we have used 16 MHz crystal
#define _XTAL_FREQ 16000000  

// Pin MACROS
////****CONTROLLED BY TOUCHMATK*********//
#define OUTPUT_RELAY1 RB1
#define OUTPUT_RELAY2 RC1
#define OUTPUT_RELAY3 RA0
#define OUTPUT_RELAY4 RF1
//****CONTROLLED BY MANUAL WITCHES************//
#define OUTPUT_RELAY5 RA3
#define OUTPUT_RELAY6 RA1
#define OUTPUT_RELAY7 RA2
//#define OUTPUT_RELAY8 RB3
///**********OUTPUT DIMMER***************///
#define OUTPUT_DIMMER RE1

#define OUTPUT_RELAY_DIR_1 TRISBbits.TRISB1
#define OUTPUT_RELAY_DIR_2 TRISCbits.TRISC1
#define OUTPUT_RELAY_DIR_3 TRISAbits.TRISA0
#define OUTPUT_RELAY_DIR_4 TRISFbits.TRISF1
#define OUTPUT_RELAY_DIR_5 TRISAbits.TRISA3        
#define OUTPUT_RELAY_DIR_6 TRISAbits.TRISA1 
#define OUTPUT_RELAY_DIR_7 TRISAbits.TRISA2
//#define OUTPUT_RELAY_DIR_8 TRISBbits.TRISB3
#define OUTPUT_DIMMER_DIR  TRISEbits.TRISE1

#define SW1 RF2
#define SW2 RF3
#define SW3 RF4
#define SW4 RF5
#define SW5 RF6   
#define SW6 RD7
#define SW7 RD6
#define SW8 RD5


#define INPUT_SWITCH_DIR_1 TRISFbits.TRISF2
#define INPUT_SWITCH_DIR_2 TRISFbits.TRISF3
#define INPUT_SWITCH_DIR_3 TRISFbits.TRISF4
#define INPUT_SWITCH_DIR_4 TRISFbits.TRISF5
#define INPUT_SWITCH_DIR_5 TRISFbits.TRISF6
#define INPUT_SWITCH_DIR_6 TRISDbits.TRISD7
#define INPUT_SWITCH_DIR_7 TRISDbits.TRISD6
#define INPUT_SWITCH_DIR_8 TRISDbits.TRISD5

unsigned char st[50] = "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT";

/*
 * Extra Periferals Direction and PORT
 */
//#define ZCD_CCP9_DIR TRISEbits.TRISE3
#define ZCD_CCP1_DIR TRISCbits.TRISC2
// USART Directions
#define USART_1_TRANSMIT_OUTPUT_DIR TRISCbits.TRISC6
#define USART_1_RECIEVE_INPUT_DIR TRISCbits.TRISC7

#define USART_2_TRANSMIT_OUTPUT_DIR TRISGbits.TRISG1
#define USART_2_RECIEVE_INPUT_DIR TRISGbits.TRISG2

#define RECIEVED_DATA_LENGTH (16*2)
#define TOTAL_NUMBER_OF_SWITCH (8*2)


#define TOUCHPANEL_DATA_LENGTH (8*2)
#define TRUE 1
#define FALSE 0

#define CHAR_TRUE '1'
#define CHAR_FALSE '0'

// Conditional compilation



#define SWITCH_7_RELAY
//#define SWITCH_8_RELAY


// ALL error Definitions
/* 
 * #define WRONG_DATA_RECIEVED_ERROR_CODE ERRX
 * #define RECIVING_OVERRUN_ERROR EROV
 * #define RECEIVING_DATA_LOST_IN_MAIN ERLS
 */
/* DATA USED IN MANUAL  STARTS HERE*/
unsigned int M1;unsigned int M2;unsigned int M3;unsigned int M4;unsigned int M5;unsigned int M6;unsigned int M7;unsigned int M8;


#define ON 1
#define OFF 0
#define CHAR_OFF '0'
#define CHAR_ON '1'
        
/* DATA USED IN MANUAL END HERE*/




unsigned char ErrorNames[5]="####";

int mainReceivedDataPosition=0, mainDataReceived=FALSE;
unsigned char mainReceivedDataBuffer[RECIEVED_DATA_LENGTH]="#"; 
unsigned char tempReceivedDataBuffer[RECIEVED_DATA_LENGTH-8]="#";
unsigned char parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="0000000000000000";
unsigned char copy_parentalLockBuffer[TOTAL_NUMBER_OF_SWITCH]="0000000000000000";
unsigned char currentStateBuffer[(TOTAL_NUMBER_OF_SWITCH*4)+2]="#";


int checkFlag=0;
unsigned int preventFlag1_Panel=0;
unsigned int preventFlag2_Panel=0;
unsigned int preventFlag3_Panel=0;
unsigned int preventFlag4_Panel=0;
int touchpanelReceivedataPosition = 0; 
int touchPanelDataReceived = FALSE;
unsigned char touchpanleReceivedDatabuffer[TOUCHPANEL_DATA_LENGTH]="#";
unsigned char tempReceiveTouchpanelDataBuffer[TOUCHPANEL_DATA_LENGTH-8]="#";


#define TouchMatikBoardAddress 'c'



int start_PWM_Generation_in_ISR_FLAG=FALSE;
char levelofDimmer_MSB='0',levelofDimmer_LSB='0';

void errorsISR(char* errNum);
void errorsMain(char* errNum);
void sendAcknowledgment(char* currentStateBuffer);
void sendFeedback_TO_Gateway(char sw_status, char Switch_Num);
void sendFeedback_TO_Touch(char Switch_Num_1s, char sw_status);

void clearAllPorts();
void pinINIT_extra();
void GPIO_pin_Initialize();

void AllInterruptEnable();
void EUSART_Initialize();
void EUSART2_Initialize();

void TMR3_Initialize();
void TMR1_Initialize();
//void CCP9_Initialize();
void CCP1_Initialize();
void allPeripheralInit();

void copyReceivedDataBuffer();
void copyTouchpanelReceiveDataBuffer();
void applianceControl(char switchMSB, char switchLSB, char switchSTATE, char dimmerSpeedMSB, char dimmerSpeedLSB, char parentalControl, char finalFrameState);

void actiontouchPanel(char Switch_Num, char sw_status );//, char speeds
void writeUART(char *str2Write);


interrupt void isr(){
    //*******************TIMER3 INTERRUPT**************************//
     if(PIE3bits.TMR3IE==1 && PIR3bits.TMR3IF==1)
    {           
        PIR3bits.TMR3IF=0;
        OUTPUT_DIMMER = TRUE;
        T3CONbits.TMR3ON=0;
       // TX1REG='Q';
    }    
   
     
    //*********************TIMER1 INTERRUPT**************************//
     if(PIE1bits.TMR1IE == 1 && PIR1bits.TMR1IF==1)
    {
        PIR1bits.TMR1IF=0;
        //TX1REG='T';        
        OUTPUT_DIMMER = FALSE;            
        TMR3H=0xFF;
        TMR3L=0xD8;
        T3CONbits.TMR3ON = 1;
        T1CONbits.TMR1ON = 0;        
    }
    //*************************ZCD INTERRRUPT****************************//
    
//    if(CCP9IF)
//    {
//        TX1REG = 'C';
//        if(CCP9IF == 1)
//        {
//            start_PWM_Generation_in_ISR_FLAG = TRUE;
//            PWM_100Stages();
//        }
//        CCP9IF=0;
//    }
      if(CCP1IF)
    {
      //  TX1REG = 'C';
        if(CCP1IF == 1)
        {
            CCP1IF=0;
            
            if(start_PWM_Generation_in_ISR_FLAG == 1)
            {
                switch(levelofDimmer_MSB)
                {
//            TX1REG = 'M';
                case '0':           // 8.5
                    /**/
                        switch(levelofDimmer_LSB)
                             {
                  //          TX1REG = 'S';
                             case '0':           // 8.5
//                                     TMR1H=0x7B;
//                                     TMR1L=0x30;
//                                     T1CONbits.TMR1ON = 1;
                                     OUTPUT_DIMMER=1;
                                     break;
                             case '1':           // 8.4
//                                     TMR1H=0x7C;
//                                     TMR1L=0xC0;
//                                     T1CONbits.TMR1ON = 1;
                                     OUTPUT_DIMMER=1;
                                     break;
                             case '2':           // 8.35
                                     TMR1H=0x7D;
                                     TMR1L=0x88;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 8.25
                                     TMR1H=0x7F;
                                     TMR1L=0x18;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4':          // 8.15
                                     TMR1H=0x80;
                                     TMR1L=0xA8;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 8.1
                                     TMR1H=0x81;
                                     TMR1L=0x70;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 8.0    
                                     TMR1H=0x83;
                                     TMR1L=0x00;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            //7.95
                                     TMR1H=0x83;
                                     TMR1L=0xC8;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           //7.9
                                     TMR1H=0x87;
                                     TMR1L=0xB0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':           // 7.85
                                     TMR1H=0x85;
                                     TMR1L=0x58;
                                     T1CONbits.TMR1ON = 1;
                                     break;

                             default:
                                 break;
                         }                    
                        break;
                case '1':           // 7.8-7.3

                            switch(levelofDimmer_LSB)
                                 {
                                 case '0':           // 7.8
                                         TMR1H=0x86;
                                         TMR1L=0x20;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '1':           // 7.75
                                         TMR1H=0x86;
                                         TMR1L=0xE8;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '2':           // 7.7
                                         TMR1H=0x87;
                                         TMR1L=0xB0;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '3':           // 7.65
                                         TMR1H=0x88;
                                         TMR1L=0x78;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '4':            // 7.6
                                         TMR1H=0x89;
                                         TMR1L=0x40;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '5':               // 7.55
                                         TMR1H=0x8A;
                                         TMR1L=0x08;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '6':               // 7.5    
                                         TMR1H=0x8A;
                                         TMR1L=0xD0;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '7':            //7.45
                                         TMR1H=0x8B;
                                         TMR1L=0x98;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '8':           //7.4
                                         TMR1H=0x8C;
                                         TMR1L=0x60;
                                         T1CONbits.TMR1ON = 1;
                                         break;
                                 case '9':           // 7.35
                                         TMR1H=0x8D;
                                         TMR1L=0x28;
                                         T1CONbits.TMR1ON = 1;
                                         break;

                                 default:
                                     break;
                                }
                        break;
                case '2':           // 7.3-
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 7.3-6.85
                                     TMR1H=0x8D;
                                     TMR1L=0xF0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 7.25
                                     TMR1H=0x8E;
                                     TMR1L=0xB8;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 7.20
                                     TMR1H=0x8F;
                                     TMR1L=0x80;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 7.15
                                     TMR1H=0x90;
                                     TMR1L=0x48;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4'://TX1REG='n';      // 7.1
                                     TMR1H=0x91;
                                     TMR1L=0x10;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 7.05
                                     TMR1H=0x91;
                                     TMR1L=0xD8;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 7.0    
                                     TMR1H=0x92;
                                     TMR1L=0xA0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            //6.95
                                     TMR1H=0x93;
                                     TMR1L=0x68;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           //6.9
                                     TMR1H=0x94;
                                     TMR1L=0x30;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':           // 6.85
                                     TMR1H=0x94;
                                     TMR1L=0xF8;
                                     T1CONbits.TMR1ON = 1;
                                     break;

                             default:
                                 break;
                         }                    
                        break;
                case '3':           // 6.8-5.9                
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 6.8
                                     TMR1H=0x95;
                                     TMR1L=0xC0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 6.7
                                     TMR1H=0x97;
                                     TMR1L=0x50;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 6.6
                                     TMR1H=0x98;
                                     TMR1L=0xE0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 6.5
                                     TMR1H=0x9A;
                                     TMR1L=0x70;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4'://TX1REG='n';      // 6.4
                                     TMR1H=0x9C;
                                     TMR1L=0x00;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 6.3
                                     TMR1H=0x9D;
                                     TMR1L=0x90;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 6.2   
                                     TMR1H=0x9F;
                                     TMR1L=0x20;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            //6.1
                                     TMR1H=0xA0;
                                     TMR1L=0xB0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           //6.0
                                     TMR1H=0xA2;
                                     TMR1L=0x40;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':           // 5.9
                                     TMR1H=0xA3;
                                     TMR1L=0xD0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             default:
                                     break;
                            }
                        break;
                case '4'://TX1REG='n';      // 5.8-4.9                    
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 5.8
                                     TMR1H=0xA5;
                                     TMR1L=0x60;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 5.7
                                     TMR1H=0xA6;
                                     TMR1L=0xF0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 5.6
                                     TMR1H=0xA8;
                                     TMR1L=0x80;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 5.5
                                     TMR1H=0xAA;
                                     TMR1L=0x10;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4':           // 5.4
                                     TMR1H=0xAB;
                                     TMR1L=0xA0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 5.3
                                     TMR1H=0xAD;
                                     TMR1L=0x30;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 5.2    
                                     TMR1H=0xAE;
                                     TMR1L=0xC0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':              // 5.1
                                     TMR1H=0xB0;
                                     TMR1L=0x50;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':              // 5.0
                                     TMR1H=0xB1;
                                     TMR1L=0xE0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':              // 4.9
                                     TMR1H=0xB3;
                                     TMR1L=0x70;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             default:
                                     break;
                         }
                        break;
                case '5':               // 4.8-3.9
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 4.8
                                     TMR1H=0xB5;
                                     TMR1L=0x00;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 4.7
                                     TMR1H=0xB6;
                                     TMR1L=0x90;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 4.6
                                     TMR1H=0xB8;
                                     TMR1L=0x20;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 4.5
                                     TMR1H=0xB9;
                                     TMR1L=0xB0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4'://TX1REG='n';      // 4.4
                                     TMR1H=0xBB;
                                     TMR1L=0x40;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 4.3
                                     TMR1H=0xBC;
                                     TMR1L=0xD0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 4.2   
                                     TMR1H=0xBE;
                                     TMR1L=0x60;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            //4.1
                                     TMR1H=0xBF;
                                     TMR1L=0xF0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           //4.0
                                     TMR1H=0xC1;
                                     TMR1L=0x80;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':           // 3.9
                                     TMR1H=0xC3;
                                     TMR1L=0x10;
                                     T1CONbits.TMR1ON = 1;
                                     break;

                             default:
                                 break;
                            }                    
                        break;
                case '6':               // 3.8-2.9 
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 3.8
                                     TMR1H=0xC4;
                                     TMR1L=0xA0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 3.7
                                     TMR1H=0xC6;
                                     TMR1L=0x30;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 3.6
                                     TMR1H=0xC7;
                                     TMR1L=0xC0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 3.5
                                     TMR1H=0xC9;
                                     TMR1L=0x50;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4'://TX1REG='n';      // 3.4
                                     TMR1H=0xCA;
                                     TMR1L=0xE0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 3.3
                                     TMR1H=0xCC;
                                     TMR1L=0x70;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 3.2   
                                     TMR1H=0xCE;
                                     TMR1L=0x00;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            //3.1
                                     TMR1H=0xCF;
                                     TMR1L=0x90;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           // 3.0
                                     TMR1H=0xD1;
                                     TMR1L=0x20;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':           // 2.9
                                     TMR1H=0xD2;
                                     TMR1L=0xB0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             default:
                                     break;
                            }                    
                        break;
                case '7':            //2.8-1.9
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 2.8
                                     TMR1H=0xD4;
                                     TMR1L=0x40;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 2.7
                                     TMR1H=0xD5;
                                     TMR1L=0xD0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 2.6
                                     TMR1H=0xD7;
                                     TMR1L=0x60;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 2.5
                                     TMR1H=0xD8;
                                     TMR1L=0xF0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4'://TX1REG='n';      // 2.4
                                     TMR1H=0xDA;
                                     TMR1L=0x80;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 2.3
                                     TMR1H=0xDC;
                                     TMR1L=0x10;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 2.2  
                                     TMR1H=0xDD;
                                     TMR1L=0xA0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            // 2.1
                                     TMR1H=0xDF;
                                     TMR1L=0x30;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           // 2.0
                                     TMR1H=0xE0;
                                     TMR1L=0xC0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':           // 1.9
                                     TMR1H=0xE2;
                                     TMR1L=0x50;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             default:
                                     break;
                            }
                        break;
                case '8':           //1.8-1.2
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 1.8
                                     TMR1H=0xE3;
                                     TMR1L=0xE0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 1.75
                                     TMR1H=0xE4;
                                     TMR1L=0xA8;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 1.7
                                     TMR1H=0xE5;
                                     TMR1L=0x70;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 1.65
                                     TMR1H=0xE6;
                                     TMR1L=0x38;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4'://TX1REG='n';      // 1.6
                                     TMR1H=0xE7;
                                     TMR1L=0x00;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 1.5
                                     TMR1H=0xE8;
                                     TMR1L=0x90;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 1.4   
                                     TMR1H=0xEA;
                                     TMR1L=0x20;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            //1.3
                                     TMR1H=0xEB;
                                     TMR1L=0xB0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           //1.25
                                     TMR1H=0xEC;
                                     TMR1L=0x78;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '9':           // 1.2
                                     TMR1H=0xED;
                                     TMR1L=0x40;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             default:
                                     break;
                            }
                        break;
                case '9':           // 1.1-0.2
/**/
                        switch(levelofDimmer_LSB)
                             {
                             case '0':           // 1.1
                                     TMR1H=0xEE;
                                     TMR1L=0xD0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '1':           // 1.0
                                     TMR1H=0xF0;
                                     TMR1L=0x60;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '2':           // 0.9
                                     TMR1H=0xF1;
                                     TMR1L=0xF0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '3':           // 0.8
                                     TMR1H=0xF3;
                                     TMR1L=0x80;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '4'://TX1REG='n';      // 0.7
                                     TMR1H=0xF5;
                                     TMR1L=0x10;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '5':               // 0.6
                                     TMR1H=0xF6;
                                     TMR1L=0xA0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '6':               // 0.5    
                                     TMR1H=0xF8;
                                     TMR1L=0x30;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '7':            //0.4
                                     TMR1H=0xF9;
                                     TMR1L=0xC0;
                                     T1CONbits.TMR1ON = 1;
                                     break;
                             case '8':           //0.3
//                                     TMR1H=0xFB;
//                                     TMR1L=0x50;
//                                     T1CONbits.TMR1ON = 1;
                                       OUTPUT_DIMMER=0;
                                     break;
                             case '9':           // 0.2
//                                     TMR1H=0xFC;
//                                     TMR1L=0xE0;
//                                     T1CONbits.TMR1ON = 1;
                                       OUTPUT_DIMMER=0;
                                     break;
                             default:
                                     break;
                            }
                        break;
                default:
                        break;
            } 
            }
        }
        
    }
    
    
    
    // ************************************* XbEE UART INTERRUPT *********************************************** //
    if(RC1IF){        
        if(RC1STAbits.OERR){    // If over run error, then reset the receiver
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        } 
        mainReceivedDataBuffer[mainReceivedDataPosition]=RC1REG;
        #ifdef DEBUG
        TX1REG=mainReceivedDataBuffer[mainReceivedDataPosition];
        #endif
        if(mainReceivedDataBuffer[0]=='%'){
            mainReceivedDataPosition++;
            if(mainReceivedDataPosition>15){
                mainDataReceived=TRUE;
                mainReceivedDataPosition=0;                
                RC1IF=0;                
            }
        }
        else{
            RC1STAbits.CREN = 0; // countinuous Recieve Disable
            RC1STAbits.CREN = 1; // countinuous Recieve Enable
            mainReceivedDataPosition=0; // Reinitiate buffer counter
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='R';      ErrorNames[3]='X';
            errorsISR(ErrorNames);            
        }
    }// End of RC1IF
     
     /**************************************TOUCH_PANEL INTERRUPT*******************************************/
    if(RC2IF){        
        if(RC2STAbits.OERR){    // If over run error, then reset the receiver
            RC2STAbits.CREN = 0; // countinuous Recieve Disable
            RC2STAbits.CREN = 1; // countinuous Recieve Enable
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='O';      ErrorNames[3]='V';
            errorsISR(ErrorNames); 
        }   
        
        touchpanleReceivedDatabuffer[touchpanelReceivedataPosition] = RC2REG;
        if(touchpanleReceivedDatabuffer[0] == '(')
        {
            touchpanelReceivedataPosition++;
            if(touchpanelReceivedataPosition > 7)
            {
                touchPanelDataReceived = TRUE;
            
                touchpanelReceivedataPosition=0;
                 RC2IF = 0;
            }
        }
        else{
            RC2STAbits.CREN = 0; // countinuous Recieve Disable
            RC2STAbits.CREN = 1; // countinuous Recieve Enable
            touchpanelReceivedataPosition=0; // Reinitiate buffer counter
            
            ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='R';      ErrorNames[3]='T';
            errorsISR(ErrorNames);            
        }
    }//End of RC2IF
}




/*
 * Alfaone Main code starts here
 * 
 */
int main() {

    OUTPUT_RELAY1=OFF;
    OUTPUT_RELAY2=OFF;
    OUTPUT_RELAY3=OFF;
    OUTPUT_RELAY4=OFF;
    OUTPUT_RELAY5=OFF;
    OUTPUT_RELAY6=OFF;
    OUTPUT_RELAY7=OFF;
    OUTPUT_DIMMER=ON;
    M1=ON;    M2=ON;    M3=ON;    M4=ON;    M5=ON;   
    M6=ON;    M7=ON;    M8 =ON;    
    GPIO_pin_Initialize();
    allPeripheralInit();
   // AllInterruptEnable();

    while(1){
         ///STARTING OF MOBILE APP DATA RECEIVE
        int man=1;
        if(mainDataReceived==TRUE){
            mainDataReceived=FALSE;
            checkFlag=1;
            int start_flag = 0;
            int end_flag = 0;
            if(mainReceivedDataBuffer[0]=='%' && mainReceivedDataBuffer[1]=='%' && mainReceivedDataBuffer[14]=='@' && mainReceivedDataBuffer[15]=='@'){
                if(mainReceivedDataBuffer[0] == '%' && mainReceivedDataBuffer[1]=='%' && start_flag == 0)
                {
                    end_flag = 1;
                }
                if(mainReceivedDataBuffer[14]=='@' && mainReceivedDataBuffer[15]=='@' && end_flag ==1)
                {
                    copyReceivedDataBuffer();
                                 start_flag = 0;
                                   end_flag = 0;
                }
                
                
                
                
                
                applianceControl(tempReceivedDataBuffer[0],
                        tempReceivedDataBuffer[1],
                        tempReceivedDataBuffer[2],
                        tempReceivedDataBuffer[3],
                        tempReceivedDataBuffer[4],
                        tempReceivedDataBuffer[5],
                        tempReceivedDataBuffer[6]);
                                
            }   // End of all buffer data check
            else{
                ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='L';      ErrorNames[3]='S';
                errorsMain(ErrorNames);
                RC1STAbits.SPEN=0;  // Serial port disabled 
                RC1STAbits.CREN = 0; // countinuous Recieve Disable                
                for(int dataBufferCounter = 0; dataBufferCounter< 15; dataBufferCounter++)
                {
                    mainReceivedDataBuffer[dataBufferCounter] = '#'; // clean received data buffer
                }
                RC1STAbits.CREN = 1; // countinuous Recieve Enable
                RC1STAbits.SPEN=1;  // Serial port enabled (configures RXx/DTx and TXx/CKx pins as serial port pins)
            }
        } // End of mainDataReceived condition
        
        ///STARTING OF TOUCHPANEL DATA RECEIVE
        if(touchPanelDataReceived == TRUE)
        {
          //  TX1REG = 'R';
            touchPanelDataReceived = FALSE;
            int start_flag = 0;
            int end_flag = 0;
            if(touchpanleReceivedDatabuffer[0] == '(' && touchpanleReceivedDatabuffer[7] == ')')
            {
                
                if(touchpanleReceivedDatabuffer[0] == '('  && start_flag == 0)
                {
                    end_flag =1;

                }
                if(touchpanleReceivedDatabuffer[7] == ')' && end_flag ==1)
                {
                copyTouchpanelReceiveDataBuffer();
                if(tempReceiveTouchpanelDataBuffer[0] != '@'){
                   actiontouchPanel(tempReceiveTouchpanelDataBuffer[0],tempReceiveTouchpanelDataBuffer[1]); //,tempReceiveTouchpanelDataBuffer[2]
                    start_flag = 0;
                    end_flag = 0;
                }
                                
                }
               
            }
                else
                {
                ErrorNames[0]='E';      ErrorNames[1]='R';      ErrorNames[2]='L';      ErrorNames[3]='S';
                errorsMain(ErrorNames);
                RC2STAbits.SPEN = 0;  // Serial port disabled  
                RC2STAbits.CREN = 0; // countinuous Recieve Disable                
                for(int dataBufferCounter = 0; dataBufferCounter< 8; dataBufferCounter++)
                {
                    touchpanleReceivedDatabuffer[dataBufferCounter] = '#'; // clean received data buffer
                }
                RC2STAbits.CREN = 1; // countinuous Recieve Enable
                RC2STAbits.SPEN=1;  // Serial port enabled (configures RXx/DTx and TXx/CKx pins as serial port pins)
            }
            
        }
       
	   //manual response only from 5 switch to 8 switch
		if(copy_parentalLockBuffer[5]==CHAR_OFF && SW5==ON && M5==ON)
		{    
				if(man==1)
				{
				__delay_ms(5);
				TX1REG='R';__delay_ms(1);
				TX1REG='1';__delay_ms(1);
				TX1REG='0';__delay_ms(1);
				TX1REG='5';__delay_ms(1);
				OUTPUT_RELAY5=ON;
				}
				M5=0;                
				man=0;
		}     
       
        	 if(copy_parentalLockBuffer[5]==CHAR_OFF && SW5==OFF && M5==OFF)
			{    
				if(man==1)
				{
				__delay_ms(5);
				TX1REG='R';__delay_ms(1);
				TX1REG='0';__delay_ms(1);
				TX1REG='0';__delay_ms(1);
				TX1REG='5';__delay_ms(1);
				OUTPUT_RELAY5=OFF;                 
				}
				M5=1;               
				man=0;
			}
	   
			if(copy_parentalLockBuffer[6]==CHAR_OFF && SW6==ON && M6==ON)
			{    
				if(man==1)
				{
				__delay_ms(5);
				TX1REG='R';__delay_ms(1);
				TX1REG='1';__delay_ms(1);
				TX1REG='0';__delay_ms(1);
				TX1REG='6';__delay_ms(1);
				OUTPUT_RELAY6=ON;
				}
				M6=0;                
				man=0;
			}     
       
     		   if(copy_parentalLockBuffer[6]==CHAR_OFF && SW6==OFF && M6==OFF)
			{    
				if(man==1)
				{
				__delay_ms(5);
				TX1REG='R';__delay_ms(1);
				TX1REG='0';__delay_ms(1);
				TX1REG='0';__delay_ms(1);
				TX1REG='6';__delay_ms(1);
				OUTPUT_RELAY6=OFF;                 
				}
				M6=1;               
				man=0;
			}
    
		 if(copy_parentalLockBuffer[1]==CHAR_OFF && SW1==ON && M1==ON)
			{    
				if(man==1)
				{
				__delay_ms(5);
				TX1REG='R';__delay_ms(1);
				TX1REG='1';__delay_ms(1);
				TX1REG='0';__delay_ms(1);
				TX1REG='7';__delay_ms(1);
				OUTPUT_RELAY7=ON;
				}
				M1=0;                
				man=0;
			}     
       
		if(copy_parentalLockBuffer[1]==CHAR_OFF && SW1==OFF && M1==OFF)
				{  
				    if(man==1)
					{
					__delay_ms(5);
					TX1REG='R';__delay_ms(1);
					TX1REG='0';__delay_ms(1);
					TX1REG='0';__delay_ms(1);
					TX1REG='7';__delay_ms(1);
					OUTPUT_RELAY7=OFF;                 
					} 
					M1=1;               
					man=0;
				}
		    if(copy_parentalLockBuffer[2]==CHAR_OFF && SW2==OFF && M2==OFF)
				{  
				    if(man==1)
					{
					__delay_ms(5);
					TX1REG='R';__delay_ms(1);
					TX1REG='0';__delay_ms(1);
					TX1REG='0';__delay_ms(1);
					TX1REG='8';__delay_ms(1);
					 start_PWM_Generation_in_ISR_FLAG = 0;                 
					 OUTPUT_DIMMER = ON;
					}
					M2=1;               
					man=0;
				}
		 if(copy_parentalLockBuffer[2]==CHAR_OFF && SW2==ON && M2==ON)
				{    
					if(man==1)
					{
					__delay_ms(5);
					TX1REG='R';__delay_ms(1);
					TX1REG='1';__delay_ms(1);
					TX1REG='0';__delay_ms(1);
					TX1REG='8';__delay_ms(1);
					start_PWM_Generation_in_ISR_FLAG = 0; 
					OUTPUT_DIMMER = OFF;
					}
					M2=0;                
					man=0;
				}     
       

}
}

void applianceControl(char charSwitchMSB, char charSwitchLSB, char charSwitchSTATE, char chDimmerSpeedMSB, char chDimmerSpeedLSB,
        char charParentalControl, char charFinalFrameState){
    
    //define used variables and initilize it with zero
    int integerSwitchNumber = 0;
    int integerSwitchState = 0;
    int integerSpeed = 0;
    int currentStateBufferPositions=0;
   // TX1REG = charParentalControl;
    // Get switch Number in Integer format 
    //define all used character data types and initlize it with "#"
    char switchNumberStringBuffer[2]="#";
    char dimmerSpeedStringBuffer[2]="#";
    
    switchNumberStringBuffer[0]=charSwitchMSB;
    switchNumberStringBuffer[1]=charSwitchLSB;    
    integerSwitchNumber = atoi(switchNumberStringBuffer);//convert string into integer
    
    // Get switch State in Integer Format
    
    integerSwitchState = charSwitchSTATE-'0';
    
    // Get speed of Fan or level of dimmer    
    dimmerSpeedStringBuffer[0]=chDimmerSpeedMSB;
    dimmerSpeedStringBuffer[1]=chDimmerSpeedLSB;    
    integerSpeed = atoi(dimmerSpeedStringBuffer);
    
    // save Parental lock state of each switch into parental lock buffer
//    int integerParentalControl=charParentalControl-'0';
    parentalLockBuffer[integerSwitchNumber] = charParentalControl;
   
   
    copy_parentalLockBuffer[integerSwitchNumber]=parentalLockBuffer[integerSwitchNumber];
  //   TX1REG = parentalLockBuffer[integerSwitchNumber]; //ok same
 //    TX1REG = copy_parentalLockBuffer[integerSwitchNumber];
    
    
    // ACKNOWLEDGMENT data Format :->> (Gateway+SwitchState+SwitchMSB+SwitchLSB)
    
    currentStateBufferPositions = ((1+4*(integerSwitchNumber))-5);
    currentStateBuffer[currentStateBufferPositions++] = 'G';
    currentStateBuffer[currentStateBufferPositions++] = charSwitchSTATE;
    currentStateBuffer[currentStateBufferPositions++] = charSwitchMSB;
    currentStateBuffer[currentStateBufferPositions] = charSwitchLSB;    
    
    currentStateBufferPositions-=3;     // since we have come forward by 3 address in current state buffer
    if(charFinalFrameState=='1')    // until 
    {
        sendAcknowledgment(currentStateBuffer+currentStateBufferPositions);  
        __delay_ms(5);
        TX2REG = '(' ;
        __delay_ms(1);
        TX2REG = TouchMatikBoardAddress ;//touchmatoc address
        __delay_ms(1);
        TX2REG =charSwitchLSB + 16 ;
        __delay_ms(1);
        TX2REG=charSwitchSTATE;
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG='0';
        __delay_ms(1);
        TX2REG=')';
    }
    
    switch(integerSwitchNumber){
        case 1:
        {
             OUTPUT_RELAY1 = integerSwitchState;
        }
            break;
        case 2:
            {
              OUTPUT_RELAY2 = integerSwitchState;
             }
            break;
            
        case 3:
        {
            OUTPUT_RELAY3 = integerSwitchState;
        }
            break;
        case 4:
        {
            OUTPUT_RELAY4 = integerSwitchState;

        }
            break;
        case 5:
        {
            
                OUTPUT_RELAY5 = integerSwitchState;
        }
            break;
            
        case 6:
        {
                OUTPUT_RELAY6 = integerSwitchState;
        }
            break;
        case 7:
        {
#ifdef  SWITCH_7_RELAY
                OUTPUT_RELAY7 = integerSwitchState;
#endif
        }
            break;
        case 8:
        {
            start_PWM_Generation_in_ISR_FLAG = integerSwitchState;
            if(integerSwitchState == 0)
            {
                OUTPUT_DIMMER = ON;
            }
            else if(integerSwitchState == 1)
            {
                levelofDimmer_MSB = chDimmerSpeedMSB;
                levelofDimmer_LSB = chDimmerSpeedLSB;
            }
        }
        break;
        default:
            break;
        }
    
}



void actiontouchPanel(char Switch_Num, char sw_status) //, char speeds
{
         
 preventFlag1_Panel=0;preventFlag2_Panel=0;preventFlag3_Panel=0;preventFlag4_Panel=0;

    int switch_status = sw_status - '0';        
    int SwNum = Switch_Num - '@';//ASCII OF SWITCH NUMBER - ASCII OF @ i.e A>>65, B>>66, C>>67, D>>68 65-64=1 and so on
  // speeds = '0';
    char ch_sw_num = SwNum +'0';//send '1' for switch A, '2' for sww2 and so on 
 //   sendFeedback_TO_Gateway(sw_status,ch_sw_num);

   
    if(checkFlag == 1)
    {
        checkFlag=0;
        
    }
    else {
        
    __delay_ms(5);      TX1REG = 'G';
    __delay_ms(1);      TX1REG = sw_status;
    __delay_ms(1);      TX1REG = '0';
    __delay_ms(1);      TX1REG = Switch_Num - 16;
    switch(Switch_Num) {
       
       case 'A':
       {
       if(preventFlag1_Panel == OFF && copy_parentalLockBuffer[1] == CHAR_OFF )
         {
               OUTPUT_RELAY1 = switch_status;
               preventFlag1_Panel = ON;
          }
       }
       
       break;
       case 'B':
       {
         if(preventFlag2_Panel == OFF && copy_parentalLockBuffer[2] == CHAR_OFF  )
          {
                 OUTPUT_RELAY2 = switch_status;
                 preventFlag2_Panel = ON;
              
          }

       }
       
       break;
       case 'C':
       {
      if(preventFlag3_Panel == OFF  && copy_parentalLockBuffer[3] == CHAR_OFF )
         {
                OUTPUT_RELAY3 = switch_status;
                preventFlag3_Panel = ON;
          }
 
       }
       break;
       case 'D':
       {

           if(preventFlag4_Panel == OFF  && copy_parentalLockBuffer[4] == CHAR_OFF)
          {
              
                OUTPUT_RELAY4 = switch_status;
                preventFlag4_Panel = ON;
         }

       }
       break;
       default:
       break;
   }
    }  
}

        
    


/*
 * All input output pin initialization
 */
void GPIO_pin_Initialize(){
    clearAllPorts();
    pinINIT_extra();
    INPUT_SWITCH_DIR_1 = 1;
    INPUT_SWITCH_DIR_2 = 1;
    INPUT_SWITCH_DIR_3 = 1;
    INPUT_SWITCH_DIR_4 = 1;
    INPUT_SWITCH_DIR_5 = 1;
    INPUT_SWITCH_DIR_6 = 1;
   INPUT_SWITCH_DIR_7 = 1;
   INPUT_SWITCH_DIR_8 = 1;
    
    OUTPUT_RELAY_DIR_1 = 0;
    OUTPUT_RELAY_DIR_2 = 0;
    OUTPUT_RELAY_DIR_3 = 0;
    OUTPUT_RELAY_DIR_4 = 0;
    OUTPUT_RELAY_DIR_5 = 0;
    OUTPUT_RELAY_DIR_6 = 0;
   OUTPUT_RELAY_DIR_7 = 0;
//   OUTPUT_RELAY_DIR_8 = 0;
    OUTPUT_DIMMER_DIR = 0; // direction of PWM OUTPUT to MOC3021
    // peripherals directions
   // ZCD_CCP9_DIR = 1;
    ZCD_CCP1_DIR = 1;
    // USART DIRECTIONS
    USART_1_TRANSMIT_OUTPUT_DIR = 0;
    USART_1_RECIEVE_INPUT_DIR = 1;
    
    USART_2_TRANSMIT_OUTPUT_DIR = 0;
    USART_2_TRANSMIT_OUTPUT_DIR = 1;
    
    clearAllPorts();
}

/*
 * ALL Peripheral Initialization
 */
void allPeripheralInit(){
    EUSART_Initialize();
    EUSART2_Initialize();
    TMR1_Initialize();
    TMR3_Initialize();
    CCP1_Initialize();
}

/*
 * USART Control Registers initialization
 */
void EUSART_Initialize(){
    PIE1bits.RC1IE = 0;
    PIE1bits.TX1IE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE enabled; ABDEN disabled;
    BAUD1CON = 0x0A;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled;
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave;
    TX1STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12;
    //SPBRGL = 0x0C;
    //SPBRGL = 0x19;                  // SP1BRGL is 25 (hex value=0x19) for 9600 baud on 16 MHz crystal frequency
    SP1BRGL = 0xA0;                  // SYNC =0 ; BRGH = 1 ; BRG16=1;
    // Baud Rate = 9600; SP1BRGH 1;
    SP1BRGH = 0x01;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

    // Transmit Enabled
    TX1STAbits.TXEN = 1;

    // Serial Port Enabled
    RC1STAbits.SPEN = 1;
}
void EUSART2_Initialize()
{
    PIE4bits.RC2IE = 0;
    PIE4bits.TX2IE = 0;

    // Set the EUSART module to the options selected in the user interface.

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE enabled; ABDEN disabled;
    BAUD2CON = 0x0A;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled;
    RC2STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave;
    TX2STA = 0x24;

    // Baud Rate = 9600; SP1BRGL 12;
    SP2BRGL = 0xA0;                  // SYNC =0 ; BRGH = 1 ; BRG16=1;
    // Baud Rate = 9600; SP1BRGH 1;
    SP2BRGH = 0x01;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

    // enable receive interrupt    
    PIE4bits.RC2IE = 1; // handled into INTERRUPT_Initialize()
    // Transmit Enabled
    TX2STAbits.TXEN = 1;

    // Serial Port Enabled
    RC2STAbits.SPEN = 1;
}

void TMR1_Initialize(void)
{
   
    T1CON = 0x00;

    //T1GSS T1G; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO_nDONE done; T1GSPM disabled;
    T1GCON = 0x00;

        //TMR1H 29;
    TMR1H = 0x00;

    //TMR1L 112;
    TMR1L = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR1bits.TMR1IF = 0;

    // Enabling TMR1 interrupt.
    PIE1bits.TMR1IE = 1;

    // Start TMR1
   // T1CONbits.TMR1ON = 1;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

}

void TMR3_Initialize(void)
{

    T3CON = 0x00;

    //T1GSS T1G; TMR1GE disabled; T1GTM disabled; T1GPOL low; T1GGO_nDONE done; T1GSPM disabled;
    T3GCON = 0x00;

        //TMR1H 29;
    TMR3H = 0x00;
 
    //TMR1L 112;
    TMR3L = 0x00;

    // Clearing IF flag before enabling the interrupt.
    PIR3bits.TMR3IF = 0;

    // Enabling TMR1 interrupt.
    PIE3bits.TMR3IE = 1;

    // Start TMR1
   // T1CONbits.TMR1ON = 1;

    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;

}
//void CCP9_Initialize(){
//    // Set the CCP1 to the options selected in the User Interface
//
//    // MODE Every edge; EN enabled; FMT right_aligned;
//    CCP9CON = 0x84;
//
//    // RH 0;
//    CCPR9H = 0x00;
//
//    // RL 0;
//    CCPR9L = 0x00;
//    
////    CCPTMRS2bits.C9TSEL0=0;
////    CCPTMRS2bits.C9TSEL1=0;
//
//    // Clear the CCP1 interrupt flag
//    PIR4bits.CCP9IF = 0;
//
//    // Enable the CCP1 interrupt
//    PIE4bits.CCP9IE = 1;
//}
void CCP1_Initialize()
{
    // Set the CCP1 to the options selected in the User Interface

    // MODE Every edge; EN enabled; FMT right_aligned;
    CCP1CON = 0x84;

    // RH 0;
    CCPR1H = 0x00;

    // RL 0;
    CCPR1L = 0x00;

    // Clear the CCP1 interrupt flag
    PIR1bits.CCP1IF = 0;

    // Enable the CCP1 interrupt
    PIE1bits.CCP1IE = 1;
}

void AllInterruptEnable(){
    // Enable all active interrupts ---> INTCON reg .... bit 7            page 105
    GIE = 1;

    // Enables all active peripheral interrupts -----> INTCON reg .... bit 6         page 105
    PEIE = 1;
    
    // enable receive interrupt
    PIE1bits.RC1IE = 1;                    // handled into INTERRUPT_Initialize()

}

void errorsISR(char* errNum){
    int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void errorsMain(char* errNum){
   int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
 		TX1REG = *errNum;
 		*errNum++;
        Tx_count++;
 	}
}
void sendAcknowledgment(char* currentStateBuffer){
  int Tx_count=0;
  	while(Tx_count!=4)
 	{ 
        while (!TX1STAbits.TRMT);
//        TX1REG='S';
 		TX1REG = *currentStateBuffer;
 		*currentStateBuffer++;
        Tx_count++;
 	}
}
void sendFeedback_TO_Gateway(char sw_status, char Switch_Num){
    __delay_ms(5);      TX1REG = 'G';
    __delay_ms(1);      TX1REG = sw_status;
    __delay_ms(1);      TX1REG = '0';
    __delay_ms(1);      TX1REG = Switch_Num;
}
void sendFeedback_TO_Touch(char Switch_Num_1s, char sw_status){
  //  TX1REG='C';
    __delay_ms(5);      TX2REG = '(';
//    __delay_ms(1);      TX2REG = TouchMatikBoardAddress;
    __delay_ms(1);          TX2REG = 'f';
    __delay_ms(1);      TX2REG = Switch_Num_1s;
    __delay_ms(1);      TX2REG = sw_status;
//	__delay_ms(1);      TX2REG = '0';
//	__delay_ms(1);      TX2REG = '0';
//	__delay_ms(1);      TX2REG = '0';
    __delay_ms(1);      TX2REG = ')';
}
void copyReceivedDataBuffer(){
    int dataBufferCounter=2;
    for(dataBufferCounter=2;dataBufferCounter<9;dataBufferCounter++){
        tempReceivedDataBuffer[dataBufferCounter-2]=mainReceivedDataBuffer[dataBufferCounter]; // copy data buffer from main
        mainReceivedDataBuffer[dataBufferCounter]='#';  // clean data buffer
    }
}
void copyTouchpanelReceiveDataBuffer()
{
     int dataBufferCounter=2;
     for(dataBufferCounter=2; dataBufferCounter<5;dataBufferCounter++)
     {
         tempReceiveTouchpanelDataBuffer[dataBufferCounter-2] = touchpanleReceivedDatabuffer[dataBufferCounter];
         touchpanleReceivedDatabuffer[dataBufferCounter] = "#";
     }
}
/*
 * AANALOG and PULL up REGISTERS related initialization
 */
void pinINIT_extra(){
    ANSELG=0x00;    WPUG = 0;
    
    ANSELF=0x00; 
    
    ANSELE=0x00;    WPUE=0x00;
    
    ANSELD=0x00;    WPUD=0x00;
    
    ANSELB=0x00;    WPUB=0x00;
    
    ANSELA=0x00;     
} 

/*
 * always clear all the ports before initialization
 */
void clearAllPorts()
{
  //  TX1REG='C';
  OUTPUT_RELAY1 = 0;
  OUTPUT_RELAY2 = 0;
  OUTPUT_RELAY3 = 0;
  OUTPUT_RELAY4 = 0;
  OUTPUT_RELAY5 = 0;
  OUTPUT_RELAY6 = 0;
  OUTPUT_RELAY7 = 0;
//  OUTPUT_RELAY8 = 0;
  OUTPUT_DIMMER = 1;
}
 void writeUART(char *str2Write)
{
    int Tx_count=0;
 	//while (*str2Write != '\0' || Tx_count!=4)
  	while(Tx_count!=4)
 	{
        //while(PIR1bits.TXIF==0); 
        while (!TX1STAbits.TRMT);
 		TX1REG = *str2Write;
 		*str2Write++;
        Tx_count++;
 	}
}
