/*
 * File:   main.c
 * Author: Shankar Ramharack
 * UWI ID: 816019096
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

/* =======|
 * EUSART |
 * =======|
 */
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

/* =========|
 * LCD      |
 * =========|
 

#define LCD_EN_Delay 500
#define LCD_DATA_PORT_D TRISD
#define LCD_RS_D TRISD5 // TRISX are used for configuring port for I or O
#define LCD_EN_D TRISD4
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

void main(void){
       
    //ANSELD = 0x00;
    //TRISD = 0x00;
    //RD7=1;
    
    
//    LCD_Init();
//    LCD_Clear();
//    LCD_Set_Cursor(1,1);
//    LCD_Write_String("Hello World\0");
//    while(1)
//    {
//      
//    }
    
    
    
    /*
     * ===========================================
     * 
     * EUSART TRANSMISSION
     * ============================================= 
     
    */
    //Configuring EUSART for transmission
    Initialize();    
       
    send_string("----------------left----------------");
    //send_char(10);
    
    
    
    return;
}

