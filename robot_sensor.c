/**
 * @file robot_sensor.c
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Reads the sensors of the roboter
 * @version 0.1
 */
#include "robot_sensor.h"

void sensor_clear(void) {
    // The following lines still let the digital input registers enabled,
    // though that's not a good idea (energy-consumption).
    A_MUX_SELECTION = 0;
}

void sensor_init(void) {
    DR_ADC_0 &= ~(1 << DP_ADC_0);
    DR_ADC_1 &= ~(1 << DP_ADC_1);
    DR_ADC_2 &= ~(1 << DP_ADC_2);
    DR_ADC_3 &= ~(1 << DP_ADC_3);

    // Sets AVcc as the ADC's voltage source and as the reference,
    // while that voltage stems from the AREF-pin (5V when the robots is
    //  powered by batteries, lower otherwise).
    A_MUX_SELECTION |= (1 << A_MUX_VOLTTAGE_REF);

    // This sets how fast (and: accurate) the ADC can run.
    // All these bits set to 1 set a sysclock-division of 128,
    // so the ADC can run with up to 125 kHz.
    A_MUX_STATUS |= A_MUX_STATUS_PRE_SCALE;

    // This enables/really turns on the ADC.
    A_MUX_STATUS |= (1 << A_MUX_STATUS_ENABLE);

    // The following lines of code start a single measurement in single
    // conversion mode. Needed once to "warm up" the ADC. The contents
    // of the result-register ADCW are ignored because they are not
    // reliable yet (until warm-up is done). ADSC goes to 0 when the
    // conversion is done. The first conversion is not only inreliable,
    // but also 25 ADC-cycles long, while the next conversions are around
    // 13 cycles long.
    A_MUX_STATUS |= (1 << A_MUX_STATUS_START);
    while (A_MUX_STATUS & (1 << A_MUX_STATUS_START)) {
        // zzzZZZzzzZZZzzz ... take a sleep until measurement done.
    }
    A_MUX_RESULT;
}

/** We have a 10-bit-ADC, so somewhere in memory we have to read that
 * 10 bits.  Due to this, this function returns a 16-bit-value.
 */
uint16_t sensor_adc_read(uint8_t channel) {
    // Remember to have the ADC initialized!

    // The following line does set all ADMUX-MUX-pins to 0, disconnects
    // all channels from the MUX.
    A_MUX_SELECTION &= ~ADMUX_CHN_ALL;
    A_MUX_SELECTION |= channel;

    // We start a single measurement and then busy-wait until
    // the ADSC-bit goes to 0, signalling the end of the measurement.
    A_MUX_STATUS |= (1 << A_MUX_STATUS_START);
    while (A_MUX_STATUS & (1 << A_MUX_STATUS_START)) {
        // zzzZZZzzzZZZzzz ... take a sleep until measurement done.
    }
    // Again, a pointer-airthmetical expression. the ADC-register has a
    // lower and a higher portion, but
    return A_MUX_RESULT;
}

uint16_t sensor_adc_read_avg(uint8_t channel, uint8_t amount_samples) {
    // How large does our datatype need to be?
    float sum = 0;

    for (uint8_t i = 0; i < amount_samples; ++i) {
        sum += sensor_adc_read(channel);
    }

    return (uint16_t)(sum / (float) amount_samples);
}

sensor_state sensor_get_state() {
    sensor_state value = 0;
    if (sensor_adc_read_avg(ADMUX_CHN_ADC2, ADC_AVG_AMOUNT) > SIGNAL_LEFT_UPPER) {
        value |= SENSOR_LEFT;
    }
    if (sensor_adc_read_avg(ADMUX_CHN_ADC1, ADC_AVG_AMOUNT) > SIGNAL_CENTER_UPPER) {
        value |= SENSOR_CENTER;
    }
    if (sensor_adc_read_avg(ADMUX_CHN_ADC0, ADC_AVG_AMOUNT) > SIGNAL_RIGHT_UPPER) {
        value |= SENSOR_RIGHT;
    }
    return value;
}

uint8_t sensor_get_battery(void) {
    return (uint8_t)(sensor_adc_read_avg(ADMUX_CHN_ADC3, 100));
}
