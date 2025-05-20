/***********************************************************************************************************************************
 Module      : Timer
 Name        : timer.c
 Author      : Salma Hamdy
 Description : Source file for the ATmega32 Timer driver
 ************************************************************************************************************************************/

#include "timer.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include <avr/io.h> /* To use Timer Registers */
#include <avr/interrupt.h> /* For Timer ISR */

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_timer0CallBackPtr)(void) = NULL_PTR;
static volatile void (*g_timer1CallBackPtr)(void) = NULL_PTR;
static volatile void (*g_timer2CallBackPtr)(void) = NULL_PTR;


/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER0_OVF_vect)
{
	if (g_timer0CallBackPtr != NULL_PTR)
	{
		(*g_timer0CallBackPtr)();
	}
}

ISR(TIMER0_COMP_vect)
{
	if (g_timer0CallBackPtr != NULL_PTR)
	{
		(*g_timer0CallBackPtr)();
	}
}

ISR(TIMER1_OVF_vect)
{
	if (g_timer1CallBackPtr != NULL_PTR)
	{
		(*g_timer1CallBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if (g_timer1CallBackPtr != NULL_PTR)
	{
		(*g_timer1CallBackPtr)();
	}
}

ISR(TIMER2_OVF_vect)
{
	if (g_timer2CallBackPtr != NULL_PTR)
	{
		(*g_timer2CallBackPtr)();
	}
}

ISR(TIMER2_COMP_vect)
{
	if (g_timer2CallBackPtr != NULL_PTR)
	{
		(*g_timer2CallBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Function to initialize the Timer driver.
 * 1. Set Timer initial value.
 * 2. Set Timer compare value in case of compare mode.
 * 3. Select required pre-scaler
 * 4. Enable Overflow Interrupt or Compare Match Interrupt
 */
void Timer_init(const Timer_ConfigType * Config_Ptr)
{
	switch (Config_Ptr->timer_ID) {

	case TIMER_0:


		/* Set Initial Value */
		TCNT0 = Config_Ptr -> timer_InitialValue;

		if(Config_Ptr -> timer_mode == COMPARE_MODE)
		{
			/* Set Compare Value */
			OCR0 = Config_Ptr -> timer_compare_MatchValue;
			/* Enable Timer0 Compare Match Interrupt */
			SET_BIT(TIMSK,OCIE0);
		}
		else  /* Normal Mode */
		{
			/* Enable Timer0 Overflow Interrupt */
			SET_BIT(TIMSK,TOIE0);
		}

		/* Non PWM mode FOC0=1 */
		SET_BIT(TCCR0,FOC0);

        /* Select wave generation mode
         * Normal mode: WGM01=0, WGM00=0
         * CTC mode:    WGM01=1, WGM00=0
         */
		CLEAR_BIT(TCCR0,WGM00);
		TCCR0 = (TCCR0 & ~(1 << WGM01)) | (GET_BIT(Config_Ptr->timer_mode, 1) << WGM01);

		/* Normal port operation, OC0 disconnected, COM00=0 & COM01=0 */
		CLEAR_BIT(TCCR0,COM00);
		CLEAR_BIT(TCCR0,COM01);

		/* Select clock type */
		TCCR0 = (TCCR0 & ~0x07) | (Config_Ptr->timer_clock & 0x07);

		break;


	case TIMER_1:

		/* Set Initial Value */
		TCNT1 = Config_Ptr -> timer_InitialValue;

		if(Config_Ptr -> timer_mode == COMPARE_MODE)
		{
			/* Set Compare Value */
			OCR1A = Config_Ptr -> timer_compare_MatchValue;
			/* Enable Timer1 Compare Match Interrupt */
			SET_BIT(TIMSK,OCIE1A);
		}
		else  /* Normal Mode */
		{
			/* Enable Timer1 Overflow Interrupt */
			SET_BIT(TIMSK,TOIE1);
		}

		/* Non PWM mode FOC1A=1 */
		SET_BIT(TCCR1A,FOC1A);

        /* Select the wave generation mode
         * Normal mode: WGM10=0, WGM11=0, WGM12=0, WGM13=0
         * CTC mode:    WGM10=0, WGM11=0, WGM12=1, WGM13=0
         */
		TCCR1A &= ~((1 << WGM10) | (1 << WGM11));
		TCCR1B = (TCCR1B & ~((1 << WGM12) | (1 << WGM13))) | (GET_BIT(Config_Ptr->timer_mode, 1) << WGM12);

		/* Normal port operation, OC1 disconnected, COM1A0=0 & COM1A1=0 */
		CLEAR_BIT(TCCR1A,COM1A0);
		CLEAR_BIT(TCCR1A,COM1A1);

		/* Select clock type */
		TCCR1B = (TCCR1B & ~0x07) | (Config_Ptr->timer_clock & 0x07);

		break;

	case TIMER_2:

		/* Set Initial Value */
		TCNT2 = Config_Ptr -> timer_InitialValue;

		if(Config_Ptr -> timer_mode == COMPARE_MODE)
		{
			/* Set Compare Value */
			OCR2 = Config_Ptr -> timer_compare_MatchValue;
			/* Enable Timer0 Compare Match Interrupt */
			SET_BIT(TIMSK,OCIE2);
		}
		else  /* Normal Mode */
		{
			/* Enable Timer0 Overflow Interrupt */
			SET_BIT(TIMSK,TOIE2);
		}

		/* Non PWM mode FOC0=1 */
		SET_BIT(TCCR2,FOC2);

        /* Select wave generation mode
         * Normal mode: WGM21=0, WGM20=0
         * CTC mode:    WGM21=1, WGM20=0
         */
		CLEAR_BIT(TCCR2,WGM20);
		TCCR2 = (TCCR2 & ~(1 << WGM21)) | (GET_BIT(Config_Ptr->timer_mode, 1) << WGM21);

		/* Normal port operation, OC2 disconnected, COM20=0 & COM21=0 */
		CLEAR_BIT(TCCR2,COM20);
		CLEAR_BIT(TCCR2,COM21);

		/* Select clock type */
		TCCR2 = (TCCR2 & ~0x07) | (Config_Ptr->timer_clock & 0x07);

		break;
	}
}

/*
 * Description :
 * Function to disable the Timer via Timer_ID.
 */
void Timer_deInit(Timer_ID_Type timer_type)
{
	switch (timer_type)
	{
		case TIMER_0:

			/* Clear All Timer0 Registers */
			TCCR0 = 0;
			TCNT0 = 0;
			OCR0 = 0;

			/* Disable the interrupt */
			CLEAR_BIT(TIMSK,OCIE0);
			CLEAR_BIT(TIMSK,TOIE0);

			/* Reset the global pointer value */
			g_timer0CallBackPtr = NULL_PTR;
			break;

		case TIMER_1:

			/* Clear All Timer1 Registers */
			TCCR1A = 0;
			TCCR1B = 0;
			TCNT1 = 0;
			OCR1A = 0;

			/* Disable the interrupt */
			CLEAR_BIT(TIMSK,OCIE1A);
			CLEAR_BIT(TIMSK,TOIE1);

			/* Reset the global pointer value */
			g_timer1CallBackPtr = NULL_PTR;

			break;

		case TIMER_2:

			/* Clear All Timer2 Registers */
			TCCR2 = 0;
			TCNT2 = 0;
			OCR2 = 0;

			/* Disable the interrupt */
			CLEAR_BIT(TIMSK,OCIE2);
			CLEAR_BIT(TIMSK,TOIE2);

			/* Reset the global pointer value */
			g_timer2CallBackPtr = NULL_PTR;
			break;
	}
}

/*
 * Description :
 * Function to set the Call Back function address to the required Timer.
 */
void Timer_setCallBack(void(*a_ptr)(void), Timer_ID_Type a_timer_ID )
{
	switch (a_timer_ID)
	{
		case TIMER_0:
			g_timer0CallBackPtr = a_ptr;
			break;

		case TIMER_1:
			g_timer1CallBackPtr = a_ptr;
			break;

		case TIMER_2:
			g_timer2CallBackPtr = a_ptr;
			break;
	}
}


