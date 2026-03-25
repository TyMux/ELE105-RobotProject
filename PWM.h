/* 
 * File:   PWM.h
 * Author: ELA24TML
 *
 * Created on 04 March 2026, 14:27
 */

#ifndef PWM_H
#define	PWM_H

void PWM_INIT(void);
void configPWM(void);       //Configure PWM
void goforward(unsigned int RightSpeed, unsigned int LeftSpeed);      //Turn both motors forward
void MovementPWM(void);  // Movement config for all movement
unsigned int markspaceL;    //Mark space ratio for Left motor
unsigned int markspaceR; 

#endif	/* PWM_H */

