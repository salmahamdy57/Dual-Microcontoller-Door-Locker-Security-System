/***********************************************************************************************************************************
 Module      : UART
 Name        : uart.h
 Author      : Salma Hamdy
 Description : Header file for the ATmega32 UART driver
 ************************************************************************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

typedef uint8 UART_BitDataType;
typedef uint8 UART_ParityType;
typedef uint8 UART_StopBitType;
typedef uint32 UART_BaudRateType;

typedef struct{
    UART_BitDataType bit_data;
    UART_ParityType parity;
    UART_StopBitType stop_bit;
    UART_BaudRateType baud_rate;
}UART_ConfigType;

/* UART Control and Status Register C (UCSRC)*/

/* Choose number of data data bits in frame (UCSZ1:0) */
#define UART_5_BIT_DATA_MODE               0
#define UART_6_BIT_DATA_MODE               1
#define UART_7_BIT_DATA_MODE               2
#define UART_8_BIT_DATA_MODE               3

/* Choose Parity Mode (UPM1:0) */
#define UART_PARITY_DISABLED               0
#define UART_PARITY_ENABLED_EVEN           2
#define UART_PARITY_ENABLED_ODD            3

/* Choose Stop Bit (USBS) */
#define UART_1_STOP_BIT                    0
#define UART_2_STOP_BIT                    1

#define DEFAULT_UART_CONFIG {UART_8_BIT_DATA_MODE, UART_PARITY_DISABLED, UART_1_STOP_BIT, 9600}



/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
