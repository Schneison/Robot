#include <avr/io.h>
#include "utility.h"
#include <util/delay.h>
#include "iesusart.h"
#include "robot_sensor.h"
#include "drive_control.h"
#include "brain.h"
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
 * @brief Drive directions
 */
typedef enum {
/**
 * @brief No direction, don't drive
 */
    DIR_NONE,
/**
 * @brief Drive straight forward
 */
    DIR_FORWARD,
/**
 * @brief Turn right
 */
    DIR_RIGHT,
/**
 * @brief Turn left
 */
    DIR_LEFT,
} direction;

/**
 * @brief Reads sensor input and evaluates the direction that the robot has to drive.
 * @param current Current sensor state
 * @param last Sensor state in the last cycle
 */
direction evaluate_sensors(sensor_state current, sensor_state last) {
    if ((current & SENSOR_CENTER) && ((current & SENSOR_LEFT) && (current & SENSOR_RIGHT) ||
                                      !(current & SENSOR_LEFT) && !(current & SENSOR_RIGHT))) {
        return DIR_FORWARD;
    }
        // Right Sensor
    else if ((current & SENSOR_RIGHT)) {
        return DIR_RIGHT;
    }
        // Left Sensor
    else if ((current & SENSOR_LEFT)) {
        return DIR_LEFT;
    }
    return DIR_NONE;
}

/**
 * @brief Perform driving of the robot
 *
 * @param current Current sensor state
 */
void driveDo(sensor_state current, sensor_state last) {
    direction dir = evaluate_sensors(current, last);
    switch (dir) {
        case DIR_FORWARD:
            motor_drive_forward();
            break;
        case DIR_RIGHT:
            motor_drive_right();
            break;
        case DIR_LEFT:
            motor_drive_left();
            break;
        default:
            break;
    }
}

/**
 * Performance the driving action
 *
 * @param state Current state
 */
void drive(track_state *state) {
    sensor_state current = sensor_get();

    switch (state->drive) {
        case CHECK_START:
            //When on start field begin first round
            if (state->pos == POS_START_FIELD) {
                state->drive = FIRST_ROUND;
                USART_print("Here I go again on my own, going down the only round I’ve ever known…\n");
            }
            break;
        case FIRST_ROUND:
        case SECOND_ROUND:
        case THIRD_ROUND:
            // Check if we were on track before and are now on the start field, WE DID A ROUND
            if (state->last_pos == POS_TRACK && state->pos == POS_START_FIELD) {
                switch (state->drive) {
                    case FIRST_ROUND:
                        USART_print("YEAH, done round 1, going for round 2/3\n");
                        state->drive = SECOND_ROUND;
                        break;
                    case SECOND_ROUND:
                        USART_print("YEAH YEAH, done round 2, going for round 3/3\n");
                        state->drive = THIRD_ROUND;
                        break;
                    case THIRD_ROUND:
                        USART_print("YEAH YEAH YEAH , I really did it my way. ... And what’s my purpose\n"
                                    "and the general sense of my further life now? Type ? for help\n");
                        state->drive = POST_DRIVE;
                        break;
                    default:
                        //Should never happen
                        break;
                }
            }
            driveDo(current, state->sensor_last);
            break;
        case POST_DRIVE:
            reset();
            break;
        case PRE_DRIVE:
            break;
    }

    //driveDo(current);
    state->sensor_last = current;
}

/**
 * @brief Prints then given message if the frequency requirement is currently meed.
 *
 * @param frequency Frequency on which the given text should be printed.
 * @param text The text that should be printed
 */
void print_at_freq(track_state *state, counter_def frequency, const char *text) {
    if (check_state_counter(state, frequency)) {
        USART_print(text);
    }
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
                    round = 2;
                    break;
                default:
                    break;
            }
            // Manuel check, so we don't have to create a pointer every tick
            if (check_state_counter(state, COUNTER_1_HZ)) {
                char *s = malloc(sizeof("Round and round I go, currently round 1\n"));
                sprintf(s, "Round and round I go, currently round %d\n", round);
                USART_print(s);
                free(s);
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
            print_at_freq(state, COUNTER_1_HZ, "In safe state! Won’t react to any instructions! Rescue me!\n");
            break;
        case RETURN_HOME:
            print_at_freq(state, COUNTER_1_HZ, "Returning home, will reset me there\n");
            break;
        case PAUSE:
            print_at_freq(state, COUNTER_1_HZ, "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            break;
        case WAIT:
            if (state->pos == POS_START_FIELD) {
                print_at_freq(state, COUNTER_1_HZ, "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again\n");
            } else {
                print_at_freq(state, COUNTER_1_HZ,
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
    char *s = malloc(156 * sizeof(char));
    sprintf(s, "Received undefined Sign %c\n", byte);
    USART_print(s);
    free(s);
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
            USART_print("Will reset in 5 seconds...\n");
            break;
        case '?':
            print_help(state);
            return;
        default:
            print_fail(byte);
            return;
    }
}

/**
 * @brief Updates position of the state. Checks if the robot is: "on the start",
 * "on the track (if already started driving)" or "unknown (on the track but not started)"
 *
 * @param trackState The currently used state
 */
void update_position(track_state *trackState) {
    if (check_state_counter(trackState, COUNTER_5_HZ)) {
        trackState->last_pos = trackState->pos;
        // All sensors on, could be home field
        if (trackState->sensor_last == SENSOR_ALL) {
            trackState->homeCache++;
            if (trackState->homeCache > 2) {
                trackState->pos = POS_START_FIELD;
                trackState->homeCache = 2;
                return;
            }
        }
        trackState->homeCache = 0;
        if (trackState->action == ROUNDS) {
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
    while (1) {
        //read_input(trackState);
        show_state(trackState);
        update_position(trackState);
        update_counters(trackState->counters);
        switch (trackState->action) {
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
    }
    free(trackState);
}

int main(void) {
    setup();

    motor_set_speed(SPEED_MIDDLE, SPEED_MIDDLE);

    runLoop();
}
