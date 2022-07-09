#include "led_control.h"

void LED_clock(void) {
    PORTD &= ~(1 << LED_CLOCK);
    PORTD |= (1 << LED_CLOCK);
    PORTD &= ~(1 << LED_CLOCK);
}

void LED_set(LED_State state) {
    // Updates LED positions from left to right
    for (int i = LED_AMOUNT - 1; i >= 0; i--) {
        if ((state >> i) & 1) {
            PORTB |= (1 << LED);
        }
        else {
            PORTB &= ~(1 << LED);
        }
        LED_clock();
    }
}
