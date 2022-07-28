/**
 * @file usart.c
 * @brief Basic serial communication via USART for ATMEGA328
 * @version 0.1
 * @date 26.06.2022
 */

#include "usart.h"

unsigned char USART_receiveByte(void) {
    while (!USART_canReceive()) {
        // Wait for data in buffer
    }
    return UB_DATA;
}


uint8_t USART_canReceive() {
    return UB_STATUS & (1 << UB_STATUS_REC_COMPLETE);
}

void USART_transmitByte(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UB_STATUS & (1 << UB_STATUS_EMPTY_FLAG))) {
        // Busy waiting! zzzZZZzzzZZZzzz
    }
    // Put data into buffer, send the data
    UB_DATA = data;
}

void USART_print(const char *c) {
    while (*c != '\0') {
        USART_transmitByte(*c);
        c++;
    }
}

void USART_init(unsigned long ubrr) {
    // Set baud rate, high byte first
    UB_BAUD_RATE_HIGH = (unsigned char) (ubrr >> 8);
    // Set baud rate, low byte second
    UB_BAUD_RATE_LOW = (unsigned char) ubrr;
    // Enable receiver/transmitter
    UB_RE_TR = (1 << UB_RECEIVER_FLAG) | (1 << UB_TRANSMITTER_FLAG);
    // Frame format: 8 data, 2 stop bits
    UB_FORMAT = (1 << UB_FORMAT_8_DATA) | (3 << UB_FORMAT_2_STOP_BITS);
    /* Transmit something right after initialization to overcome the lagg at the
     * start of a simulation in SimulIDE.
    */
    USART_print("<(^_^)>\n\0");
}