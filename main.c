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

#define DPSW0           RA0
#define DPSW1           RA1
#define DPSW2           RA2
#define DPSW3           RA3
#define ECHO            RB4
#define TRIG            RB3
#define OC_LEC          RB2 //LED for optical encoder
#define LDRI0           RB1
#define LDRI1           RB0


/* ============|
 * SERIAL COMS |
 * ============|
 */


void cfg_eusart(void)
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

void cfg_pwm(void)
{
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    
    ANSELAbits.ANSA0 = 0;
    ANSELAbits.ANSA1 = 0;
    ANSELAbits.ANSA2 = 0;
    ANSELAbits.ANSA3 = 0;
    
    TRISBbits.TRISB5 = 0; 
    ANSELBbits.ANSB5 = 0; 
    RB5PPS = 0x0A;  //Routing CCP2 peripheral to pin 5 on port b
    CCP2CON = 0x9F;
    CCPTMRS0 = 0x05;
    PR2 = 19;
    CCPR2L = 0xFF;
    CCPR2H = 0xFF;
    T2CLKCON = 0x01;
    T2CON = 0xF0;
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



/* ===========================
 * ULTRASONIC DISTANCE SENSOR
 * ============================
 * 
 */

void ranging_sys_init(void){

    // configuring IO pins for ranging(orientation included)
    TRISBbits.TRISB4 = 1; // ECHO pin - input
    TRISBbits.TRISB3 = 0; // TRIG pin - output
    TRISBbits.TRISB2 = 0; // LED on encoder - output
    TRISBbits.TRISB1 = 1; // LDR0 - input
    TRISBbits.TRISB0 = 1; // LDR1 - input
    ANSELB = 0x00; // all pins on port b are digital
    
    LATBbits.LATB2 = 1;
    
    
    
    T0CON0bits.T0EN = 0;    // Disabling TMR0
    T0CON0bits.T016BIT = 1; // configuring TMR0 for 16 bit operation
    T0CON0bits.T0OUTPS = 0b0000; // setting TMR0 pre-scaler to none
    
    T0CON1bits.T0CS = 0b010;    // selecting Fosc/4 as clk source
    T0CON1bits.T0ASYNC = 1;     // configuring TMR0 for async operaiton
    T0CON1bits.T0CKPS = 0b0001; // setting TMR0 post-scaler as 1:2

  
    
}



int calcAngle(void)
{
    int angle = 0;
    
    //LDRI0 is the inner photo resistor
  
    if((LDRI0 == 1) && (LDRI1 == 0))
    {
        //90 deg
        angle = 90;
    }
    else if((LDRI0 == 1) && (LDRI1 == 1))
    {
        //180 deg
        angle = 180;
    }
    else if((LDRI0 == 0) && (LDRI1 ==1))
    {
        //270deg
        angle = 270;
    }
    else
    {
        //0 or 360deg or error
        angle = 0;   
    }
    
    return angle;
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
     static char buffer[12];        // using 12 bytes to hold int32 dtype
     int original = value;        // save original value
 
     int c = sizeof(buffer)-1;
 
     buffer[c] = 0;                // write null char at the end   
 
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

int distance = 0; // for storing distance


/*
 * =====================
 * PWM BLDC FAN CONTROL
 * ======================
 * 
 */
            
void set_pwm_dc(int dc)
{
        if(dc>20)
        {
            CCPR2H = 20;
            //CCPR2L = 0XFF;
        }
        else if(dc <= 0)
        {
            CCPR2H = 0;
            //CCPR2L = 0;
        }
        else{
            CCPR2H = dc;
            //CCPR2L = 0XFF;
        }
}

int dpsw_to_dc(void)
{
    //converts the switch combination from 4DIP-SW to a duty cycle value between 0-20
    int d0;
    int d1;
    int d2;
    int d3;
    
    if(DPSW0){d0 = 1000;} else {d0 = 0;}
    if(DPSW1){d1 = 100;} else {d1 = 0;}
    if(DPSW2){d2 = 10;} else {d2 = 0;}
    if(DPSW3){d3 = 1;} else {d3 = 0;}
    
    int binary_d = d0 + d1 + d2 + d3;

    if(binary_d == 0)
    {
        return 0;
    }
    
    int num_base = 1;
    int rem = 0;
    int decimal_num = 5; //offset to allow reaching max speed at 0b1111
    
    while ( binary_d > 0)  
    {  
        rem = binary_d % 10; /* divide the binary number by 10 and store the remainder in rem variable. */  
        decimal_num = decimal_num + rem * num_base;  
        binary_d = binary_d/ 10; // divide the number with quotient  
        num_base = num_base * 2;  
    }  
    
    
    return decimal_num;
}
    

void main(void){
    
    
    // Configuring the EUSART 
    cfg_eusart();
    cfg_pwm();
    ranging_sys_init();
    
    /*================
     * TESTING PWM
     * =============
    while(1){
    
    set_pwm_dc(20);
    
    __delay_ms(5000);
    
    set_pwm_dc(0);
     
    __delay_ms(5000);
    
     set_pwm_dc(20);
    }
    */
    
 
    
    /*
     * ================
     * TESTING 4-DIP SW 
     * =================
    __delay_ms(1000);
    send_string("[!]Connected\r\n");
    while(1){
        send_string(itoa(dpsw_to_dc()));
        send_string("\r\n");
        __delay_ms(3000);
        
        //if(PORTEbits.RE2 == 1){LATDbits.LATD1 = 1;}else{LATDbits.LATD1 = 0;} //testing if photo resistor working
    }
     * */
//    
    
    
                
    //__delay_ms(1000);   // to avoid corrupted characters in serial tx
    //send_string("[*]Serial Connection successful\r\n");
    
    //send_string(itoa(143)); //remember to add a \r\n for clean formatting
    //angle = "90\r\n"; //the \r\n are used to go to a new line
    //send_string("Angle: "); 
    //send_string(angle);
   
    /* ===============
     * RANGING SYSTEM
     * =================
     
    TRISBbits.TRISB4 = 1; // setting port  RB0 as output and RB4 as input
    TRISBbits.TRISB3 = 0;
    ANSELBbits.ANSB4 = 0;
    ANSELBbits.ANSB3 = 0;

   while(1)
    {
        TMR0H = 0;                  //Sets the Initial Value of Timer
        TMR0L = 0;                  //Sets the Initial Value of Timer
        T0CON1bits.T0CKPS = 0b0001; //Resetting post scaler since TMR0L write clears it

        PORTBbits.RB3 = 1;               //TRIGGER HIGH
        __delay_us(10);               //10uS Delay
        PORTBbits.RB3 = 0;               //TRIGGER LOW

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
//    
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

    
    /* Testing Angle system
    
    __delay_ms(1000);   // to avoid corrupted characters in serial tx
    send_string("[*]Serial Connection successful\r\n");
    
    while(1){
        send_string("Angle = ");
        send_string(itoa(calcAngle()));
        send_string(" deg\r\n");
        __delay_ms(1000);
    }
    */
    
    
    
    // INTEGRATION TEST
    __delay_ms(1000);   // to avoid corrupted characters in serial tx
    send_string("[*]Serial Connection successful\r\n");
    
    int dc = 0;
    
    while(1)
    {
        set_pwm_dc(dpsw_to_dc());
        TMR0H = 0;                  //Sets the Initial Value of Timer
        TMR0L = 0;                  //Sets the Initial Value of Timer
        T0CON1bits.T0CKPS = 0b0001; //Resetting post scaler since TMR0L write clears it

        PORTBbits.RB3 = 1;               //TRIGGER HIGH
        __delay_us(10);               //10uS Delay
        PORTBbits.RB3 = 0;               //TRIGGER LOW

        while(!PORTBbits.RB4);           //Waiting for Echo
        T0CON0bits.T0EN = 1;               //Timer Starts
        while(PORTBbits.RB4);            //Waiting for Echo goes LOW
        T0CON0bits.T0EN = 0;               //Timer Stops

        a = (TMR0L | (TMR0H<<8));   //Reads Timer Value
        a = a*0.068;                //Converts Time to Distance
        a = a + 1; //TODO: change this value to reflect correct calibration
        
        dc = CCPR2H; // duty cycle
        
        if(a>=2 && a<=400)          //Check whether the result is valid or not
        {

            send_string("Distance = ");
            send_string(itoa(a));
            send_string(" cm");
          
            send_string("\tAngle = ");
            send_string(itoa(calcAngle()));
            send_string(" deg");
            //__delay_ms(1000);
            
            send_string("\tPWM Duty Cycle = ");
            send_string(itoa(dc));
            send_string("\r\n");
        }
        else
        {
          send_string("Out of Range\r\n");
        }
        __delay_ms(400);
    }  
    
    
    
    return;
}

