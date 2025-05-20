/***********************************************************************************************************************************
 Module      : PIR Sensor
 Name        : pir_sensor.c
 Author      : Salma Hamdy
 Description : Source file for the ATmega32 PIR driver
 ************************************************************************************************************************************/

#include "pir_sensor.h"
#include "gpio.h"

/*
 * Description :
 * Function to initialize the PIR driver.
 */
void PIR_init(void)
{
	/*set PIR pin direction*/
	GPIO_setupPinDirection(PIR_PORT_ID, PIR_PIN_ID, PIN_INPUT);
}

/*
 * Description :
 * Function to return PIR State
 */
uint8 PIR_getState(void)
{
	return GPIO_readPin(PIR_PORT_ID,PIR_PIN_ID);
}
