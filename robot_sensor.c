#include "robot_sensor.h"

ADC_clear(void) {
    // The following lines still let the digital input registers enabled,
    // though that's not a good idea (energy-consumption).

    // Klare Verhältnisse erstma!
    ADMUX = 0;
}

/** Initializes the ADC-unit. There is ONE single ADC unit on the
 * microcontroller, but different "channels" (input pins) can be
 * connected to the ADC via a MUX-register. The s4etup of the ADC is
 * done in this method, the MUX is used in the read-function.
 */
void ADC_init(void) {

  // Sets AVcc as the ADC's voltage source and as the reference,
  // while that voltage stems from the AREF-pin (5V when the robots is
  //  powered by batteries, lower otherwise).
  ADMUX |= (1<<REFS0);

  // This sets how fast (and: accurate) the ADC can run.
  // All these bits set to 1 set a sysclock-division of 128,
  // so the ADC can run with up to 125 kHz.
  ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);

  // This enables/really turns on the ADC. 
  ADCSRA |= (1<<ADEN);

  // The following lines of code start a single measurement in single
  // conversion mode. Needed once to "warm up" the ADC. The contents
  // of the result-register ADCW are ignored because they are not
  // reliable yet (until warm-up is done). ADSC goes to 0 when the
  // conversion is done. The first conversion is not only inreliable,
  // but also 25 ADC-cycles long, while the next conversions are around
  // 13 cycles long.
  ADCSRA |= (1<<ADSC);
  while (ADCSRA & (1<<ADSC)) {
    // zzzZZZzzzZZZzzz ... take a sleep until measurement done.
  }
  ADCW;
}

/** We have a 10-bit-ADC, so somewhere in memory we have to read that
 * 10 bits.  Due to this, this function returns a 16-bit-value.
 */
uint16_t ADC_read(uint8_t channel) {
  // Remember to have the ADC initialized!

  // The following line does set all ADMUX-MUX-pins to 0, disconnects
  // all channels from the MUX.
  ADMUX &= ~(ADMUX_CHN_ALL);
  ADMUX |= channel;

  // We start a single measurement and then busy-wait until
  // the ADSC-bit goes to 0, signalling the end of the measurement.
  ADCSRA |= (1<<ADSC);
  while (ADCSRA & (1<<ADSC)) {
    // zzzZZZzzzZZZzzz ... take a sleep until measurement done.
  }
  // Again, a pointer-airthmetical expression. the ADC-register has a
  // lower and a higher portion, but
  return ADCW;
}

uint16_t ADC_read_avg(uint8_t channel, uint8_t amount_samples) {
  // How large does our datatype need to be?
  uint32_t sum = 0;

  for (uint8_t i = 0; i < amount_samples; ++i ) {
    sum += ADC_read(channel);
  }

  return (uint16_t)( sum / (float)amount_samples );
}

SensorState sensor_get() {
    SensorState value = 0;
    if(ADC_read_avg(ADMUX_CHN_ADC2, ADC_AVG_AMOUNT) > SIGNAL_LEFT_UPPER){
        value|=SENSOR_LEFT;
    }
    if(ADC_read_avg(ADMUX_CHN_ADC1, ADC_AVG_AMOUNT) > SIGNAL_CENTER_UPPER){
        value|=SENSOR_CENTER;
    }
    if(ADC_read_avg(ADMUX_CHN_ADC0, ADC_AVG_AMOUNT) > SIGNAL_RIGHT_UPPER){
        value|=SENSOR_CENTER;
    }
    return value;
}

uint8_t left_state() {
	uint16_t value = ADC_read_avg(ADMUX_CHN_ADC2, ADC_AVG_AMOUNT);
	return value > SIGNAL_LEFT_UPPER ? STATE_HIGH : STATE_LOW;
}

uint8_t center_state() {
	uint16_t value = ADC_read_avg(ADMUX_CHN_ADC1, ADC_AVG_AMOUNT);
	return value > SIGNAL_CENTER_UPPER ? STATE_HIGH : STATE_LOW;
}

uint8_t right_state() {
	uint16_t value = ADC_read_avg(ADMUX_CHN_ADC0, ADC_AVG_AMOUNT);
	return value > SIGNAL_RIGHT_UPPER ? STATE_HIGH : STATE_LOW;
}
