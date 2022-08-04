/**
 * @file
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Utility class
 * @version 0.1
 * @copyright MIT License.
 */
/**
 * @page timers Timer module
 * @tableofcontents
 * @section secFre Frequencies
 * Many tasks of this project are executed in on pre defined frequencies, i.e. every half second,
 * second, and so on. For this function we need a clock / timer that counts how many milliseconds
 * passed since the last check. In our case we use the @ref secTimer1 "Timer 1" for this task. The
 * timer is setup for with matching values to accomplish this task for more information see
 * the timer 1 section. @n
 * For every pre defined unique frequency we have a struct array in the
 * @ref secGloStat "global state". The struct itself is called "counter" and is a helper to
 * keep control over the different frequencies without doing to much boiler plate code.
 *
 * @section secCounter Counters
 * Counters are a helper struct to keep track of all frequencies there associated time in
 * milliseconds and an variable that contains the last time value on that the frequency requirement
 * was meet. Every start of a @ref secCycle "work cycle" every counter will be checked if the delta
 * between the last time the frequency requirement was meet and the current time is equal or exceeds
 * the associated time of the frequency. If so the counter will revive an enabled value for this
 * cycle and the last time variable will be set to the current time. In the next cycle the counter
 * will be disabled again automatically because the delta is to small.
 *
 * @section secTimer0 Timer 0
 * This timer used by the duty cycle of the motors. It is a 8-Bit timer wich pre-scale value is set
 * to 64 (For reference: @ref TIMER_0_PRE_SCALE). The operation mode is set to fast PWM (i.e. it
 * waits until the compared value is meet and then resets its value to 0).
 * The two compare values of this timer A and B are set by the
 * @ref secDriDuty "duty cycle" to the corresponding speed value of the wheels.
 * @n
 * For more info see datasheet p.142
 * @f[ f = \frac{F\_CPU}{PRESCALER * 2^8}@f]
 *
 * @section secTimer1 Timer 1
 * This timer is used for the counters used to check for meet frequency requirements. This timer is
 * a 16-Bit timer and has a pre-scale value set to 64. Here only the compare value A is used and
 * set to 250 together with the defined pre-scale value the timer will meet is compare value every
 * milli second. If the timer value exceeds or equals the compare value an interrupt will be caused
 * wich increases the internal current time value which is used by the @ref secCounter "counter"
 * structures.
 * @f[ f = \frac{F\_CPU}{PRESCALER}@f]
 *
 */
#ifndef TIMERS
#define TIMERS

#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "utility.h"

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
#define TIMER_0_NORMAL_OPERATION_A (~(1 << COM0A1) & ~(1 << COM0A0))
/**
 * @brief Timer 0 normal operation mode for compare B
 */
#define TIMER_0_NORMAL_OPERATION_B (~(1 << COM0A1) & ~(1 << COM0A0))

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
/** @brief Register for interrupt mask */
#define TIMER_1_INTERRUPT TIMSK1
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
 * @brief Counter variable, which contains a value from 0 to 255. This value represents the
 * milliseconds since the last second. One unit is (1000/255) ms.
 */
extern uint32_t millis;

/**
 * @brief Contains the frequencies for the corresponding counters in #counter_def
 */
extern const uint16_t counter_frequencies[COUNTER_AMOUNT];

/**
 * @brief Defines counters with different frequencies to allow output in the given frequencies.
 */
typedef enum {
    /**
     * @brief 1 HZ Counter
     */
    COUNTER_1_HZ,
    COUNTER_2_HZ,
    /**
 * @brief 5 HZ Counter
 */
    COUNTER_10_HZ,
    /**
 * @brief 8 HZ Counter
 */
    COUNTER_12_HZ,
    COUNTER_32_HZ,
} counter_def;

/**
 * @brief Creates all internal timers/counters and place them in the given array.
 *
 * @param counters Array of counters, has to be the size of #COUNTER_AMOUNT
 * @sa #counter_def
 */
void timers_create(counter *counters);

/**
 * @brief Updates all counters based on the current millisecond value that is created by the
 * internal board timer1.
 * <p>Allocates and initialises the counters if the pointer is NULL.
 * @sa #timers_setup_timer_1()
 * @param counters Array / Pointer that contains the counters for all registered frequencies.
 */
void timers_update(counter *counters);

/**
 * @copybrief timers_check(const counter*, counter_def)
 * @param state Current state of the robot that contains the counters.
 * @param counterDef The definition of the counter that should be checked.
 * @details The amount of counter is defined by #COUNTER_AMOUNT.
 * @retval 1 if the frequency is meet this cycle.
 * @retval 0 if the frequency is not meet this cycle.
 */
uint8_t timers_check_state(const track_state *state, counter_def counterDef);

/**
 * @brief Checks if the counter that is defined with the given definition has a true value this
 * cycle.
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
 * @param counters Array of counters, has to be the size of #COUNTER_AMOUNT, and is typically
 * located on the global state
 */
void timers_print(const counter *counters, counter_def frequency, const char *text);

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