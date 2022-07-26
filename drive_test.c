#include <avr/io.h>
#include "timers.h"
#include "iesusart.h"
#include "robot_sensor.h"
#include "drive_control.h"
#include "state_control.h"
#include "led_control.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Setup board registries
 */
void setup(void) {
    motor_clear();
    ADC_clear();

    USART_init(UBRR_SETTING);
    ADC_init();
    motor_init();
    led_init();
    timers_init();
}

/**
 * @brief Represents the current state to the outside world. For example printing USART message or turn on led's.
 * @param state Current state
 */
void show_state(track_state *state) {
    switch (state->action) {
        case ROUNDS: {
            uint8_t round = 1;
            switch (state->drive) {
                case SECOND_ROUND:
                    round = 2;
                    break;
                case THIRD_ROUND:
                    round = 3;
                    break;
                default:
                    break;
            }
            // Manuel check, so we don't have to create a pointer every tick
            if (timers_check_state(state, COUNTER_1_HZ)) {
                char s[sizeof("Round and round I go, currently round 1\n")];
                sprintf(s, "Round and round I go, currently round %d\n", round);
                USART_print(s);
            }
//            LED_State ledState = LED_NONE;
//            if ((state->sensor_last) & SENSOR_LEFT) {
//                ledState |= LED_LEFT;
//            }
//            if ((state->sensor_last) & SENSOR_CENTER) {
//                ledState |= LED_CENTER;
//            }
//            if ((state->sensor_last) & SENSOR_RIGHT) {
//                ledState |= LED_RIGHT;
//            }
//            LED_set(ledState);
            //TODO: Remove before deployment
            led_sensor(state->sensor_last);
            break;
        }
        case FROZEN:
            timers_print(state->counters, COUNTER_1_HZ,
                         "In safe state! Won’t react to any instructions! Rescue me!\n");
            if (timers_check_state(state, COUNTER_12_HZ)) {
                led_chase(&(state->last_led));
            }
            break;
        case RETURN_HOME:
            timers_print(state->counters, COUNTER_1_HZ, "Returning home, will reset me there\n");
            break;
        case PAUSE:
            timers_print(state->counters, COUNTER_1_HZ,
                         "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            if (timers_check_state(state, COUNTER_3_HZ)) {
                led_chase(&(state->last_led));
            }
            break;
        case WAIT:
            if (state->pos == POS_START_FIELD) {
                timers_print(state->counters, COUNTER_1_HZ,
                             "On the starting field. Waiting for your instructions... Send ? for help.\n");
                if (timers_check_state(state, COUNTER_5_HZ)) {
                    led_blink(&(state->last_led));
                }
            } else {
                timers_print(state->counters, COUNTER_1_HZ,
                             "Not on the starting field. Place me there please... Send ? for help.\n");
//                LED_State ledState = LED_NONE;
//                if ((state->sensor_last) & SENSOR_LEFT) {
//                    ledState |= LED_LEFT;
//                }
//                if ((state->sensor_last) & SENSOR_CENTER) {
//                    ledState |= LED_CENTER;
//                }
//                if ((state->sensor_last) & SENSOR_RIGHT) {
//                    ledState |= LED_RIGHT;
//                }
//                LED_set(ledState);
                led_sensor(state->sensor_last);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Print help message for the given state. Print different text if we are located on starting field and no at all
 * if the robot was once in the drive state.
 * @param state Current state
 */
void print_help(track_state *state) {
    //Only print help text if S was not received once
    if (state->has_driven_once) {
        return;
    }
    if (state->pos == POS_START_FIELD) {
        USART_print("-X Safe action_state\n-S 3 Rounds\n-P Pause\n-R Reset\n-C Home\n-? Help\n");
    } else {
        USART_print("Not on start field, please position on start field!\n");
    }
}

/**
 * Print fail message if character was not defined for an action.
 *
 * @param byte The character which was not defined.
 */
void print_fail(unsigned char byte) {
    char s [90 * sizeof(char)];// More is better
    sprintf(s, "Received undefined Sign %c\n", byte);
    USART_print(s);
}

/**
 * @brief Applies effects and show state to the outside that depend on the current action.
 */
void on_action_change(track_state *state) {
    switch (state->action) {
        case RESET:
            USART_print("Will reset in 5 seconds...\n");
            break;
        case WAIT:
        case FROZEN:
        case PAUSE:
            //Reset chase or blink light
            state->last_led = 0;
            break;
        case ROUNDS:
            state->has_driven_once = 1;
            break;
        default:
            break;
    }
}

/**
 * @brief Tries to read an input from the USART, apply the action behind the character if any is defined, send an
 * error message for undefined characters.
 * @details Defined characters are: S, X; P, C, R, ?
 *
 * @param state Internal state
 */
void read_input(track_state *state) {
    if (!USART_canReceive()) {
        return;
    }
    if (state->action == FROZEN) {
        return;
    }
    unsigned char byte = USART_receiveByte();
    switch (byte) {
        case 'S':
            state->action = ROUNDS;
            break;
        case 'X':
            state->action = FROZEN;
            break;
        case 'P':
            if (state->action == PAUSE) {
                state->action = ROUNDS;
                break;
            }
            if (state->action != ROUNDS) {
                return;
            }
            state->action = PAUSE;
            break;
        case 'C':
            state->action = RETURN_HOME;
            break;
        case 'R':
            state->action = RESET;
            break;
        case '?':
            print_help(state);
            return;
        default:
            print_fail(byte);
            return;
    }
    on_action_change(state);
}

/**
 * @brief Updates position of the state. Checks if the robot is: "on the start",
 * "on the track (if already started driving)" or "unknown (on the track but not started)"
 *
 * @param trackState The currently used state
 */
void update_position(track_state *trackState) {
    if (timers_check_state(trackState, COUNTER_4_HZ)) {
        trackState->last_pos = trackState->pos;
        // All sensors on, could be home field
        if (trackState->sensor_last == SENSOR_ALL) {
            trackState->homeCache++;
            if (trackState->homeCache > 2) {
                if (trackState->pos != POS_START_FIELD) {
                    USART_print("Start field found \n");
                }
                trackState->pos = POS_START_FIELD;
                trackState->homeCache = 2;
            }else{
                char s[sizeof("Start field tick 1\n")];
                sprintf(s, "Start field tick %d\n", trackState->homeCache);
                USART_print(s);
            }
            return;
        }
        trackState->homeCache = 0;
        if (trackState->action == ROUNDS) {
            if((trackState->pos) != POS_TRACK) {
                USART_print("Start field lost \n");
            }
            trackState->pos = POS_TRACK;
        } else {
            trackState->pos = POS_UNKNOWN;
        }
    }
}

/**
 * @brief Run loop of the script
 */
void runLoop(void) {
    //Create track state
    track_state *trackState = malloc(sizeof(track_state));
    trackState->drive = FIRST_ROUND;
    trackState->action = ROUNDS;
    trackState->pos = POS_UNKNOWN;
    trackState->last_pos = POS_UNKNOWN;
    trackState->homeCache = 0;
    // Create counters, has to be done before first use
    timers_create(trackState->counters);
    while (1) {
        //read_input(trackState);
        show_state(trackState);
        update_position(trackState);
        timers_update(trackState->counters);
        action_state oldAction = trackState->action;
        switch (oldAction) {
            case ROUNDS: {
                drive_run(trackState);
                break;
            }
            case RESET: {
                reset();
                break;
            }
            default:
                //Do nothing
                break;
        }
        /**
         * If the state changes an action was applied, usually when the 3 rounds were driven and the robot is switched to
         * reset
         */
        if (oldAction != trackState->action) {
            if (oldAction == ROUNDS) {
                motor_drive_stop();
            }
            on_action_change(trackState);
        }
    }
    free(trackState);
}

int main(void) {
    setup();

    motor_set_speed(SPEED_MIDDLE, SPEED_MIDDLE);

    runLoop();
}
