/**
 * @file
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Reads the sensors of the roboter
 * @version 0.1
 *
 * This module provides functions to read the state of the adc converters with are connected to the
 * reflective optical sensors on the robot and to the battery.
 */
/**
 * @page sensor Sensor module
 * @tableofcontents
 * This module provides functions to read the state of the adc converters with are connected to the
 * reflective optical sensors on the robot and to the battery.
 *
 * @section secADC Analog Digital Converter
 * The adc converts an analog input stream to a digital output stream. For this is needs a
 * reference Voltage which is provided by the battery. The adc has many channels which can be used
 * to connect and read different devices. We use the channels 0 to 2 for the optical sensors and
 * channel 3 is connected to an voltage divider which is connected to the battery.
 * @sa ADMUX_CHN_ADC0
 * @sa #ADMUX_CHN_ADC1
 * @sa #ADMUX_CHN_ADC2
 * @sa #ADMUX_CHN_ADC3
 * @sa #ADMUX_CHN_ALL
 *
 * @section Reflective Optical Sensors
 * We use three reflective optical sensors for detection of the @ref track "track". Every sensor
 * has its own threshold when the program will accept a line to be found this is needed because
 * every sensor has a different calibration. Every measurement is done multiple times to reduce the
 * possibility that indirect noise can distort the result. (The amount is defined in
 * @ref ADC_AVG_AMOUNT)
 * @sa #ADC_AVG_AMOUNT
 * @sa #SIGNAL_RIGHT_UPPER
 * @sa #SIGNAL_CENTER_UPPER
 * @sa #SIGNAL_LEFT_UPPER
 *
 * @section secBat Battery Voltage
 * The last channel we use is for the battery voltage. The battery is connected to the pin adc 3 via
 * a voltage divider.
 */
#ifndef RO_SIGNALS
#define RO_SIGNALS

#include <avr/io.h>
#include "utility.h"

/** @brief Data direction registry of the right sensor */
#define DR_ADC_0 DDRC
/** @brief Data direction registry pint of the right sensor */
#define DP_ADC_0 DDC0
/** @brief Data direction registry of the center sensor */
#define DR_ADC_1 DDRC
/** @brief Data direction registry pin of the center sensor */
#define DP_ADC_1 DDC1
/** @brief Data direction registry of the left sensor */
#define DR_ADC_2 DDRC
/** @brief Data direction registry pin of the left sensor */
#define DP_ADC_2 DDC2
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
#define ADMUX_CHN_ALL 3  // 0000 0011

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

/** @brief Min Operating Voltage of the board */
#define BATTERY_MIN 20
/** @brief Max Operating Voltage of the board */
#define BATTERY_MAX 220
/** @brief Range in that the battery voltage can fluctuate */
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
