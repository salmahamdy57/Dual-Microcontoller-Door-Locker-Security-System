/***********************************************************************************************************************************
 Module      : PIR Sensor
 Name        : pir_sensor.h
 Author      : Salma Hamdy
 Description : Header file for the ATmega32 PIR driver
 ************************************************************************************************************************************/

#ifndef PIR_SENSOR_H_
#define PIR_SENSOR_H_


#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* PIR HW Ports and Pins Ids */
#define PIR_PORT_ID              PORTC_ID
#define PIR_PIN_ID               PIN2_ID


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function to initialize the PIR driver.
 */
void PIR_init(void);

/*
 * Description :
 * Function to return PIR State
 */
uint8 PIR_getState(void);

#endif /* PIR_SENSOR_H_ */
