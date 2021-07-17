/*
 * File:   main.c
 * Author: jack
 *
 * Created on July 17, 2021, 5:07 PM
 */


#pragma config FEXTOSC = XT     
#pragma config RSTOSC = EXT1X   
#pragma config CLKOUTEN = OFF  
#pragma config CSWEN = OFF      
#pragma config FCMEN = OFF     
#pragma config MCLRE = ON       
#pragma config PWRTE = OFF      
#pragma config LPBOREN = OFF    
#pragma config BOREN = OFF     
#pragma config BORV = LO        
#pragma config ZCD = OFF       
#pragma config PPS1WAY = OFF    
#pragma config STVREN = ON      
#pragma config WDTCPS = WDTCPS_31
#pragma config WDTE = OFF      
#pragma config WDTCWS = WDTCWS_7
#pragma config WDTCCS = SC      
#pragma config WRT = OFF        
#pragma config SCANE = not_available
#pragma config LVP = ON      
#pragma config CP = OFF         
#pragma config CPD = OFF       


#include <xc.h>
#define _XTAL_FREQ 2000000
#define Baud_rate 9600

int frequency = 5;

void Initialize(void)
{
    RB4PPS = 0x10;

    TX1STAbits.TXEN = 1;
    TX1STAbits.TX9 = 0;
    TX1STAbits.SYNC = 0;
    TRISCbits.TRISC6 = 0;

    RC1STAbits.CREN = 1;
    RC1STAbits.RX9 = 0;
    TRISCbits.TRISC7 = 1;

    SP1BRGH = 0x00;
    SP1BRGL = 0x32;
    TX1STAbits.BRGH = 1;
    BAUD1CONbits.BRG16 = 1;

    RC1STAbits.SPEN = 1;
    
    SPEN  = 1;
    
    TRISBbits.TRISB1=0;
    ANSELBbits.ANSB1=0;
    PORTBbits.RB1=1;
    
}

void send_char(char word){
    while(!TXIF);
    TXREG = word;
}

void send_string(char* st_pt){
    while(*st_pt)
        send_char(*st_pt++);
}


void main(void){
       
    ANSELD = 0x00;
    TRISD = 0x00;
    RD7=1;
    Initialize();    
       
    send_string("----------------left----------------");
    send_char(10);
    
    return;
}

