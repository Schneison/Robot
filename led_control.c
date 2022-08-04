#include "led_control.h"

void led_init(void) {
    DR_SR_DATA |= 1 << DP_SR_DATA;

    DR_SR_CLK |= 1 << DP_SR_CLK;
}

void led_clock(void) {
    LED_CLOCK_PORT &= ~(1 << LED_CLOCK);
    LED_CLOCK_PORT |= (1 << LED_CLOCK);
    LED_CLOCK_PORT &= ~(1 << LED_CLOCK);
}

void led_set(led_state state) {
    // Updates LED positions from left to right
    for (int i = LED_AMOUNT - 1; i >= 0; i--) {
        if ((state >> i) & 1) {
            LED_PORT |= (1 << LED_INPUT);
        } else {
            LED_PORT &= ~(1 << LED_INPUT);
        }
        led_clock();
    }
}

void led_chase(led_state *lastState) {
    int state = *lastState;
    if (!state) {
        state = LED_LEFT;
    }
    // From right to left
    if (state & CHASE_FLAG) {
        *lastState =
                ((state & LED_ALL) << 1) & LED_ALL | ((!(state & LED_CENTER)) ? CHASE_FLAG : 0);
    } else {
        *lastState = ((state & LED_ALL) >> 1) & LED_ALL | ((state & LED_CENTER) ? CHASE_FLAG : 0);
    }
    led_set(*lastState);
}

void led_blink(led_state *lastState) {
    if (*lastState) {
        *lastState = LED_NONE;
    } else {
        *lastState = LED_ALL;
    }
    led_set(*lastState);
}

void led_sensor(sensor_state sensorState) {
    led_state ledState = LED_NONE;
    if (sensorState & SENSOR_LEFT) {
        ledState |= LED_LEFT;
    }
    if (sensorState & SENSOR_CENTER) {
        ledState |= LED_CENTER;
    }
    if (sensorState & SENSOR_RIGHT) {
        ledState |= LED_RIGHT;
    }
    led_set(ledState);
}
