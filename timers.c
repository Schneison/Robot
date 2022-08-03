#include "timers.h"

uint16_t millis = 0;

/**
 * @brief Updates counter variables
 *
 * Called after the millis timer reaches the compare value
 */
ISR (TIMER1_COMPA_vect) {
    millis++;
}

void timers_create(counter *counters) {
    for (int i = 0; i < COUNTER_AMOUNT; i++) {
        counter *counter = &counters[i];
        counter->value = 0;
        counter->threshold = counter_frequencies[i];
        counter->last_millis = millis;
    }
}

void timers_update(counter *counters) {
    for (int i = 0; i < COUNTER_AMOUNT; i++) {
        counter *counter = &counters[i];
        // Diff since last true cycle
        uint16_t delta = millis - counter->last_millis;
        if (delta > counter->threshold) {
            counter->last_millis = millis;
            counter->value = 1;
        } else {
            counter->value = 0;
        }
    }
}

uint8_t timers_check_state(const track_state *state, counter_def counterDef) {
    return timers_check(state->counters, counterDef);
}

uint8_t timers_check(const counter *counters, counter_def counterDef) {
    return counters && counters[counterDef].value;
}

void timers_print(const counter *counters, counter_def frequency, const char *text) {
    if (timers_check(counters, frequency)) {
        usart_print(text);
    }
}

void timers_init(void) {
    timers_setup_timer_0();
    timers_setup_timer_1();
}

// timer0
void timers_setup_timer_0(void) {
    // Disable all interrupts
    cli();
    TIMER_0_CONTROL = 0;
    TIMER_0_CONTROL |= TIMER_0_PRE_SCALE;
    TIMER_0_WAVE = 0;
    TIMER_0_WAVE |= TIMER_0_WAVE_MODE;
    // Re-enable all interrupts
    sei();
}

// timer1
void timers_setup_timer_1(void) {
    // Disable all interrupts
    cli();
    TIMER_1_CONTROL = 0;
    TIMER_1_CONTROL |= TIMER_1_PRE_SCALE;
    TIMER_1_CONTROL |= (1 << TIMER_1_MODE);
    TIMER_1_INTERRUPT |= (1 << TIMER_1_COMPARE_MODE);
    TIMER_1_COMPARE_RESOLUTION = TIMER_1_COMPARE_VALUE;
    // Re-enable all interrupts
    sei();
}
