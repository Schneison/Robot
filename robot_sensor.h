/**
 * @file robot_sensor.h
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Reads the sensors of the roboter
 * @version 0.1
 */
#ifndef RO_SIGNALS
#define RO_SIGNALS

#include <avr/io.h>
#include <stdio.h>

// ADC Channels
#define ADMUX_CHN_ADC0 0 // 0000 0000
#define ADMUX_CHN_ADC1 1 // 0000 0001
#define ADMUX_CHN_ADC2 2 // 0000 0010
#define ADMUX_CHN_ADC3 3 // 0000 0011
#define ADMUX_CHN_ALL 7  // 0000 0111

// Average some measurements to reduce probable noise.
#define ADC_AVG_WINDOW 20

// Seonsor state
#define STATE_LOW 0
#define STATE_MIDDLE 1
#define STATE_HIGH 2

#define SIGNAL_RIGHT_UPPER 600
#define SIGNAL_CENTER_UPPER 600
#define SIGNAL_LEFT_UPPER 600


/**
 * @brief Reads the output signals on the given channel of the adc (analog-digital-converter) module
 * @param channel Channel on the adc module as defined
 *
 * @return Digital value measured
 */
uint16_t ADC_read(uint8_t channel);

/**
 * @brief Reads the output signals on the given channel of the adc (analog-digital-converter) module but with a given
 * amount of time, to reduce noise
 * @param channel Channel on the adc module as defined
 * @param nsamples Numbers of samples used
 *
 * @return Average digital value measured
 */
uint16_t ADC_read_avg(uint8_t channel, uint8_t nsamples);

/**
 * @brief Reads the state of the left field sensor
 * @retval STATE_LOW No signal
 * @retval STATE_MIDDLE Mediocre signal
 * @retval STATE_HIGH Full signal
 */
uint8_t left_state();

/**
 * @brief Reads the state of the center field sensor
 * @retval STATE_LOW No signal
 * @retval STATE_MIDDLE Mediocre signal
 * @retval STATE_HIGH Full signal
 */
uint8_t center_state();

/**
 * @brief Reads the state of the right field sensor
 * @retval STATE_LOW No signal
 * @retval STATE_MIDDLE Mediocre signal
 * @retval STATE_HIGH Full signal
 */
uint8_t right_state();

/**
 * @brief Initialises the sensor module
 */
void ADC_init(void);

#endif
