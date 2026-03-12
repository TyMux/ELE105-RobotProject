#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "PWM.h"
#include "I2C.h"
#define LED1 LATBbits.LB2
#define LED2 LATBbits.LB3
#define LED3 LATBbits.LB4
#define LED4 LATBbits.LB5

#define _XTAL_FREQ 10000000
void WaitFor(float TimeSeconds);
void SetUpLEDs(void);
void FlashLEDs(unsigned int Flashes);
void ShowTest(void);


void WaitFor(float TimeSeconds){
    for(int Times = 0; Times < (TimeSeconds/0.1); Times++){
        __delay_ms(100);
    }
}

void SetUpLEDs(void){
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
}

void FlashLEDs(unsigned int Flashes){
    SetUpLEDs();
    for(int i = 0; i < Flashes; i++){
        LED1 = 1;
        LED2 = 1;
        LED3 = 1;
        LED4 = 1;
        WaitFor(1);
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        LED4 = 0;
        WaitFor(1);
    }
    
}
void ShowTest(void){
    UpdateLineData();
    LED1 = (linesensor >> 2) & 1;
    LED2 = (linesensor >> 3) & 1;
    LED3  = (linesensor >> 4) & 1;
    LED4  = (linesensor >> 5) & 1;
    
}
int main(void){

    // I2C Has to go first
    I2C_INIT();
    PWM_INIT();
    SetUpLEDs();
    
    while(1){
        unsigned int Speed = 200;
        goforward(Speed);
        
        ShowTest();
    }

    
    
    
    
    
}
