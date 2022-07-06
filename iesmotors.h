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
* @brief Sets up timer 0 (8-bit-timer)
*/
void setupTimer0(void);
/**
  @brief Sets duty-cycle at pin PD5 or PD6 (OC0A or OC0B) to a value
  (0 - 255 = 0% - 100%).

  @details Timer0 needs to be setup before usage!

  @details The required pins also need to be set as output first. That
  setup is NOT done in the respective timer-setup-method!
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
