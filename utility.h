/**
 * @file utility.h
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Utility class
 * @version 0.1
 */
#ifndef UTILITY
#define UTILITY

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/**
 * @brief Counter variable, which contains a value from 0 to 255. This value represents the milliseconds since the last
 * second. One unit is (1000/255) ms.
 */
extern uint16_t millis;

/**
 * @brief Defines counters with different frequencies to allow output in the given frequencies.
 */
typedef enum {
    /**
     * @brief 1 HZ Counter
     */
    COUNTER_1_HZ,
    /**
 * @brief 5 HZ Counter
 */
    COUNTER_5_HZ,
    /**
 * @brief 8 HZ Counter
 */
    COUNTER_8_HZ
} counter_def;

/**
 * @brief Contains the frequencies for the corresponding counters in #counter_def
 */
static const uint16_t counter_frequencies[] = { 1000 / 1, 1000 / 5,1000 / 8 };

/**
 * @brief Amount of counters that are defined in #counter_def
 */
#define COUNTER_AMOUNT 3

/**
 * @brief Helper struct that is used to check frequency requirements every cycle. For example this is used to print a
 * message every 1 HZ or every 5 HZ.
 * <p>
 * The amount of defined counters is #COUNTER_AMOUNT
 * @details All counters for the frequencies 1HZ, 5HZ and 8HZ are located in the enum #counter_def
 */
struct Counter {
    /**
     * @brief Milliseconds since last true cycle.
     */
    uint16_t lastMillis;
    /**
     * @brief If the frequency applies this cycle. Possible values are 0 and 1.
     */
    uint8_t value;
    /**
     * @brief Frequency threshold
     */
    uint8_t threshold;
};

/**
 * @brief Updates all counters based on the current millisecond value that is created by the internal board timer1.
 * <p>Allocates and initialises the counters if the pointer is NULL.
 * @sa #setupCountTimer()
 * @param counters Array / Pointer that contains the counters for all registered frequencies.
 */
void updateCounters(struct Counter* counters);

/**
 * @brief  Timer Control Register of the first timer
 */
#define TIMER_0_CONTROL TCCR0B
/**
 * @brief Set prescaler to 64. For more info see datasheet p.142
 */
#define TIMER_0_PRE_SCALE ((1 << CS00) | (1 << CS01))
/**
 * @brief Timer 0 settings (waveform generation mode and port operation)
 */
#define TIMER_0_WAVE TCCR0A

/**
 * @brief Set waveform generation mode to Fast PWM, frequency = F_CPU / (PRESCALER * 2^8)
 */
#define TIMER_0_WAVE_MODE ((1 << WGM00) | (1 << WGM01))

/**
 * @brief Control Register A and B of timer1
 */
#define TIMER_1_CONTROL TCCR1B
/**
 * @brief Sets the prescale value of timer1 to 64. For more info see datasheet p.173
 */
#define TIMER_1_PRE_SCALE ((1 << CS10) | (1 << CS11))
/**
 * @brief Timer 1 in CTC-mode
 */
#define TIMER_1_MODE WGM12
/**
 * @brief Enables compare-match-interrupt
 */
#define TIMER_1_COMPARE_MODE OCIE1A
/**
 * @brief Registry of the pin that is used  to manipulate the counter resolution
 */
#define TIMER_1_COMPARE_RESOLUTION OCR1A
/**
 * @brief Compare value of timer1
 * @details 16E6/64=250E3; 250E3/250 => 1000ms
 */
#define TIMER_1_COMPARE_VALUE 250

/**
 * @brief Checks the current millisecond value if the equals the frequency
 * @param frequency
 * @return
 * @retval 0 If the current millisecond value is between two thresholds.
 * @retval 1 if the current millisecond value equals the frequency
 */
uint8_t check_freq(uint8_t frequency);

/**
 * @brief Checks if the counter that is defined with the given definition has a true value this cycle.
 *
 * @param counters Array / Pointer that contains the counters for all registered frequencies.
 * @param counterDef The definition of the counter that should be checked.
 * @details The amount of counter is defined by #COUNTER_AMOUNT.
 * @retval 1 if the frequency is meet this cycle.
 * @retval 0 if the frequency is not meet this cycle.
 */
uint8_t check_counter(struct Counter* counters, counter_def counterDef);

/**
 * @brief Sets up timer which is responsible for the duty cycle of the two motors.
 * @details Timer0 on the board
*/
void setupMotorTimer(void);

/**
 * @brief Sets up timer which is responsible for the internal counter.
 * @details Timer1 on the board
*/
void setupCountTimer(void);
#endif






























































/*
 .______________ _________                       __           ._.
|   \_   _____//   _____/ _______  ____   ____ |  | __  _____| |
|   ||    __)_ \_____  \  \_  __ \/  _ \_/ ___\|  |/ / /  ___/ |
|   ||        \/        \  |  | \(  <_> )  \___|    <  \___ \ \|
|___/_______  /_______  /  |__|   \____/ \___  >__|_ \/____  >__
            \/        \/                     \/     \/     \/ \/
*/

// blubb
