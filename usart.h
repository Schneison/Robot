/**
 * @file
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
 * @brief Usart Status Registry
 */
#define UB_STATUS UCSR0A
/**
 * @brief USART receive complete
 */
#define UB_STATUS_REC_COMPLETE RXC0
/**
 * @brief USART transmit buffer empty
 */
#define UB_STATUS_EMPTY_FLAG UDRE0
/**
 * @brief USART Data Registry
 */
#define UB_DATA UDR0
/**
 * @brief USART Baud Rate High Registry
 */
#define UB_BAUD_RATE_HIGH UBRR0H
/**
 * @brief USART Baud Rate Low Registry
 */
#define UB_BAUD_RATE_LOW UBRR0L
/**
 * @brief USART Transmit/Receive Registry Registry
 */
#define UB_RE_TR UCSR0B
/**
 * @brief Enables Receiver
 */
#define UB_RECEIVER_FLAG RXEN0
/**
 * @brief Enables Transmitter
 */
#define UB_TRANSMITTER_FLAG TXEN0
/**
 * @brief USART Data Format Registry
 */
#define UB_FORMAT UCSR0C
/**
 * @brief 8 Bit Data Format
 */
#define UB_FORMAT_8_DATA USBS0
/**
 * @brief 2 Stop Bit Data Format
 */
#define UB_FORMAT_2_STOP_BITS UCSZ00

/**
 * @brief Reads a byte from the receive buffer.
 * @return received byte
 */
unsigned char usart_receive_byte(void);

/**
 * @brief Checks if there is any data to be read
 * @return
 */
uint8_t usart_can_receive();

/**
 * @brief Writes a byte to the transmit buffer
 * @param data Byte that shall be transmitted
 */
void usart_transmit_byte(unsigned char data);

/**
 * @brief Transmitters a string (char by char) until '\0’ is reached
 */
void usart_print(const char *c);

/**
 * @brief Sets up the USART port (The USART baudrate register)
 * @param ubrr Content to write into the UBRR register
 */
void usart_init(unsigned long ubrr);

#endif