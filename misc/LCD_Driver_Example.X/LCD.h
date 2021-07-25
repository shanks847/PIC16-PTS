
#include <xc.h>

#define _XTAL_FREQ 48000000

#define LCD_EN_Delay 500
#define LCD_DATA_PORT_D TRISB
#define LCD_RS_D TRISB5
#define LCD_EN_D TRISB4
#define RS RB5
#define EN RB4
#define D4 RB0
#define D5 RB1
#define D6 RB2
#define D7 RB3

//==========================================
//-----[ Prototypes For All Functions ]-----

void LCD_Init();  // Initialize The LCD For 4-Bit Interface
void LCD_Clear(); // Clear The LCD Display
void LCD_SL();    // Shift The Entire Display To The Left
void LCD_SR();    // Shift The Entire Display To The Right

void LCD_CMD(unsigned char);   // Send Command To LCD
void LCD_DATA(unsigned char);  // Send 4-Bit Data To LCD
void LCD_Set_Cursor(unsigned char, unsigned char);  // Set Cursor Position
void LCD_Write_Char(char);     // Write Character To LCD At Current Position
void LCD_Write_String(char*);  // Write A String To LCD
//------------------------------------------
