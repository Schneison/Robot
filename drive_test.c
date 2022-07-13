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
    LED_init();
    setupCountTimer();
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
            if (check_state_counter(state, COUNTER_1_HZ)) {
                char s [sizeof("Round and round I go, currently round 1\n")];
                sprintf(s, "Round and round I go, currently round %d\n", round);
                USART_print(s);
            }
            LED_State ledState = LED_NONE;
            if ((state->sensor_last) & SENSOR_LEFT) {
                ledState |= LED_LEFT;
            }
            if ((state->sensor_last) & SENSOR_CENTER) {
                ledState |= LED_CENTER;
            }
            if ((state->sensor_last) & SENSOR_RIGHT) {
                ledState |= LED_RIGHT;
            }
            LED_set(ledState);
            break;
        }
        case FROZEN:
            print_at_freq(state->counters, COUNTER_1_HZ, "In safe state! Won’t react to any instructions! Rescue me!\n");
            break;
        case RETURN_HOME:
            print_at_freq(state->counters, COUNTER_1_HZ, "Returning home, will reset me there\n");
            break;
        case PAUSE:
            print_at_freq(state->counters, COUNTER_1_HZ, "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            break;
        case WAIT:
            if (state->pos == POS_START_FIELD) {
                print_at_freq(state->counters, COUNTER_1_HZ, "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            } else {
                print_at_freq(state->counters, COUNTER_1_HZ,
                              "Not on the starting field. Place me there please... Send ? for help.\n");
                LED_State ledState = LED_NONE;
                if ((state->sensor_last) & SENSOR_LEFT) {
                    ledState |= LED_LEFT;
                }
                if ((state->sensor_last) & SENSOR_CENTER) {
                    ledState |= LED_CENTER;
                }
                if ((state->sensor_last) & SENSOR_RIGHT) {
                    ledState |= LED_RIGHT;
                }
                LED_set(ledState);
            }
            break;
        default:
            break;
    }
}

/**
 * @brief Print help message for the given state. Print different if we are located on starting field.
 * @param state Current state
 */
void print_help(track_state *state) {
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
 * @param newAction The action to apply.
 */
void on_action_change(action_state newAction){
    switch (newAction) {
        case RESET:
            USART_print("Will reset in 5 seconds...\n");
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
    unsigned char byte = USART_receiveByte();
    switch (byte) {
        case 'S':
            state->action = ROUNDS;
            break;
        case 'X':
            state->action = FROZEN;
            break;
        case 'P':
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
    on_action_change(state->action);
}

/**
 * @brief Updates position of the state. Checks if the robot is: "on the start",
 * "on the track (if already started driving)" or "unknown (on the track but not started)"
 *
 * @param trackState The currently used state
 */
void update_position(track_state *trackState) {
    if (check_state_counter(trackState, COUNTER_3_HZ)) {
        trackState->last_pos = trackState->pos;
        // All sensors on, could be home field
        if (trackState->sensor_last == SENSOR_ALL) {
            trackState->homeCache++;
            if (trackState->homeCache > 2) {
                if(trackState->pos != POS_START_FIELD) {
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
    track_state *trackState = malloc(sizeof(track_state));
    trackState->drive = FIRST_ROUND;
    trackState->action = ROUNDS;
    trackState->pos = POS_UNKNOWN;
    trackState->last_pos = POS_UNKNOWN;
    trackState->homeCache = 0;
    init_counters(trackState->counters);
    while (1) {
        //read_input(trackState);
        show_state(trackState);
        update_position(trackState);
        update_counters(trackState->counters);
        action_state oldAction = trackState->action;
        switch (oldAction) {
            case ROUNDS: {
                drive(trackState);
                break;
            }
            case RESET: {
                reset();
                break;
            }
            case PAUSE:
                //Do nothing, we pause until action changes
                break;
            default:
                break;
        }
        if(oldAction != trackState->action){
            on_action_change(trackState->action);
        }
    }
    free(trackState);
}

int main(void) {
    setup();

    motor_set_speed(SPEED_MIDDLE, SPEED_MIDDLE);

    runLoop();
}
