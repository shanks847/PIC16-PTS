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
 

#define LCD_EN_Delay 500
#define LCD_DATA_PORT_D TRISD
#define LCD_RS_D TRISD0 // TRISX are used for configuring port for I or O
#define LCD_EN_D TRISD1
#define RS RD0
#define EN RD1
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7



//-----[ Alphanumeric LCD 16x2 Routines ]-----
void LCD_DATA(unsigned char Data)
{
    if(Data & 1) 
        D4 = 1;
    else
        D4 = 0;
    if(Data & 2)
        D5 = 1;
    else
        D5 = 0;
    if(Data & 4)
        D6 = 1;
    else
        D6 = 0;
    if(Data & 8) 
        D7 = 1;
    else
        D7 = 0;
}
void LCD_CMD(unsigned char CMD)
{
    // Select Command Register
    RS = 0;           
    // Move The Command Data To LCD
    LCD_DATA(CMD);
    // Send The EN Clock Signal
    EN  = 1;         
    __delay_us(LCD_EN_Delay);
    EN  = 0;         
}
void LCD_Clear()
{
    LCD_CMD(0);
    LCD_CMD(1);
}
void LCD_Set_Cursor(unsigned char r, unsigned char c)
{
    unsigned char Temp,Low4,High4;
    if(r == 1)
    {
      Temp  = 0x80 + c - 1; //0x80 is used to move the cursor
      High4 = Temp >> 4;
      Low4  = Temp & 0x0F;
      LCD_CMD(High4);
      LCD_CMD(Low4);
    }
    if(r == 2)
    {
      Temp  = 0xC0 + c - 1;
      High4 = Temp >> 4;
      Low4  = Temp & 0x0F;
      LCD_CMD(High4);
      LCD_CMD(Low4);
    }
}
void LCD_Init()
{
  // IO Pin Configurations
  LCD_DATA_PORT_D = 0x00;
  LCD_RS_D = 0;
  LCD_EN_D = 0;
  
  // The Init. Procedure As Described In The Datasheet
  LCD_DATA(0x00);
  __delay_ms(30);
  __delay_us(LCD_EN_Delay); 
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(0x03);
  __delay_us(150);
  LCD_CMD(0x03); 
  LCD_CMD(0x02);
  LCD_CMD(0x02);
  LCD_CMD(0x08);
  LCD_CMD(0x00);
  LCD_CMD(0x0C);
  LCD_CMD(0x00);
  LCD_CMD(0x06);
}
void LCD_Write_Char(char Data)
{
   char Low4,High4;
   Low4  = Data & 0x0F;
   High4 = Data & 0xF0;
   RS = 1;        
   LCD_DATA(High4>>4);  
   EN = 1;
   __delay_us(LCD_EN_Delay); 
   EN = 0;
   __delay_us(LCD_EN_Delay); 
   LCD_DATA(Low4); 
   EN = 1;
   __delay_us(LCD_EN_Delay); 
   EN = 0;
   __delay_us(LCD_EN_Delay); 
}
void LCD_Write_String(char *str)
{
    int i;
    for(i=0;str[i]!='\0';i++)
       LCD_Write_Char(str[i]);
}
void LCD_SL()
{
    LCD_CMD(0x01);
    LCD_CMD(0x08);
}
void LCD_SR()
{
    LCD_CMD(0x01);
    LCD_CMD(0x0C);
}
*/


/* ===========================
 * ULTRASONIC DISTANCE SENSOR
 * ============================
 * 
 * 
 * MODIFY CODE TO USE IOC ON RB4
 * 
 * 
 * void main(void){

    send_string("[!]Serial Connection successful");
    
    int a;

    TRISB = 0b00010000;           //RB4 as Input PIN (ECHO)


    T1CON = 0x10;                 //Initialize Timer Module

     while(1)
  {
    TMR1H = 0;                  //Sets the Initial Value of Timer
    TMR1L = 0;                  //Sets the Initial Value of Timer

    RB0 = 1;               //TRIGGER HIGH
    __delay_us(10);               //10uS Delay
    RB0 = 0;               //TRIGGER LOW
    
    while(!RB4);           //Waiting for Echo
    TMR1ON = 1;               //Timer Starts
    while(RB4);            //Waiting for Echo goes LOW
    TMR1ON = 0;               //Timer Stops

    a = (TMR1L | (TMR1H<<8));   //Reads Timer Value
    a = a*0.068;                //Converts Time to Distance
    //a = a + 1;
    
    //Distance Calibration
    if(a>=2 && a<=400)          //Check whether the result is valid or not
    {
        send_string("In range");
    }
    else
    {
      send_string("Out of Range");
    }
    __delay_ms(400);
  }
    
    return;
}

 * 
 */



/*
 * =================
 * ORIENTATION LUT
 * =================
 */

#define LDRI0       RA0
#define LDRI1       RA1

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

//char data;
//
//void send_string(const char *x)
//{
//    while(*x)
//    {
//        EUSART_Write(*x++);
//    }
//}



void main(void){
       
 
    
    
    
    /*
     * ===========================================
     * 
     * EUSART TRANSMISSION
     * ============================================= 
     
    */
    //Configuring EUSART for transmission
    //Initialize();  
    //PIN_MANAGER_Initialize();
    EUSART_Initialize();
    //Initialize();
    send_string("[*]Serial Connection successful");
    //send_char('U');
    while(1){}
    
    //while(1){}
   
    
    
    //ADCON1=0x06;             //All pins as digital
    //TRISA=0b111111;          //PortA as Input
    //TRISC2  = 0;             //Make CCP1 pin as output
    //CCP1CON = 0x0C;          //Configure CCP1 module in PWM mode
    //PR2   = 0xFF;            //Configure the Timer2 period
    //rundutycycle(512);
    
    
    
    
    return;
}

