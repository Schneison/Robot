#include "led_control.h"

// Robot function/peripheral RIGHT LF.
#define DR_LF_R DDRC
#define DP_LF_R DDC0
#define IR_LF_R PINC
#define IP_LF_R PINC0

// Robot function/peripheral MIDDLE LF.
#define DR_LF_M DDRC
#define DP_LF_M DDC1
#define IR_LF_M PINC
#define IP_LF_M PINC1

// Robot function/peripheral LEFT LF.
#define DR_LF_L DDRC
#define DP_LF_L DDC2
#define IR_LF_L PINC
#define IP_LF_L PINC2

// Robot funktion/peripheral SR
#define REGWIDTH 3

// SR clock
#define DR_SR_CLK  DDRD
#define DP_SR_CLK  DDD4
#define OR_SR_CLK  PORTD
#define OP_SR_CLK  PORTD4

// SR data
#define DR_SR_DATA DDRB
#define DP_SR_DATA DDB2
#define OR_SR_DATA PORTB
#define OP_SR_DATA PORTB2

void led_init(void) {
    DR_SR_DATA |= 1 << DP_SR_DATA;

    DR_SR_CLK |= 1 << DP_SR_CLK;
}

void led_clock(void) {
    PORTD &= ~(1 << LED_CLOCK);
    PORTD |= (1 << LED_CLOCK);
    PORTD &= ~(1 << LED_CLOCK);
}

void led_set(LED_State state) {
    // Updates LED positions from left to right
    for (int i = LED_AMOUNT - 1; i >= 0; i--) {
        if ((state >> i) & 1) {
            PORTB |= (1 << LED);
        } else {
            PORTB &= ~(1 << LED);
        }
        led_clock();
    }
}

void led_chase(LED_State *lastState) {
    int state = *lastState;
    // From right to left
    if ((state & CHASE_FLAG) == CHASE_FLAG) {
        *lastState = ((state & LED_ALL) << 1) | (state & LED_LEFT ? 0 : CHASE_FLAG);
    } else {
        *lastState = ((state & LED_ALL) >> 1) | (state & LED_RIGHT ? CHASE_FLAG : 0);
    }
    led_set(*lastState);
}

void led_blink(LED_State *lastState) {
    if (*lastState) {
        *lastState = LED_NONE;
    } else {
        *lastState = LED_ALL;
    }
    led_set(*lastState);
}

void led_sensor(sensor_state sensorState) {
    LED_State ledState = LED_NONE;
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
