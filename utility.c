#include "utility.h"

/**
 * @brief Updates counter variables
 * @details Called after the count timer reaches the compare value
 */
ISR (PCINT0_vect) {
    if(count == 1000){
        seconds+=1;
    }
    count++;
}

uint8_t check_freq(uint8_t frequency){
    return count % frequency == 0;
}

// timer0
void setupMotorTimer(void) {
  // Disable all interrupts
  cli();
  // Set prescaler to 64, cf. datasheet for TCCR0B
  // (TCCR0B: Timer/Counter Control Register 0 B)
  TCCR0B = 0;
  TCCR0B |= (1 << CS00) | (1 << CS01);
  // Set waveform generation mode to Fast PWM, frequency = F_CPU / (PRESCALER * 2^8)
  TCCR0A = 0;
  TCCR0A |= (1 << WGM00) | (1 << WGM01);
  // Re-enable all interrupts
  sei();
}

// timer1
void setupCountTimer(void) {
    //16E6/64=250E3
    //250E3/250 = 1ms
    cli();
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
