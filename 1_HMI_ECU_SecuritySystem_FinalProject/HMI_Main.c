/************************************************************************************************************************************
 Module      : HMI ECU Main
 Name        : HMI_main.c
 Author      : Salma Hamdy
 Description : Dual Microcontroller-Based Door Locker Security System Using Password Authentication (Human Interface Microcontroller)
 ***********************************************************************************************************************************/

#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer.h"
#include <util/delay.h>
#include "common_macros.h"
#include "std_types.h"
#include <avr/io.h>

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
 *                         Function Prototype                                  *
 *******************************************************************************/
void getPassword(uint8 * password, uint8 passwordSize);
void createPassword(void);
uint8 checkPassword(void);
void timerCallBack(void);
void alarmSystem(void);

/*******************************************************************************
 *                         Global Variables                                    *
 *******************************************************************************/
volatile uint8 tick = 0;
volatile boolean flag_t_15s = FALSE;
volatile boolean flag_t_60s = FALSE;
volatile boolean flag_alarm = FALSE;

/* Create configuration structure for timer driver
   Description:
   - initial value = 0
   - compare value = 39062, so the interrupt occurs every 5 seconds
   - Timer 1
   - pre-scaler 1024
   - compare mode
*/
Timer_ConfigType timerConfig = {0,39062,TIMER_1,F_CPU_1024,COMPARE_MODE};

/*******************************************************************************
 *                                Main                                         *
 *******************************************************************************/
int main(void)
{
	uint8 pressedKey;
	uint8 checkPasswordState;

	/* Create configuration structure for UART driver
	   Description:
	   - 8-bit data
	   - parity disabled
	   - one stop bit
	   - Baud-rate = 9600 bits/s
	 */
	UART_ConfigType uartConfig = {UART_8_BIT_DATA_MODE, UART_PARITY_DISABLED, UART_1_STOP_BIT, 9600};

	/* Enable Global Interrupt I-Bit */
	SET_BIT(SREG,7);

	UART_init(&uartConfig);
	LCD_init();

	/* System start, create new password */
	createPassword();

	LCD_clearScreen();

	while(1)
	{
		/* Main Menu options, always on display */
		LCD_displayStringRowColumn(0,0,"+ : Open Door");
		LCD_displayStringRowColumn(1,0,"- : Change Pass");

		/* Get desired action from user */
		pressedKey = KEYPAD_getPressedKey();
		_delay_ms(500);

		/* If user chooses +: open door */
		if(pressedKey == '+')
		{
			/* User should enter saved system password */
			checkPasswordState = checkPassword();

			if(checkPasswordState == TRUE_PASSWORD)
			{
				/******************************************* To Unlock The Door ***********************************************/

				/* send a signal to the Control ECU to unlock the door */
				UART_sendByte(UNLOCK_DOOR);

				/* Set call back function pointer in timer driver */
				Timer_setCallBack(timerCallBack, TIMER_1);
				/* Initialize timer driver */
				Timer_init(&timerConfig);

				/* Display "Door unlocking please wait" message on screen for 15s */
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Door unlocking");
				LCD_displayStringRowColumn(1,1,"Please wait");
				/* wait 15s */
				while (flag_t_15s != TRUE);
				/* reset the flag */
				flag_t_15s = FALSE;

				/************************************************* Door Open ***************************************************/

				/* display "wait for people to enter" message on screen until a signal is received to lock the door */
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Wait for people");
				LCD_displayStringRowColumn(1,2,"to enter");

				/******************************************** To Lock The Door *************************************************/

				/* wait until Control ECU sends LOCKING_DOOR */
				while (UART_recieveByte() != LOCKING_DOOR);

				/* Set the Call back function pointer in the timer driver */
				Timer_setCallBack(timerCallBack, TIMER_1);
				/*Initialize timer driver*/
				Timer_init(&timerConfig);

				/* Display "Door locking" message on screen for 15s */
				LCD_clearScreen();
				LCD_displayStringRowColumn(0,0,"Door locking");
				/* wait 15s  */
				while (flag_t_15s != TRUE);
				/* reset the flag */
				flag_t_15s = FALSE;

				LCD_clearScreen();
			}
			/* Turn alarm system on */
			else if(checkPasswordState == FALSE_PASSWORD)
			{
				alarmSystem();
			}

		}
		/* If user chooses -: change password */
		else if(pressedKey == '-')
		{
			/* User should enter saved system password */
			checkPasswordState = checkPassword();

			if(checkPasswordState == TRUE_PASSWORD)
			{
				/* Send CHANGE_PASSWORD to Control ECU to save the new password */
				UART_sendByte(CHANGE_PASSWORD);

				/* Create new password */
				createPassword();

				LCD_clearScreen();
			}
			/*turn alarm system on*/
			else if(checkPasswordState == FALSE_PASSWORD)
			{
				alarmSystem();
			}
		}

	}

	return 0;
}

/* Function that gets the password from the user */
void getPassword(uint8 * password, uint8 passwordSize)
{
	uint8 i;
	for(i = 0; i < passwordSize-2; i++)
	{
		password[i] = KEYPAD_getPressedKey()+48;
		LCD_displayCharacter('*');
		_delay_ms(500);
	}

	/* End password with # and null */
	password[i++] = '#';
	password[i] = '\0';
}

/* Function that creates a new password by making the user enter the password twice and sends it to the Control ECU via the UART */
void createPassword(void)
{
	uint8 pass_size = PASSWORD_SIZE+2;
	uint8 password_1[pass_size], password_2[pass_size];
	uint8 savedPassword;

	while(1)
	{
		/* user should enter password for first time */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Plz enter pass: ");
		LCD_moveCursor(1,0);
		getPassword(password_1,pass_size);

		/*check for user to press enter */
		while(KEYPAD_getPressedKey() != '=');
		_delay_ms(500);


		/* user should enter password for the second time */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Plz re-enter the");
		LCD_displayStringRowColumn(1,0,"same pass: ");
		getPassword(password_2,pass_size);

		/*check for user to press enter*/
		while(KEYPAD_getPressedKey() != '=');
		_delay_ms(500);

		/* Wait until Control ECU is ready to receive the string */
		while(UART_recieveByte() != CONTROL_ECU_READY);

		/* send the 2 passwords to the Control ECU via UART */
		UART_sendString(password_1);
		UART_sendString(password_2);

		/* UART will send the agreed upon signal if the 2 passwords match */
		savedPassword = UART_recieveByte();

		/* if the 2 passwords are a match, this becomes the system password*/
		if(savedPassword == PASSWORDS_MATCH)
		{
			return;
		}
	}
}

/* Function the checks if user entered the correct saved system password */
uint8 checkPassword(void)
{
	uint8 pass_size = PASSWORD_SIZE+2;
	uint8 password[pass_size];
	uint8 i, passwordCheck;

	/* user has 3 chances to enter the correct password */
	for(i = 0 ; i < 3 ; i++){

		/* prompt user to enter the password to unlock the system */
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,"Plz enter old");
		LCD_displayStringRowColumn(1,0,"pass: ");

		/* get entered password from user */
		getPassword(password,pass_size);

		/*check for user to press enter */
		while(KEYPAD_getPressedKey() != '=');
		_delay_ms(500);

		/* Wait until Control ECU is ready to receive the string */
		while(UART_recieveByte() != CONTROL_ECU_READY);

		/* send entered password to the Control ECU */
		UART_sendString(password);

		/* check if entered password matches the save password in the EEPROM */
		passwordCheck = UART_recieveByte();

		/* if entered password and saved password are a match, return TRUE_PASSWORD */
		if(passwordCheck == PASSWORDS_MATCH)
		{
			return TRUE_PASSWORD;
		}
	}
	/* if entered password and saved password are NOT a match, return FALSE_PASSWORD */
	return FALSE_PASSWORD;
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

/* Function to activate alarm system */
void alarmSystem(void)
{
	flag_alarm = TRUE;

	/* Set call back function pointer in timer driver */
	Timer_setCallBack(timerCallBack, TIMER_1);
	/* Initialize timer driver */
	Timer_init(&timerConfig);

	/* Display "Door unlocking please wait" message on screen for 15s */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,1,"SYSTEM LOCKED");
	LCD_displayStringRowColumn(1,1,"Wait for 1 min");

	/* wait 1min */
	while (flag_t_60s != TRUE);

	/* reset the flags */
	flag_t_15s = FALSE;
	flag_alarm = FALSE;

	LCD_clearScreen();
}





