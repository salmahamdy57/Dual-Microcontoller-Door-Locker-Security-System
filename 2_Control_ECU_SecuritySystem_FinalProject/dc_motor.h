/****************************************************************************************************************************************
 Module      : DC Motor
 Name        : dc_motor.h
 Author      : Salma Hamdy
 Description : Header file for the ATmega32 DC Motor driver
 ****************************************************************************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* Enumeration for the DC Motor states */
typedef enum {
	Clockwise,
    Anti_Clockwise,
    Stop
} DcMotor_State;


/* DC HW Ports and Pins Ids */
#define DC_ENABLE_PORT_ID              PORTB_ID
#define DC_ENABLE_PIN_ID               PIN3_ID

#define DC_IN_PORT_ID                  PORTD_ID
#define DC_IN1_PIN_ID                  PIN6_ID
#define DC_IN2_PIN_ID                  PIN7_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * 1. Initializes the DC motor by setting the direction for the motor pins.
 * 2. Stopping the motor at the beginning.
 */
void DcMotor_Init(void);

/*
 * Description :
 * Controls the motor's state (Clockwise/Anti-Clockwise/Stop) and adjusts the speed based on the input duty cycle.
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed);




#endif /* DC_MOTOR_H_ */
