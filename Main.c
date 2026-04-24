#include <xc.h>
#include "PWM.h"
#include "I2C.h"

#define LED1 LATBbits.LB2
#define LED2 LATBbits.LB3
#define LED3 LATBbits.LB4
#define LED4 LATBbits.LB5
#define LEncoder PORTCbits.RC0
int LEncoderReading = 0;
#define REncoder PORTCbits.RC5
int REncoderReading = 0;


#define _XTAL_FREQ 10000000
#define QuarterPulses 130
#define BaseSpeed 300
int CurrentSpeed = 0;
#define K 30        // proportional gain
#define Lambda 1

// Function prototypes
void WaitFor(unsigned int TimeSeconds);
void SetUpLEDs(void);
void FlashLEDs(unsigned int Flashes);
void ShowTest(void);
void MovementSystem(void);
void Move(unsigned int Speed, int RightRatio);
int FollowLine(void);
int LookUpRobotLineOffset(void);




int ReadEncoderL(void){
    int pulse = 0;
    if(LEncoder == 1 && LEncoderReading == 0) {
        LEncoderReading = LEncoder;
        pulse = 1;
    }
    LEncoderReading = LEncoder;
    return pulse;
    
}



void Turn(unsigned int Quarters){
    int TotalPulses = Quarters * QuarterPulses;
    int CurrentPulses = 0;
    while (CurrentPulses < TotalPulses){
        Move(0, 250);
        CurrentPulses += ReadEncoderL();
    }
    Move(0,0);
    WaitFor(5);
}

void WaitFor(unsigned int TimeSeconds){
    for(unsigned int i = 0; i < TimeSeconds * 10; i++){
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



void ShowTest(void){
    UpdateLineData();

    LED1 = (linesensor >> 2) & 1;
    LED2 = (linesensor >> 3) & 1;
    LED3 = (linesensor >> 4) & 1;
    LED4 = (linesensor >> 5) & 1;
}



int main(void){
    I2C_INIT();
    PWM_INIT();
    SetUpLEDs();

    CurrentSpeed = BaseSpeed;

    while(1){
        UpdateLineData();
        MovementSystem();
    }
}



void MovementSystem(void){
    int Difference = FollowLine();
    if(Difference == -111){
        Turn(2);
        CurrentSpeed = 0;
    }
    else if(Difference == 999){
        CurrentSpeed = 150;
        Difference = 0;
    }
    else{
        CurrentSpeed = BaseSpeed;
    }
    Move(CurrentSpeed, Difference);
}


void Move(unsigned int Speed, int Difference){

    if(Speed > 1023){
        Speed = 1023;
    }
    
    int LeftSpeed = Speed - Difference;
    int RightSpeed = Speed + Difference;

    goforward(RightSpeed, LeftSpeed);
}



int FollowLine(void){ //  Returns the difference

    int theta = LookUpRobotLineOffset();

    if(theta == 999){
        return 999; // straight
    }
    if(theta == -111){
        return -111; // Hit an event marker
    }

    int e = -theta;
    return K * e * Lambda;

}



    






int LookUpRobotLineOffset(void){
    unsigned char inverted = (~linesensor) & 0xFF; // REMOVE THIS LATER
    //switch(linesensor){ Add this later
    switch(inverted){

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
        
        
        case 0xFF: return -111;
        
        default: return 999;
    }
}