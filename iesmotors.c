#include "iesmotors.h"

/**
 * @brief Updates counter variables
 * @details Called after the count timer reaches the compare value
 */
ISR (PCINT0_vect) {
    if(count == 255){
        seconds+=1;
    }
    count++;
}

uint8_t check_freq(uint8_t frequency){
    return count % frequency == 0;
}

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

void setupCountTimer(void) {
    cli();
    TCCR1B = 0; // TODO: Yes / no ?
    TCCR1B |= (1 << CS10) | (1 << CS12); // Prescaler: 1 => 16E6 ticks/second
    TCCR1B |= (1 << WGM12); // Use Timer 1 in CTC-mode
    TIMSK1 |= (1 << OCIE1A); // Enable compare-match-interrupt for OCR1A
    OCR1A = 62746;           // Every 16E6/ 62746 ticks COMPA_vect is fired.
    // This equals an (non-existent) 512-clock-divisor.
    // We need this information for later calculations.
    // BTW: Keep in mind that there is one more OCR-register
    // for timer 1, which you can use to do some more neat
    // stuff.
    sei();
}

void setDutyCycle(uint8_t pin, uint8_t value)
{
  // Suggestion to handle PD6 - note the code-clones wrt. PD5 below!
  // Code-clones are extraordinary f cky! Correct this (tricky though
  // due to the PP-macros, which you cannot simply pass to functions)!
  // (But PP-macros can help you here....)
  if (pin == PD6) {
    if (value == 0) {
      TCCR0A &= ~(1 << COM0A1) & ~(1 << COM0A0);  // Normal port operation mode
      PORTD &= ~(1 << PD6);                       // PD6 LOW, equals 0% duty,
    }                                             // timer disconnected
    else if (value == 255) {
      TCCR0A &= ~(1 << COM0A1) & ~(1 << COM0A0);  // Normal port operation mode
      PORTD |= (1 << PD6);                        // PD6 HIGH, equals 100% duty,
    }                                             // timer disconnected
    else {
      TCCR0A |= (1 << COM0A1);                    // OC0A to LOW on Compare Match,
      TCCR0A &= ~(1 << COM0A0);                   // to HIGH at BOTTOM (non-inverting mode)
      OCR0A = value;                              // generates sequences of 1-0-1-0...
    }                                             // for certain periods of time
  }

  if (pin == PD5) {
    if (value == 0) {
      TCCR0A &= ~(1 << COM0B1) & ~(1 << COM0B0);
      PORTD &= ~(1 << PD5);
    }
    else if (value == 255) {
      TCCR0A &= ~(1 << COM0B1) & ~(1 << COM0B0);
      PORTD |= (1 << PD5);
    }
    else {
      TCCR0A |= (1 << COM0B1);
      TCCR0A &= ~(1 << COM0B0);
      OCR0B = value;
    }
  }
}
