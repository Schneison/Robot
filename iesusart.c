/**
 * @file iesusart.c
 * @brief Basic serial communication via USART for ATMEGA328
 * @version 0.1
 * @date 26.06.2022
 */

#include "iesusart.h"

unsigned char USART_receiveByte(void) {
    while (!USART_canReceive()) {}
    return UDR0;
}

void USART_receiveString(unsigned char *buffer, uint8_t max_len) {
    unsigned char c_char;
    uint8_t len = 0;

    c_char = USART_receiveByte();

    while (c_char != '\n' && len < max_len - 1) {
        *buffer++ = c_char;
        len++;
        c_char = USART_receiveByte();
    }

    *buffer = '\0';
}


uint8_t USART_canReceive() {
    return UCSR0A & (1 << RXC0);
}

void USART_transmitByte(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0))) {
        // Busy waiting! zzzZZZzzzZZZzzz
    }
    // Put data into buffer, send the data
    UDR0 = data;
}

void USART_print(const char *c) {
    while (*c != '\0') {
        USART_transmitByte(*c);
        c++;
    }
}

void USART_init(unsigned long ubrr) {
    // Set baud rate, high byte first
    UBRR0H = (unsigned char) (ubrr >> 8);
    // Set baud rate, low byte second
    UBRR0L = (unsigned char) ubrr;
    // Enable receiver/transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Frame format: 8 data, 2 stop bits
    UCSR0C = (1 << USBS0) | (3 << UCSZ00);
    /* Transmit something right after initialization to overcome the lagg at the
     * start of a simulation in SimulIDE.
    */
    USART_print("<(^_^)>\n\0");
}