/*
 * File:   main.c
 * Author: Shankar Ramharack
 * UWI ID: 816019096
 * Created on July 17, 2021, 5:07 PM
 */


// CONFIG1
#pragma config FEXTOSC = XT    // External Oscillator mode selection bits->XT (crystal oscillator) above 500kHz, below 4MHz; PFM set to medium power
#pragma config RSTOSC = EXT1X    // Power-up default value for COSC bits->EXTOSC operating per FEXTOSC bits
#pragma config CLKOUTEN = OFF    // Clock Out Enable bit->CLKOUT function is disabled; i/o or oscillator function on OSC2
#pragma config CSWEN = ON    // Clock Switch Enable bit->Writing to NOSC and NDIV is allowed
#pragma config FCMEN = ON    // Fail-Safe Clock Monitor Enable bit->FSCM timer enabled

// CONFIG2
#pragma config MCLRE = ON    // Master Clear Enable bit->MCLR pin is Master Clear function
#pragma config PWRTE = OFF    // Power-up Timer Enable bit->PWRT disabled
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit->ULPBOR disabled
#pragma config BOREN = ON    // Brown-out reset enable bits->Brown-out Reset Enabled, SBOREN bit is ignored
#pragma config BORV = LO    // Brown-out Reset Voltage Selection->Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices
#pragma config ZCD = OFF    // Zero-cross detect disable->Zero-cross detect circuit is disabled at POR.
#pragma config PPS1WAY = ON    // Peripheral Pin Select one-way control->The PPSLOCK bit can be cleared and set only once in software
#pragma config STVREN = ON    // Stack Overflow/Underflow Reset Enable bit->Stack Overflow or Underflow will cause a reset
#pragma config DEBUG = OFF    // Background Debugger->Background Debugger disabled

// CONFIG3
#pragma config WDTCPS = WDTCPS_31    // WDT Period Select bits->Divider ratio 1:65536; software control of WDTPS
#pragma config WDTE = OFF    // WDT operating mode->WDT Disabled, SWDTEN is ignored
#pragma config WDTCWS = WDTCWS_7    // WDT Window Select bits->window always open (100%); software control; keyed access not required
#pragma config WDTCCS = SC    // WDT input clock selector->Software Control

// CONFIG4
#pragma config WRT = OFF    // UserNVM self-write protection bits->Write protection off
#pragma config SCANE = available    // Scanner Enable bit->Scanner module is available for use
#pragma config LVP = ON    // Low Voltage Programming Enable bit->Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.

// CONFIG5
#pragma config CP = OFF    // UserNVM Program memory code protection bit->Program Memory code protection disabled
#pragma config CPD = OFF    // DataNVM code protection bit->Data EEPROM code protection disabled



#include <xc.h>
#include <stdbool.h>
#include <stdint.h>


#define _XTAL_FREQ 2000000
#define Baud_rate 1200

/* =======|
 * EUSART |
 * =======|
 */



void EUSART_Initialize(void)
{
    TRISCbits.TRISC6 = 0;   // configuring pin 6 on port c as output
    ANSELCbits.ANSC6 = 0;   // configuring pin 6 on port c as digital output
    RC6PPS = 0x10;          // configuring pin 6 on port c for 
                            // serial transmission(TX)
    SP1BRGH = 0x00;         // setting lower reg to 0 
    SP1BRGL = 0b00011001;   // Setting baud rate of 1200
    BAUD1CONbits.BRG16 = 0; // configuring BRG for 8-bit mode
    
    TX1STAbits.BRGH = 0;    // configures BRG for low baud rate
    TX1STAbits.SYNC = 0;    // configures EUSART for async. transmission
    
    RC1STAbits.SPEN = 1;    // enables serial port
    
    TX1STAbits.TXEN = 1;    // enables serial transmission
    TX1STAbits.TX9 = 0;     // disables 9 bit transmission
    

}


void send_char(char word){
    while(!TXIF);
    TXREG = word;
}

void send_string(char* st_pt)
{
    while(*st_pt)
        send_char(*st_pt++);
}


/* =========|
 * LCD      |
 * =========|
 
*/



/* ===========================
 * ULTRASONIC DISTANCE SENSOR
 * ============================
 * 
 * 

 * 
 */

void ranging_sys_init(void){

    
    T0CON0bits.T0EN = 0;    // Disabling TMR0
    T0CON0bits.T016BIT = 1; // configuring TMR0 for 16 bit operation
    T0CON0bits.T0OUTPS = 0b0000; // setting TMR0 pre-scaler to none
    
    T0CON1bits.T0CS = 0b010;    // selecting Fosc/4 as clk source
    T0CON1bits.T0ASYNC = 1;     // configuring TMR0 for async operaiton
    T0CON1bits.T0CKPS = 0b0001; // setting TMR0 post-scaler as 1:2

}



/*
 * =================
 * ORIENTATION LUT
 * =================
 */

#define LDRI0       RA0
#define LDRI1       RA1
/*REFACTOR TO USE itoa_opt
char* angle;
void calcAngle()
{

    //1)Poll pins RA0 and RA1
    
    if(LDRI0 == 1 && LDRI1 == 0)
    {
        //90 deg
        angle = "90";
    }
    else if(LDRI0 == 1 && LDRI1 == 1)
    {
        //180 deg
        angle = "180";
    }
    else if(LDRI0 == 0 && LDRI1 == 1)
    {
        //270deg
        angle = "270";
    }
    else
    {
        //0 or 360deg or error
        angle = "0";   
    }
}
*/


/*
 * =====================
 * PWM BLDC FAN CONTROL
 * ======================
 * 
 * TODO: MODIFY CODE TO WORK WITH MY PIC AND ADJUST TMR2 VALUE
 */

#define DPSW0           RC2
#define DPSW1           RC3


void rundutycycle(unsigned int dutycyc){
    
    //This needs to be modified to work, ref Lab Script #2 from ECNG LAB III
    
    T2CON = 0x01;           // Set Prescaler to be 4, hence PWM frequency is set to 4.88KHz.
    T2CON |= 0x04;          // Enable the Timer2, hence enable the PWM. 
                            // the |= sets the bits, ref xc8 mannual for info

    while(1){
        CCPR1L   = dutycyc>>2;                  // Put MSB 8 bits in CCPR1L
        CCP1CON &= 0xCF;                        // Make bit4 and 5 zero
        CCP1CON |= (0x30&(dutycyc<<4));         // Assign Last 2 LSBs to CCP1CON
        if(DPSW0==1 && DPSW0==0){dutycyc=172; }
        else if(DPSW1==1 && DPSW0==1){dutycyc=512; }
        else if(DPSW1==0 && DPSW0==1){dutycyc=858; }
        else {dutycyc=1020;}

        dutycyc=dutycyc; //is this line necessary?
    }
}

char itoa_opt(int x)
{
    switch(x)
    {
        case 1:return '1';
        case 2:return '2';
        case 3:return '3';
        case 4:return '4';
        case 5:return '5';
        case 6:return '6';
        case 7:return '7';
        case 8:return '8';
        case 9:return '9';
        case 0:return '0';
        default: return'x'; 
    }
}

char *itoa(int value) 
 {
     static char buffer[12];        // 12 bytes is big enough for an INT32
     int original = value;        // save original value
 
     int c = sizeof(buffer)-1;
 
     buffer[c] = 0;                // write trailing null in last byte of buffer    
 
     if (value < 0)                 // if it's negative, note that and take the absolute value
         value = -value;
     
     do                             // write least significant digit of value that's left
     {
         buffer[--c] = (value % 10) + '0';    
         value /= 10;
     } while (value);
 
     if (original < 0) 
         buffer[--c] = '-';
 
     return &buffer[c];
 }

int a = 0;

void main(void){
    //TRISDbits.TRISD1 = 0;
    //LATDbits.LATD1 = 1; // for checking if PIC is working

    //Configuring EUSART for transmission
    EUSART_Initialize();
    ranging_sys_init();


    
    __delay_ms(1000);   // to avoid corrupted characters in serial tx
    send_string("[*]Serial Connection successful\r\n");
    
    //send_string(itoa(143)); //remember to add a \r\n for clean formatting
    //angle = "90\r\n"; //the \r\n are used to go to a new line
    //send_string("Angle: "); 
    //send_string(angle);
   
    TRISB = 0xFE; // setting port  RB0 as output and RB4 as input
    ANSELB = 0xEF;

   while(1)
    {
        TMR0H = 0;                  //Sets the Initial Value of Timer
        TMR0L = 0;                  //Sets the Initial Value of Timer
        T0CON1bits.T0CKPS = 0b0001; //Resetting post scaler since TMR0L write clears it

        PORTBbits.RB0 = 1;               //TRIGGER HIGH
        __delay_us(10);               //10uS Delay
        PORTBbits.RB0 = 0;               //TRIGGER LOW

        while(!PORTBbits.RB4);           //Waiting for Echo
        T0CON0bits.T0EN = 1;               //Timer Starts
        while(PORTBbits.RB4);            //Waiting for Echo goes LOW
        T0CON0bits.T0EN = 0;               //Timer Stops

        a = (TMR0L | (TMR0H<<8));   //Reads Timer Value
        a = a*0.068;                //Converts Time to Distance
        a = a + 1; //TODO: change this value to reflect correct calibration
        
        if(a>=2 && a<=400)          //Check whether the result is valid or not
        {

          send_string("Distance = ");
          send_string(itoa(a));
          send_string(" cm\r\n");
        }
        else
        {
          send_string("Out of Range\r\n");
        }
        __delay_ms(400);
    }  
    
    
    

//    
//    
//    
//    
//    
//    
//    /*
//     * TESTING FOR USART STRING FORMATTING AND ITOA
//     * 
//     *  while(1){
//            int a = 231;
//            int digit_0 = a%10;
//            a = a/10;
//            int digit_1 = a%10;
//            a = a/10;
//            int digit_2 = a%10;
//            char dist_str[3] = {itoa_opt(digit_0),itoa_opt(digit_1),itoa_opt(digit_2)};
//            send_string("Distance = ");
//            send_string(dist_str);
//            send_string(" cm");
//            send_string("\r\n");
//            __delay_ms(1000);
//        }   
//     */

    
    
    
    return;
}

