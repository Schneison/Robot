#include "timers.h"

uint16_t millis = 0;

/**
 * @brief Updates counter variables
 * @details Called after the millis timer reaches the compare value
 */
ISR (TIMER1_COMPA_vect) {
    millis++;
}

void timers_create(Counter *counters) {
    for (int i = 0; i < COUNTER_AMOUNT; i++) {
        Counter *counter = &counters[i];
        counter->value = 0;
        counter->threshold = counter_frequencies[i];
        counter->lastMillis = millis;
    }
}

void timers_update(Counter *counters) {
    for (int i = 0; i < COUNTER_AMOUNT; i++) {
        Counter *counter = &counters[i];
        // Diff since last true cycle
        uint16_t delta = millis - counter->lastMillis;
        if (delta > counter->threshold) {
            counter->lastMillis = millis;
            counter->value = 1;
        } else {
            counter->value = 0;
        }
    }
}

uint8_t timers_check_state(track_state *state, counter_def counterDef) {
    return timers_check(state->counters, counterDef);
}

uint8_t timers_check(Counter *counters, counter_def counterDef) {
    return counters && counters[counterDef].value;
}

void timers_print(Counter *counters, counter_def frequency, const char *text) {
    if (timers_check(counters, frequency)) {
        USART_print(text);
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
    // Set prescaler to 64, cf. datasheet for TCCR0B
    // (TCCR0B: Timer/Counter Control Register 0 B)
    TIMER_0_CONTROL = 0;
    TIMER_0_CONTROL |= TIMER_0_PRE_SCALE;
    //TCCR0B = 0;
    //TCCR0B |= (1 << CS00) | (1 << CS01);
    // Set waveform generation mode to Fast PWM, frequency = F_CPU / (PRESCALER * 2^8)
    //TCCR0A = 0;
    //TCCR0A |= (1 << WGM00) | (1 << WGM01);
    TIMER_0_WAVE = 0;
    TIMER_0_WAVE |= TIMER_0_WAVE_MODE;
    // Re-enable all interrupts
    sei();
}

// timer1
void timers_setup_timer_1(void) {
    //16E6/64=250E3
    //250E3/250 = 1ms
    cli();
    //TIMER_1_CONTROL = 0;
    //TIMER_1_CONTROL |= TIMER_1_PRE_SCALE;
    //TIMER_1_CONTROL |= (1 << TIMER_1_MODE);
    //TIMER_1_CONTROL |= (1 << TIMER_1_COMPARE_MODE);
    //TIMER_1_COMPARE_RESOLUTION = TIMER_1_COMPARE_VALUE;
    TCCR1B = 0; // TODO: Yes / no ?
    TCCR1B |= (1 << CS10) | (1 << CS11); // Prescaler: 64 => 16E6/64 ticks/second
    TCCR1B |= (1 << WGM12); // Use Timer 1 in CTC-mode
    TIMSK1 |= (1 << OCIE1A); // Enable compare-match-interrupt for OCR1A
    OCR1A = 250;           // Every 16E6/ 16E3 ticks COMPA_vect is fired.
    // This equals an (non-existent) 512-clock-divisor.
    // We need this information for later calculations.
    // BTW: Keep in mind that there is one more OCR-register
    // for timer 1, which you can use to do some more neat
    // stuff.
    sei();
}
