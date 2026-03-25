#include <xc.h>
#include "PWM.h"
#include "I2C.h"

#define LED1 LATBbits.LB2
#define LED2 LATBbits.LB3
#define LED3 LATBbits.LB4
#define LED4 LATBbits.LB5

#define _XTAL_FREQ 10000000

#define K 20        // proportional gain
#define Lambda 1

// Function prototypes
void WaitFor(unsigned int TimeSeconds);
void SetUpLEDs(void);
void FlashLEDs(unsigned int Flashes);
void ShowTest(void);
void MovementSystem(unsigned int Speed);
void Move(unsigned int Speed, int RightRatio);
int FollowLine(unsigned int Speed);
int LookUpRobotLineOffset(void);

// External variable (make sure this exists in your I2C code)


// ----------------------------

void WaitFor(unsigned int TimeSeconds){
    for(unsigned int i = 0; i < TimeSeconds * 10; i++){
        __delay_ms(100);
    }
}

// ----------------------------

void SetUpLEDs(void){
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB5 = 0;
}

// ----------------------------

void FlashLEDs(unsigned int Flashes){
    SetUpLEDs();
    for(unsigned int i = 0; i < Flashes; i++){
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

// ----------------------------

void ShowTest(void){
    UpdateLineData();

    LED1 = (linesensor >> 2) & 1;
    LED2 = (linesensor >> 3) & 1;
    LED3 = (linesensor >> 4) & 1;
    LED4 = (linesensor >> 5) & 1;
}

// ----------------------------

int main(void){
    I2C_INIT();
    PWM_INIT();
    SetUpLEDs();

    unsigned int Speed = 300;

    while(1){
        UpdateLineData();
        MovementSystem(Speed);
    }
}

// ----------------------------

void MovementSystem(unsigned int Speed){
    int RightRatio = FollowLine(Speed);
    //Move(Speed, RightRatio);
}

// ----------------------------

void Move(unsigned int Speed, int RightRatio){

    if(Speed > 1023){
        Speed = 1023;
    }

    unsigned int LeftSpeed = Speed;
    unsigned int RightSpeed = (unsigned int)(Speed * (RightRatio/100));

    goforward(RightSpeed, LeftSpeed);
}

// ----------------------------

int FollowLine(unsigned int Speed){

    int theta = LookUpRobotLineOffset();

    if(theta == 999){
        return 1; // straight
    }

    int e = -theta;
    int u = K * e;

    unsigned int vR = Speed + Lambda * u;
    
    unsigned int vL = Speed - Lambda * u;

    if(vL == 0){
        return 1;
    }

    goforward(vR, vL);
    return (int)(vR * 100) / vL; // scaled ratio
}

// ----------------------------

int LookUpRobotLineOffset(void){
    switch(linesensor){

        case 0x80: return -12;
        case 0xC0: return -10;
        case 0x40: return -9;
        case 0x60: return -7;
        case 0x20: return -5;
        case 0x30: return -3;
        case 0x10: return -2;
        case 0x18: return 0;
        case 0x08: return 2;
        case 0x0C: return 3;
        case 0x04: return 5;
        case 0x06: return 7;
        case 0x02: return 9;
        case 0x03: return 10;
        case 0x01: return 12;

        default: return 999;
    }
}