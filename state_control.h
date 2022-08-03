/**
 * @file
 * @author Larson Schneider
 * @date 09.07.2022
 * @brief Main module for state handling
 * @version 0.1
 * @copyright MIT License.
 *
 * This modules handles the global state of the robot and calls every other modules. It is the key
 * point of the program. It reads inputs, changes operation modes, prints messages via the usart
 * module and so on.
 */

/**
 * @page states State module
 * This modules handles the global state of the robot and calls every other modules. It is the key
 * point of the program. It reads inputs, changes operation modes, prints messages via the usart
 * module and so on.
 * @tableofcontents
 *
 * @section secGloStat Global State
 * The global state is a section of the dynamic memory that is reserved in the form of a struct
 * and contains all kind of variables that are used by the @ref modules to do various things and
 * keep track of old states to evaluate or over function boarders.
 * @section secCycle Working Cycle
 *
 */

/**
 * @page ui User Interface
 * @section secUIUpdate State Updates
 */
#ifndef STATE_CONTROL_H
#define STATE_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <avr/wdt.h>
#include "robot_sensor.h"
#include "timers.h"
#include "usart.h"
#include "drive_control.h"
#include "state_control.h"
#include "led_control.h"

/**
 * @brief Represents the current state to the outside world. For example printing USART message or
 * turn on led's.
 * @param state Current state
 */
void state_show(track_state *state);

/**
 * @brief Print help message for the given state. Print different text if we are located on starting
 * field and no at all if the robot was once in the drive state.
 * @param state Current state
 */
void state_print_help(const track_state *state);

/**
 * @brief Applies effects and show state to the outside that depend on the current action.
 * @param oldAction Action that was present before the new state
 * @param state Internal state that contains the new state
 */
void state_on_action_change(track_state *state, action_type oldAction);

/**
 * @brief Tries to read an input from the USART, apply the action behind the character if any is
 * defined, send an error message for undefined characters.
 * @details Defined characters are: S, X; P, C, R, ?
 *
 * @param state Internal state
 */
void state_read_input(track_state *state);

/**
 * @brief Updates position of the state. Checks if the robot is: "on the start",
 * "on the track (if already started driving)" or "unknown (on the track but not started)"
 *
 * @param trackState The currently used state
 */
void state_update_position(track_state *trackState);

/**
 * @brief Runs the main loop of the robot, applies all actions, reads inputs
 *
 * @param trackState The currently used state
 */
_Noreturn void state_run_loop(track_state *trackState);

/**
 * @brief Sends state to the user interface if we are connected to our personal implementation.
 * @param trackState Internal state
 */
void state_send_update(const track_state *trackState);

#endif
