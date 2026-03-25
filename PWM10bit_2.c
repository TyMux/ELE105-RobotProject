/* File:   pwm10bit_2.c 
 * Author: Craig Bacon
 * Created on 05 Jan 2017, 14:30
 * Mark space ratio is 10 bit (0-1023) and the PWM frequency is 610 Hz
 * PWM Period = (PR2+1)x4 x 1/Osc x TMR2 Prescaler
 * e.g (255+1)x4 x1/10Mhz x 16 =1.638ms or 610Hz.PR2 changes frequency.
 * The 10bit markspace ratio is placed in: 2 LSB in bits 4,5 of CCP1CON and 
 * the higher 8 bits in CCPR1L. Duplicated for CCP2CON and CCPR2L,Both motors 
 * turn forward. markspaceL Left motor speed, markspaceR Right motor speed
 */

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "PWM.h"
#pragma config OSC = HS     //High speed resonator
#pragma config WDT = OFF    //watchdog timer off
#pragma config LVP = OFF    //Low voltage programmer disabled
#pragma config PWRT = ON    //Power up timer on
#define Leftmotor1A LATAbits.LA4    //Direction bits Left motor
#define Leftmotor2A LATAbits.LA5
#define Rightmotor3A LATBbits.LB0   //Direction bits Right motor
#define Rightmotor4A LATBbits.LB1
void PWM_INIT(void); // Set Up PWM
void configPWM(void);       //Configure PWM
void goforward(unsigned int RightSpeed, unsigned int LeftSpeed);       //Turn both motors forward
void MovementPWM(void);     // Handles the PWM for translating movement to the registers
unsigned int markspaceL;    //Mark space ratio for Left motor
unsigned int markspaceR;    //Mark space ratio for Right motor


void PWM_INIT(void)
{
// StartUp Code ---------
ADCON1 = 0b00001101;    //AN0,AN1 are analogue inputs,RA2 -RA5 are digital 
TRISA = 0b11001111;    //Set PORTA pins
TRISB = 0;              	 //Set all PORTB pins to outputs
TRISC = 0b00111001;    //Set PORTC pins
LATB = 0;              	 //Turn LEDs off
configPWM();          	  //Configure PWM
//--------------
}




void goforward(unsigned int RightSpeed, unsigned int LeftSpeed){
    
    markspaceL = LeftSpeed;
    markspaceR = RightSpeed;
    
    Leftmotor1A = 0;    //Left motor forward;
    Leftmotor2A = 1;    
    Rightmotor3A = 0;   //Right motor forward;
    Rightmotor4A = 1;
    MovementPWM();
    return;
}  

void configPWM(void){   //Configures PWM
PR2 = 0b11111111 ;     //set period of PWM,610Hz
T2CON = 0b00000111 ;   //Timer 2(TMR2)on, prescaler = 16 
CCP1CON = 0b00001100;   //enable CCP1 PWM
CCP2CON = 0b00001100;   //enable CCP2 PWM
CCPR1L = 0;             //turn left motor off
CCPR2L = 0;             //turn Right motor off
return;
}




// This function is NESSECARY when moving the Motors translates the PWM to fit with the registers
void MovementPWM(void){
    CCP1CON = (0x0c)|((markspaceL&0x03)<<4);//0x0c enables PWM,then insert the 2 LSB
    CCPR1L = markspaceL>>2; //of markspaceL into CCP1CON and the higher 8 bits into
    CCP2CON = (0x0c)|((markspaceR&0x03)<<4); //CCPR1L.  Same as above but for 
    CCPR2L = markspaceR>>2;
    return;
}





