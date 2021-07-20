#define _XTAL_FREQ 8000000

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>
#include "lcd.h";
#include <pic16f877a.h>

// BEGIN CONFIG
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)
//END CONFIG

int a;

void interrupt echo()
{
  if(RBIF == 1)                 //Makes sure that it is PORTB On-Change Interrupt
  {
    RBIE = 0;                   //Disable On-Change Interrupt
    if(RB4 == 1)                  //If ECHO is HIGH
      TMR1ON = 1;                    //Start Timer
    if(RB4 == 0)                  //If ECHO is LOW
    {
      TMR1ON = 0;                    //Stop Timer
      a = (TMR1L | (TMR1H<<8))/58.82;  //Calculate Distance
    }
  }
  RBIF = 0;                     //Clear PORTB On-Change Interrupt flag
  RBIE = 1;                     //Enable PORTB On-Change Interrupt
}


void main()
{
  TRISB = 0b00010000;           //RB4 as Input PIN (ECHO)
  TRISD = 0x00; // LCD Pins as Output
  GIE = 1;                      //Global Interrupt Enable
  RBIF = 0;                     //Clear PORTB On-Change Interrupt Flag
  RBIE = 1;                     //Enable PORTB On-Change Interrupt

  Lcd_Init();

  Lcd_Set_Cursor(1,1);
  Lcd_Write_String("Developed By");
  Lcd_Set_Cursor(2,1);
  Lcd_Write_String("electroSome");

  __delay_ms(3000);
  Lcd_Clear();

  T1CON = 0x10;                 //Initialize Timer Module

  while(1)
  {
    TMR1H = 0;                  //Sets the Initial Value of Timer
    TMR1L = 0;                  //Sets the Initial Value of Timer

    RB0 = 1;               //TRIGGER HIGH
    __delay_us(10);               //10uS Delay
    RB0 = 0;               //TRIGGER LOW

    __delay_ms(100); //Waiting for ECHO
    a = a + 1; //Error Correction Constant

    if(a>=2 && a<=400)          //Check whether the result is valid or not
    {
      Lcd_Clear();
      Lcd_Set_Cursor(1,1);
      Lcd_Write_String("Distance = ");
      
      Lcd_Set_Cursor(1,14);
      Lcd_Write_Char(a%10 + 48);

      a = a/10;
      Lcd_Set_Cursor(1,13);
      Lcd_Write_Char(a%10 + 48);

      a = a/10;
      Lcd_Set_Cursor(1,12);
      Lcd_Write_Char(a%10 + 48);

      Lcd_Set_Cursor(1,15);
      Lcd_Write_String("cm");
    }
    else
    {
      Lcd_Clear();
      Lcd_Set_Cursor(1,1);
      Lcd_Write_String("Out of Range");
    }
    __delay_ms(400);
  }
}