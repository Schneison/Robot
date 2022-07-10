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
/**
 * @brief Counter variable, which contains a value from 0 to 255. This value represents the milliseconds since the last
 * second. One unit is (1000/255) ms.
 */
uint16_t count = 0;
/**
 * @brief Seconds since start of the timer
 */
uint16_t seconds = 0;

/**
 * @brief Checks the current millisecond value if the equals the frequency
 * @param frequency
 * @return
 * @retval 0 If the current millisecond value is between two thresholds.
 * @retval 1 if the current millisecond value equals the frequency
 */
uint8_t check_freq(uint8_t frequency);

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
