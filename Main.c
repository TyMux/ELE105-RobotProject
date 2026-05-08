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
int disable_movement_system = 0;

#define _XTAL_FREQ 10000000
#define EighthPulses 60
#define BaseSpeed 350
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
int LeftDistanceSensor();
int RightDistanceSensor();
int SlowDown(int leftSensorValue,int rightSensorValue,int Speed);
void ChangeLane();
int events_position = 0;

int LeftDistanceSensor() {
    //LS is channel 0
    ADCON0 = 0b0000011; //initialises ports and sets reading val to left sensor
    int leftSensorValue = 0;
    do {
        leftSensorValue = (ADRESH<<8)+ADRESL; //saves val for ls to variable
    } while(ADCON0bits.GO);  
    return leftSensorValue;
}
int RightDistanceSensor() {
    //RS is channel 1
    ADCON0 = 0b0000111; //initialises ports and sets reading val to left sensor
    int rightSensorValue = 0;
    do {
        rightSensorValue = (ADRESH<<8)+ADRESL;
    } while(ADCON0bits.GO);
    return rightSensorValue;
}

int ReadEncoderL(void){
    int pulse = 0;
    if(LEncoder == 1 && LEncoderReading == 0) {
        LEncoderReading = LEncoder;
        pulse = 1;
    }
    LEncoderReading = LEncoder;
    return pulse;
    
}

void ChangeLane() {
    Turn(1);
    WaitFor(1);
    LED1 = 1;
    
    /*
    UpdateLineData();
    int Difference = FollowLine();
    while (Difference != -333){
        UpdateLineData();
        Difference = FollowLine();
        Move((BaseSpeed - 200),0);
    }
    
    while (Difference == 999) {
        UpdateLineData();
        Difference = FollowLine();
        Move((BaseSpeed - 200),0);
    }*/
    
    Move(300,0);
    WaitFor(2);
    
    LED1 = 0;
    disable_movement_system = 0;
}

void Turn(unsigned int Eighths){
    int TotalPulses = Eighths * EighthPulses;
    int CurrentPulses = 0;
    while (CurrentPulses < TotalPulses){
        Move(0, 250);
        CurrentPulses += ReadEncoderL();
    }
    Move(0,0);
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
    FlashLEDs(3);
    
    int finished = 0;
    
    int events[] = {1,2,3,1,2,4};

    while(events_position < 6) {
        
        if (disable_movement_system == 1) {
            switch (events[events_position]) {
                case 1:
                    //change lane
                    ChangeLane();
                    disable_movement_system = 0;
                    events_position += 1;
                    break;
                case 2:
                    //stop robot / lights
                    Move(0,0);
                    FlashLEDs(3);
                    events_position += 1;
                    break;
                case 3:
                    //Turn around
                    Turn(4);
                    WaitFor(5);
                    disable_movement_system = 0;
                    events_position += 1;
                    break;
                case 4:
                    //End
                    disable_movement_system = 1;
                    events_position += 1;
                    finished = 1;
                    break;
        }
            
        }
        else {
            UpdateLineData();
            MovementSystem();
        }
        while (finished == 1) {
            disable_movement_system = 1;
        }
    }
    
    
    
    /*
    while(1){
        if (disable_movement_system == 1) {
            
        }
        else {
            UpdateLineData();
            MovementSystem();
        }
        
        
    }*/
}

int SlowDown(int leftSensorValue,int rightSensorValue,int Speed){
    //For the sake of decency we'll use the closest sensor value.
    //Using ranges of 24000 - 4000
    int speed = 0;
    if (leftSensorValue > 24000 || rightSensorValue > 24000) {
        return 0;
    }
    
    if (leftSensorValue > rightSensorValue) {
        return (Speed - ((leftSensorValue-4000) / 50));
    }
    else if (rightSensorValue > leftSensorValue) {
        return (Speed - ((rightSensorValue-4000) / 50));
    }
    else {
        return (Speed - ((((rightSensorValue+leftSensorValue)/2)-4000) / 50));
    }
}

void MovementSystem(void){
    int Difference = FollowLine();
    if(Difference == -111){
        //This means there is a full line.
        disable_movement_system = 1;
        //Turn(2);
        CurrentSpeed = BaseSpeed;
        
    }
    else if(Difference == 999){
        CurrentSpeed = 150;
        Difference = 0;
    }
    else{
        CurrentSpeed = BaseSpeed;
    }
    
    int rightSensorValue = RightDistanceSensor(); 
    int leftSensorValue = LeftDistanceSensor(); 
    
    if (rightSensorValue > 4000 || leftSensorValue > 4000){
        CurrentSpeed = SlowDown(leftSensorValue,rightSensorValue,CurrentSpeed);
    }
    
    if (CurrentSpeed < 0) {
        CurrentSpeed = 0;
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
    //switch(linesensor){
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
        case 0x00: return -333;
        
        default: return 999;
    }
}
