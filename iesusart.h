/**
 * @file iesusart.h
 * @brief Basic serial communication via USART for ATMEGA328
 * @version 0.1
 * @date 2021-06-08
 */

#ifndef IESUSART_h
#define IESUSART_h

#include <avr/io.h>

/// CPU clock speed
#ifndef F_CPU
#define F_CPU 16E6
#endif
/// Desired baudrate
#define BAUD 9600
/// What to write into the UBRR register
#define UBRR_SETTING F_CPU/16.0/BAUD-1

/**
 * @brief 
 * Reads a single byte out of the USART receive buffer.
 * A good way to use this would be calling it from an receive interrupt serice routine
 * @return received byte
 */
unsigned char USART_receiveByte(void);

/**
 * @brief
 * Reads a string of characters of the USART receive buffer.
 *
 * @param buffer A pointer to the location where the string should be stored
 * @param max_len The maximal length of the read string
 */
void USART_receiveString(unsigned char *buffer, uint8_t max_len);

/**
 * @brief
 * Checks if there is any data to be read
 * @return
 */
uint8_t USART_canReceive();

/**
 * @brief Writes a single byte to the USART transmit buffer
 * @param data Byte that shall be transmitted
 */
void USART_transmitByte(unsigned char data);


/**
 * @brief Transmitters a string (char by char) until '\0’ is reached
 */
void USART_print(const char *c);

/**
 * @brief Sets up the USART port (The USART baudrate register)
 * @param ubrr Content to write into the UBRR register
 */
void USART_init(unsigned long ubrr);

#endif