/* 
 * File:   I2C.h
 * Author: ELA24TML
 *
 * Created on 04 March 2026, 14:31
 */

#ifndef I2C_H
#define	I2C_H

void I2C_INIT(void);
void I2C_Initialise(void);      	//Initialise I2C
void I2C_checkbus_free(void);   //Wait until I2C bus is free
void I2C_Start(void);           	//Generate I2C start condition
void I2C_RepeatedStart(void);  	 //Generate I2C Repeat start condition
void I2C_Stop(void);           	 //Generate I2C stop condition
void I2C_Write(unsigned char write);    //Generate I2C write condition
unsigned char I2C_Read(void);   	//Generate I2C read condition
unsigned char linesensor;
void UpdateLineData(void);

#endif	/* I2C_H */

