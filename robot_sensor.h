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

/**
 * @brief First channel, used by left sensor
 * @sa #ADMUX_CHN_ADC1
 * @sa #ADMUX_CHN_ADC2
 * @sa #ADMUX_CHN_ALL
 */
#define ADMUX_CHN_ADC0 0 // 0000 0000
/**
 * @brief Second channel, used by center sensor
 * @sa #ADMUX_CHN_ADC0
 * @sa #ADMUX_CHN_ADC2
 * @sa #ADMUX_CHN_ALL
 */
#define ADMUX_CHN_ADC1 1 // 0000 0001
/**
 * @brief Third channel, used by right sensor
 * @sa #ADMUX_CHN_ADC0
 * @sa #ADMUX_CHN_ADC1
 * @sa #ADMUX_CHN_ALL
 */
#define ADMUX_CHN_ADC2 2 // 0000 0010
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
 * @brief No signal strength for any of the three sensors.
 * @sa #STATE_MIDDLE
 * @sa #STATE_HIGH
 */
#define STATE_LOW 0
/**
 * @brief Mediocre signal strength for any of the three sensors.
 * @sa #STATE_LOW
 * @sa #STATE_HIGH
 */
#define STATE_MIDDLE 1
/**
 * @brief Maximal signal strength for any of the three sensors.
 * @sa #STATE_LOW
 * @sa #STATE_MIDDLE
 */
#define STATE_HIGH 2

/**
 * @brief Describes the binary state of the sensors
 */
typedef enum{
/**
 * @brief Left sensor is high
 */
    SENSOR_LEFT = 1,
/**
 * @brief Center sensor is high
 */
    SENSOR_CENTER = 2,
/**
 * @brief Right sensor is high
 */
    SENSOR_RIGHT = 4,
/**
 * @brief All sensors are high
 */
    SENSOR_ALL = 7,
} SensorState;

/**
 * @brief Threshold of the right sensor
 * @details This will determine if the signal of the sensor is read as positive.
 */
#define SIGNAL_RIGHT_UPPER 600
/**
 * @brief Threshold of the center sensor
 * @details This will determine if the signal of the sensor is read as positive.
 */
#define SIGNAL_CENTER_UPPER 600
/**
 * @brief Threshold of the left sensor
 * @details This will determine if the signal of the sensor is read as positive.
 */
#define SIGNAL_LEFT_UPPER 600

/**
 * @brief Reads the output signals on the given channel of the adc (analog-digital-converter) module
 * @param channel Channel on the adc module as defined
 * @details We have a 10-bit-ADC, so somewhere in memory we have to read that
 * 10 bits.  Due to this, this function returns a 16-bit-value.
 * @return Digital value measured
 */
uint16_t ADC_read(uint8_t channel);

/**
 * @brief Reads the output signals on the given channel of the adc (analog-digital-converter) module but with a given
 * amount of time, to reduce noise
 * @param channel Channel on the adc module as defined
 * @param amount_samples Numbers of samples used
 *
 * @return Average digital value measured
 */
uint16_t ADC_read_avg(uint8_t channel, uint8_t amount_samples);

/**
 * @brief Reads the state of all field sensors.
 * @retval SensorState#SENSOR_LEFT
 */
SensorState sensor_get();

/**
 * @brief Reads the state of the left field sensor
 * @retval #STATE_LOW No signal
 * @retval #STATE_MIDDLE Mediocre signal
 * @retval #STATE_HIGH Full signal
 */
uint8_t left_state();

/**
 * @brief Reads the state of the center field sensor
 * @retval #STATE_LOW No signal
 * @retval #STATE_MIDDLE Mediocre signal
 * @retval #STATE_HIGH Full signal
 */
uint8_t center_state();

/**
 * @brief Reads the state of the right field sensor
 * @retval #STATE_LOW No signal
 * @retval #STATE_MIDDLE Mediocre signal
 * @retval #STATE_HIGH Full signal
 */
uint8_t right_state();



/**
 * @brief Initialises the sensor module
 */
void ADC_init(void);

/**
 * @brief Clears all pins used by the sensor module.
 *
 * @details Required to be called before #ADC_init()
 */
void ADC_clear(void);

#endif
