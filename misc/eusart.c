

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

char *itoa(int value); 


void adcconfig(void){
    
    ADPCH = 0b00010111;//Channel Selection
    
    ADREF = 0b00000000; //ADC Voltage Reference
    
    ADCON0 = 0b10000100;
    
    ADCON1 = 0b10000000;
 
}

int adcconvert(void){

ADCON0 = 0b10000101;
    do
    {
      
    } while(ADCON0bits.ADGO != 0);
    int temp = 0x0000;
    temp = ADRESH;
    temp = temp << 8;
    temp = temp | ADRESL;
    //send_string(itoa(temp));  // old test to ensure the internal module functioned 
    //send_char(10);
    
    return temp;
}


void main(void){
       
    ANSELD = 0x00;
    TRISD = 0x00;
    RD7=1;
    Initialize();    
       
    adcconfig ();
    int val_reg = 0b00010100;
    while (1){
        int temp_joy = 0;
        
        
        //send_string("Count: ");
        //send_string(itoa(count));
        //send_string(" - ");
        
        temp_joy = adcconvert ();
        
        
        if (temp_joy < 100){
            send_string("----------------left----------------");
        send_char(10);
        }
        else if (temp_joy > 200){
            send_string("----------------right----------------");
        send_char(10);
        }
        
        
       
    }
    
    return;
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