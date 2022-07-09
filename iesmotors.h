/**
 * @file iesmotors.h
 * @author Larson Schneider
 * @date 06.07.2022
 * @brief Utility class for motor functions
 * @version 0.1
 */
#ifndef IESMOTORS
#define IESMOTORS

#include <avr/io.h>
#include <avr/interrupt.h>
/**
 * @brief Counter variable, which contains a value from 0 to 255. This value represents the milliseconds since the last
 * second. One unit is (1000/255) ms.
 */
uint8_t count = 0;
/**
 * @brief Seconds since start of the timer
 */
uint8_t seconds = 0;

/**
* @brief Sets up timer which is responsible for the duty cycle of the two motors.
*/
void setupMotorTimer(void);

/**
* @brief Sets up timer which is responsible for the internal counter.
*/
void setupCountTimer(void);

/**
 * @brief Defines speed of the wheels by defining how often a they are turned on,
 * a higher value results in an faster wheel.
 *
 * @details Call #setupMotorTimer() before usage!
 * @param pin PD5/PD6 for left/right motor cycle
 * @param value Defines speed of the wheels. (0 - 255 = 0% - 100%)
 */
void setDutyCycle(uint8_t pin, uint8_t value);
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
