
/* File:   i2c7.c     
 * Author: Craig Bacon
 * Created on 14 Jan 2018, 10:25
 * Uses I2C to communicate with Sparkfun's Sensor array (slave)
 * PIC(master) reads RegdataA on the sensor array which stores values of the 
 * 8 IR sensors then displays on PICs 4 LEDs, only middle 4 IR sensors displayed
 * Sensor array slave Address is 0x3E (0b00111110)
 * 7 bit value used so write =  7 bit address + write = 0x7C ( 0b01111100)
 * read = 7 bit address + read = 0x7D ( 0b01111101)
 */


// READS LINE DATA as in handles where the sensor detects the lines



#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "I2C.h"
#pragma config OSC = HS    	//High speed resonator
#pragma config WDT = OFF  	 //Watchdog timer off
#pragma config LVP = OFF   	//Low voltage programming disabled
#pragma config PWRT = ON   	//Power up timer on


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




void I2C_INIT(void){
  
    //Set up Sensor data ------    	 
    TRISC = 0xFF;                	 //Set PORTC as inputs
    TRISB = 0x00;                 	//Set PORTB as outputs
    LATB = 0x00;                  	//Turn All LEDs off
    I2C_Initialise();             	//Initialise I2C Master 
    // -------------

}



















void UpdateLineData(void){
    // This function will update the data in "linesensor" via specified methods
    
    I2C_Start();                	//Send Start condition to slave
    I2C_Write(0x7C);            	//Send 7 bit address + Write to slave
    I2C_Write(0x11);            	//Write data, select RegdataA and send to slave
    I2C_RepeatedStart();        	//Send repeat start condition
    I2C_Write(0x7D);            	//Send 7 bit address + Read
    linesensor=I2C_Read();      	//Read  the IR sensors 
    LATB=linesensor;            	//Output to LEDs
    I2C_Stop();                 	//Send Stop condition
}





// PROBABLY DONT NEED TO USE THESE ANYMORE----------
void I2C_Initialise(void)  	//Initialise I2C
{
  SSPCON1 = 0b00101000; 	//set to master mode, enable SDA and SCL pins
  SSPCON2 = 0;         		 //reset control register 2
  SSPADD = 0x63;       		 //set baud rate to 100KHz
  SSPSTAT = 0;         		 //reset status register
  }
void I2C_checkbus_free(void)   	 //Wait until I2C bus is free
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));	//wait until I2C bus is free
}

void I2C_Start(void)        //Generate I2C start condition
{
  I2C_checkbus_free(); 	 //Test to see I2C bus is free
  SEN = 1;              	//Generate start condition,SSPCON2 bit 0 = 1
}

void I2C_RepeatedStart(void) 	//Generate I2C Repeat start condition
{
  I2C_checkbus_free();  		//Test to see I2C bus is free
  RSEN = 1;             		//Generate repeat start, SSPCON2 bit1 = 1
}

void I2C_Stop(void) 		//Generate I2C stop condition
{
  I2C_checkbus_free();  		//Test to see I2C bus is free
  PEN = 1;              		// Generate stop condition,SSPCON2 bit2 = 1
}

void I2C_Write(unsigned char write) 	//Write to slave
{
  I2C_checkbus_free();  		//check I2C bus is free
  SSPBUF = write;       		//Send data to transmit buffer
}

unsigned char I2C_Read(void)    //Read from slave
{
  unsigned char temp;
  I2C_checkbus_free(); 	 //Test to see I2C bus is free
  RCEN = 1;            	 //enable receiver,SSPCON2 bit3 = 1
  I2C_checkbus_free();  	//Test to see I2C bus is free
  temp = SSPBUF;        	//Read slave
  I2C_checkbus_free(); 	 //Test to see I2C bus is free
  ACKEN = 1;           	 //Acknowledge
  return temp;         	 //return sensor array data
}

