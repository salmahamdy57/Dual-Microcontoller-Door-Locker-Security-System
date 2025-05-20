/****************************************************************************************************************************************
 Module      : DC Motor
 Name        : dc_motor.c
 Author      : Salma Hamdy
 Description : Source file for the ATmega32 DC Motor driver
 ****************************************************************************************************************************************/

#include "common_macros.h" /* For GET_BIT Macro */
#include "dc_motor.h"
#include "gpio.h"
#include "pwm.h"


/*
 * Description :
 * 1. Initializes the DC motor by setting the direction for the motor pins.
 * 2. Stopping the motor at the beginning.
 */
void DcMotor_Init(void)
{
	/*set motor pin direction*/
	GPIO_setupPinDirection(DC_IN_PORT_ID , DC_IN1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_IN_PORT_ID , DC_IN2_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_ENABLE_PORT_ID, DC_ENABLE_PIN_ID, PIN_OUTPUT);

	/*stop motor at the beginning*/
	GPIO_writePin(DC_IN_PORT_ID , DC_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(DC_IN_PORT_ID , DC_IN2_PIN_ID, LOGIC_LOW);
}

/*
 * Description :
 * Controls the motor's state (Clockwise/Anti-Clockwise/Stop) and adjusts the speed based on the input duty cycle.
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{
	switch(state)
	{
		case Clockwise:
			GPIO_writePin(DC_IN_PORT_ID, DC_IN1_PIN_ID, LOGIC_HIGH);
			GPIO_writePin(DC_IN_PORT_ID, DC_IN2_PIN_ID, LOGIC_LOW);
			PWM_Timer0_Start(speed);
			break;

		case Anti_Clockwise:
			GPIO_writePin(DC_IN_PORT_ID, DC_IN1_PIN_ID, LOGIC_LOW);
			GPIO_writePin(DC_IN_PORT_ID, DC_IN2_PIN_ID, LOGIC_HIGH);
			PWM_Timer0_Start(speed);
			break;

		case Stop:
			GPIO_writePin(DC_IN_PORT_ID, DC_IN1_PIN_ID, LOGIC_LOW);
			GPIO_writePin(DC_IN_PORT_ID, DC_IN2_PIN_ID, LOGIC_LOW);
			PWM_Timer0_Start(speed);
			break;
	}

}
