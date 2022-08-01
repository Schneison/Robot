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
#include "utility.h"

/** @brief Active channels and reference of the admux */
#define A_MUX_SELECTION ADMUX
/** @brief Admux voltage reference */
#define A_MUX_VOLTTAGE_REF REFS0
/** @brief Status of the adc control registry */
#define A_MUX_STATUS ADCSRA
/** @brief Flag to enable the admux*/
#define A_MUX_STATUS_ENABLE ADEN
/** @brief Flag to start adc conversion */
#define A_MUX_STATUS_START ADSC
/** @brief Bits determine the division factor between the system clock
 * frequency and the input clock to the ADC
 * @details See datasheet p.319
 * @details sysclock-division of 128
 * @details With this the ADC can run with up to 125 kHz
 */
#define A_MUX_STATUS_PRE_SCALE (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
/** @brief Registry that contains the result when the conversion is complete */
#define A_MUX_RESULT ADCW

/**
 * @brief First channel, used for right sensor
 * @sa #ADMUX_CHN_ADC1
 * @sa #ADMUX_CHN_ADC2
 * @sa #ADMUX_CHN_ADC3
 * @sa #ADMUX_CHN_ALL
 */
#define ADMUX_CHN_ADC0 0 // 0000 0000
/**
 * @brief Second channel, used for center sensor
 * @sa #ADMUX_CHN_ADC0
 * @sa #ADMUX_CHN_ADC2
 * @sa #ADMUX_CHN_ADC3
 * @sa #ADMUX_CHN_ALL
 */
#define ADMUX_CHN_ADC1 1 // 0000 0001
/**
 * @brief Third channel, used for left sensor
 * @sa #ADMUX_CHN_ADC0
 * @sa #ADMUX_CHN_ADC1
 * @sa #ADMUX_CHN_ADC3
 * @sa #ADMUX_CHN_ALL
 */
#define ADMUX_CHN_ADC2 2 // 0000 0010
/**
 * @brief Third channel, used for the battery
 * @sa #ADMUX_CHN_ADC0
 * @sa #ADMUX_CHN_ADC1
 * @sa #ADMUX_CHN_ADC2
 * @sa #ADMUX_CHN_ALL
 */
#define ADMUX_CHN_ADC3 3 // 0000 0011
/**
 * @brief Constant that contains all possible channels
 * @sa #ADMUX_CHN_ADC0
 * @sa #ADMUX_CHN_ADC1
 * @sa #ADMUX_CHN_ADC2
 */
#define ADMUX_CHN_ALL 7  // 0000 0111

/**
 * @brief Amount of measurements made by the analog-digital-converter
 * @details Average some measurements to reduce probable noise.
 */
#define ADC_AVG_AMOUNT 20

/**
 * @brief Threshold of the right sensor
 * @details This will determine if the signal of the sensor is read as positive.
 */
#define SIGNAL_RIGHT_UPPER 220
/**
 * @brief Threshold of the center sensor
 * @details This will determine if the signal of the sensor is read as positive.
 */
#define SIGNAL_CENTER_UPPER 150
/**
 * @brief Threshold of the left sensor
 * @details This will determine if the signal of the sensor is read as positive.
 */
#define SIGNAL_LEFT_UPPER 250

/**
 * @brief Min Operating Voltage of the board
 */
#define BATTERY_MIN 20
/**
 * @brief Max Operating Voltage of the board
 */
#define BATTERY_MAX 220

#define BATTERY_RANGE 10

/**
 * @brief Reads the output signals on the given channel of the adc (analog-digital-converter) module
 * @param channel Channel on the adc module as defined
 * @details We have a 10-bit-ADC, so somewhere in memory we have to read that
 * 10 bits.  Due to this, this function returns a 16-bit-value.
 * @return Digital value measured
 */
uint16_t sensor_adc_read(uint8_t channel);

/**
 * @brief Reads the output signals on the given channel of the adc (analog-digital-converter) module
 * but with a given amount of time, to reduce noise
 * @param channel Channel on the adc module as defined
 * @param amount_samples Numbers of samples used
 *
 * @return Average digital value measured
 */
uint16_t sensor_adc_read_avg(uint8_t channel, uint8_t amount_samples);

/**
 * @brief Reads the state of all field sensors.
 * @retval sensor_state#SENSOR_LEFT
 */
sensor_state sensor_get_state();

/**
 * @brief Reads the state of the battery and retrieves a percent value of voltage of the battery
 * multiplied by 100
 * @retval 0 to 100, based on the percent of voltage of the battery multiplied by 100
 */
uint8_t sensor_get_battery(void);

/**
 * @brief Initialises the sensor module
 * @details There is ONE single ADC unit on the microcontroller but different "channels"
 * @details The setup of the ADC is done in this method, the MUX is used in the read-function.
 * @sa #sensor_adc_read
 * @sa #sensor_adc_read_avg
 */
void sensor_init(void);

/**
 * @brief Clears all pins used by the sensor module.
 *
 * @details Required to be called before #sensor_init()
 */
void sensor_clear(void);

#endif
