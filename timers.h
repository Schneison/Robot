/**
 * @file timers.h
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Utility class
 * @version 0.1
 */
#ifndef TIMERS
#define TIMERS

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "usart.h"
#include <stdio.h>
#include "utility.h"

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
    COUNTER_6_HZ,
    /**
 * @brief 8 HZ Counter
 */
    COUNTER_8_HZ,
    COUNTER_3_HZ,
    COUNTER_12_HZ,
    COUNTER_4_HZ,
} counter_def;

/**
 * @brief Contains the frequencies for the corresponding counters in #counter_def
 */
static const uint16_t counter_frequencies[COUNTER_AMOUNT] = {1000 / 1, 1000 / 5, 1000 / 6, 1000 / 8, 1000 / 3,
                                                             1000 / 12, 1000 / 4};

/**
 * @brief Creates all internal timers/counters and place them in the given array.
 *
 * @param counters Array of counters, has to be the size of #COUNTER_AMOUNT
 * @sa #counter_def
 */
void timers_create(counter *counters);

/**
 * @brief Updates all counters based on the current millisecond value that is created by the internal board timer1.
 * <p>Allocates and initialises the counters if the pointer is NULL.
 * @sa #timers_setup_timer_1()
 * @param counters Array / Pointer that contains the counters for all registered frequencies.
 */
void timers_update(counter *counters);

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
 * @brief Timer 0 normal operation mode for compare A
 */
#define TIMER_0_NORMAL_OPERATION_A ~(1 << COM0A1) & ~(1 << COM0A0)
/**
 * @brief Timer 0 normal operation mode for compare B
 */
#define TIMER_0_NORMAL_OPERATION_B ~(1 << COM0A1) & ~(1 << COM0A0)

/**
 * @brief Set waveform generation mode to Fast PWM, frequency = F_CPU / (PRESCALER * 2^8)
 */
#define TIMER_0_WAVE_MODE ((1 << WGM00) | (1 << WGM01))
/**
 * @brief Counter 0 counter resolution A
 */
#define TIMER_0_COMPARE_RESOLUTION_A OCR0A
/**
 * @brief Counter 0 counter resolution B
 */
#define TIMER_0_COMPARE_RESOLUTION_B OCR0B

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
 * @copybrief timers_check(struct Counter*, counter_def)
 * @param state Current state of the robot that contains the counters.
 * @param counterDef The definition of the counter that should be checked.
 * @details The amount of counter is defined by #COUNTER_AMOUNT.
 * @retval 1 if the frequency is meet this cycle.
 * @retval 0 if the frequency is not meet this cycle.
 */
uint8_t timers_check_state(track_state *state, counter_def counterDef);

/**
 * @brief Checks if the counter that is defined with the given definition has a true value this cycle.
 *
 * @param counters Array / Pointer that contains the counters for all registered frequencies.
 * @param counterDef The definition of the counter that should be checked.
 * @details The amount of counter is defined by #COUNTER_AMOUNT.
 * @retval 1 if the frequency is meet this cycle.
 * @retval 0 if the frequency is not meet this cycle.
 */
uint8_t timers_check(const counter *counters, counter_def counterDef);

/**
 * @brief Prints then given message if the frequency requirement is currently meed.
 *
 * @param frequency Frequency on which the given text should be printed.
 * @param text The text that should be printed
 * @param counters Array of counters, has to be the size of #COUNTER_AMOUNT, and is typically located on the global state
 */
void timers_print(counter *counters, counter_def frequency, const char *text);

/**
 * @brief Setup method for timers module, setups all timers
 */
void timers_init(void);


/**
 * @brief Sets up timer which is responsible for the duty cycle of the two motors.
 * @details Timer0 on the board
*/
void timers_setup_timer_0(void);

/**
 * @brief Sets up timer which is responsible for the internal counter.
 * @details Timer1 on the board
*/
void timers_setup_timer_1(void);

#endif