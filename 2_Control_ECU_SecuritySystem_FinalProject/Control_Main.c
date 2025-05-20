/************************************************************************************************************************************
 Module      : Control ECU Main
 Name        : Control_Main.c
 Author      : Salma Hamdy
 Description : Dual Microcontroller-Based Door Locker Security System Using Password Authentication (Control Microcontroller)
 ***********************************************************************************************************************************/


#include "uart.h"
#include "timer.h"
#include "buzzer.h"
#include "dc_motor.h"
#include "external_eeprom.h"
#include "pir_sensor.h"
#include "twi.h"
#include <util/delay.h>
#include "common_macros.h"
#include "std_types.h"
#include <avr/io.h>
#include "string.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASSWORD_SIZE                 5
#define CONTROL_ECU_READY             0x10
#define PASSWORDS_MATCH               0x11
#define PASSWRDS_NOT_MATCH            0x12
#define TRUE_PASSWORD                 0x13
#define FALSE_PASSWORD                0x14
#define UNLOCK_DOOR                   0x15
#define LOCKING_DOOR                  0x16
#define CHANGE_PASSWORD               0x17

#define TICKS_15S                     3
#define TICKS_60S                     12

/*******************************************************************************
 *                         Global Variables                                    *
 *******************************************************************************/
volatile uint8 tick = 0;
volatile boolean flag_t_15s = FALSE;
volatile boolean flag_t_60s = FALSE;
volatile boolean flag_alarm = FALSE;

/*******************************************************************************
 *                         Function Prototype                                  *
 *******************************************************************************/
void setPassword(void);
void timerCallBack(void);

/*******************************************************************************
 *                                Main                                         *
 *******************************************************************************/


int main(void)
{
	uint8 enteredPassword[PASSWORD_SIZE+1];
	uint8 savedPassword[PASSWORD_SIZE+1];
	uint8 choice = 0;

	/* Create configuration structure for timer driver
	   Description:
	   - initial value = 0
	   - compare value = 39062, so the interrupt occurs every 5 seconds
	   - Timer 1
	   - pre-scaler 1024
	   - compare mode
	 */
	Timer_ConfigType timerConfig = {0,39062,TIMER_1,F_CPU_1024,COMPARE_MODE};

	/* Create configuration structure for UART driver
	   Description:
	   - 8-bit data
	   - parity disabled
	   - one stop bit
	   - Baud-rate = 9600 bits/s
	 */
	UART_ConfigType uartConfig = {UART_8_BIT_DATA_MODE, UART_PARITY_DISABLED, UART_1_STOP_BIT, 9600};

	/* Create configuration structure for TWI/I2C driver
	   Description:
	   - my address = 0x01
	   - bit rate = 0x02, SCL frequency= 400 bits/s
	 */

	/* Enable Global Interrupt I-Bit */
	SET_BIT(SREG,7);

	UART_init(&uartConfig);

	TWI_ConfigType twiConfig = {0x01,0x02};
	TWI_init(&twiConfig);

	Buzzer_init();
	DcMotor_Init();
	PIR_init();

	setPassword();
	_delay_ms(10);

	while(1)
	{
		uint8 i;
		/* User has 3 chances to enter correct password */
		for(i = 0; i < 3; i++)
		{
			/* Send CONTROL_ECU_READY byte to HMI ECU to signal it to send the entered passwords */
			UART_sendByte(CONTROL_ECU_READY);

			/* Receive the entered password from the user in HMI ECU */
			UART_receiveString(enteredPassword);

			/* Get the saved system password from EEPROM */
			EEPROM_readData(0x0311,savedPassword,PASSWORD_SIZE);
			savedPassword[PASSWORD_SIZE] = '\0';

			/* Compare the 2 passwords */
			if(!strcmp((char *)enteredPassword,(char *)savedPassword))
			{
				/* Send a signal to the HMI ECU that the entered password matches the set password */
				UART_sendByte(PASSWORDS_MATCH);

				/* receive byte from HMI ECU to indicate which action is to be taken by the Control ECU */
				choice = UART_recieveByte();
				break;
			}
			else{
				/* Send a signal to the HMI ECU that the entered password does NOT matche the set password */
				UART_sendByte(PASSWRDS_NOT_MATCH);
			}
		}

		/* If user entered wrong password 3 times, activate alarm system*/
		if(i == 3)
		{
			flag_alarm = TRUE;
			/* Set call back function pointer in timer driver */
			Timer_setCallBack(timerCallBack, TIMER_1);
			/* Initialize timer driver */
			Timer_init(&timerConfig);

			/* Turn buzzer on for 1min */
			Buzzer_on();
			/* wait 1min */
			while (flag_t_60s != TRUE);

			/* reset the flags */
			flag_t_60s = FALSE;
			flag_alarm = FALSE;
			/* Turn buzzer off */
			Buzzer_off();
		}

		/* If user entered the correct password, proceed with the choice of the user */
		else
		{
			if(choice == UNLOCK_DOOR)
			{
				/* Set call back function pointer in timer driver */
				Timer_setCallBack(timerCallBack, TIMER_1);
				/* Initialize timer driver */
				Timer_init(&timerConfig);

				/* Rotate motor clockwise for 15s to unlock the door */
				DcMotor_Rotate(Clockwise,50);
				/* wait 15s */
				while (flag_t_15s != TRUE);
				/* reset the flags */
				flag_t_15s = FALSE;

				/* Stop the motor to hold the door open */
				DcMotor_Rotate(Stop,0);
				/* Wait for people to stop entering*/
				while(PIR_getState());

				/* Send LOCKING_DOOR signal to the HMI ECU */
				UART_sendByte(LOCKING_DOOR);

				/* Set call back function pointer in timer driver */
				Timer_setCallBack(timerCallBack, TIMER_1);
				/* Initialize timer driver */
				Timer_init(&timerConfig);

				/* Rotate motor anti-clockwise for 15s to lock the door */
				DcMotor_Rotate(Anti_Clockwise,50);
				/* wait 15s */
				while (flag_t_15s != TRUE);

				/* reset the flags */
				flag_t_15s = FALSE;
				/* Stop the motor to close the door */
				DcMotor_Rotate(Stop,0);

			}
			else if(choice == CHANGE_PASSWORD)
			{
				/* Set new system password */
				setPassword();
				_delay_ms(10);
			}
		}
	}

	return 0;
}

/* Function responsible for setting the system password */
void setPassword(void)
{
	uint8 password_1[PASSWORD_SIZE+1], password_2[PASSWORD_SIZE+1];

	/* Send CONTROL_ECU_READY byte to HMI ECU to signal it to send the two passwords */
	UART_sendByte(CONTROL_ECU_READY);

	/* Receive the 2 passwords from the HMI ECU */
	UART_receiveString(password_1);
	UART_receiveString(password_2);

	/* Compare the 2 passwords */
	if(!strcmp((char *)password_1,(char *)password_2))
	{
		/* If the 2 passwords match, save the password in the EEPROM */
		EEPROM_writeData(0x0311,password_1,PASSWORD_SIZE);

		/* Send a signal to the HMI ECU that the 2 passwords match */
		UART_sendByte(PASSWORDS_MATCH);

		return;
	}
	/* If the 2 passwords do NOT match, send a signal to the HMI ECU that the 2 passwords do NOT match */
	else
	{
		UART_sendByte(PASSWRDS_NOT_MATCH);
	}

}

/* Timer call back function */
void timerCallBack(void){

	tick++;


	/* Check if we should trigger the 15s flag */
	if((!flag_alarm) && (tick == TICKS_15S))
	{
		flag_t_15s = TRUE;
		tick = 0;
	}
	/* Check if we should trigger the 15s flag */
	else if((flag_alarm) && (tick == TICKS_60S))
	{
		flag_t_60s = TRUE;
		tick = 0;
	}

	/* De-initialize the timer after the flag is set */
	if (flag_t_15s || flag_t_60s) {
		Timer_deInit(TIMER_1);
	}
}
