/*
 * File:   main.c
 * Author: Khaled Magdy
 */
#include <xc.h>
#include "LCD.h"
#include "config.h"


void main(void) {
    
    LCD_Init();
    LCD_Clear();
    LCD_Set_Cursor(1,1);
    LCD_Write_String("Hello World\0");
    while(1)
    {
      
    }
    return;
}
