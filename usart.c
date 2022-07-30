/**
 * @file usart.c
 * @brief Basic serial communication via USART for ATMEGA328
 * @version 0.1
 * @date 26.06.2022
 */

#include "usart.h"

unsigned char usart_receive_byte(void) {
    while (!usart_can_receive()) {
        // Wait for data in buffer
    }
    return UB_DATA;
}


uint8_t usart_can_receive() {
    return UB_STATUS & (1 << UB_STATUS_REC_COMPLETE);
}

void usart_transmit_byte(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UB_STATUS & (1 << UB_STATUS_EMPTY_FLAG))) {
        // Busy waiting! zzzZZZzzzZZZzzz
    }
    // Put data into buffer, send the data
    UB_DATA = data;
}

void usart_print(const char *c) {
    while (*c != '\0') {
        usart_transmit_byte(*c);
        c++;
    }
}

void usart_init(unsigned long ubrr) {
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
    usart_print("<(^_^)>\n\0");
}