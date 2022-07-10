#include <avr/io.h>
#include "iesmotors.h"
#include <util/delay.h>
#include "iesusart.h"
#include "robot_sensor.h"
#include "robot_controller.h"
#include "brain.h"
#include "led_control.h"
#include <stdio.h>
#include <stdlib.h>

void setup(void) {
    motor_clear();
    ADC_clear();

    USART_init(UBRR_SETTING);
	ADC_init();
    motor_init();
}

void drive(track_state* state) {
    sensor_state current = sensor_get();

    // Right Sensor
    if((current & SENSOR_RIGHT)) {
        motor_drive_right();
    }

    // Center Sensor
    if ((current & SENSOR_CENTER)) {
        motor_drive_forward();
    }

    // Left Sensor
    if ((current & SENSOR_LEFT)) {
        motor_drive_left();
    }
    state->sensor_last = current;
}

void print_at_freq(uint8_t frequency, const char* string){
    if(check_freq(frequency)){
        USART_print(string);
    }
}

void show_state(track_state* state){
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
            if (check_freq(1)) {
                char *s = malloc(sizeof("Round and round I go, currently round 1"));
                sprintf(s, "Received undefined Sign %c", round);
                USART_print(s);
                free(s);
            }
        }
        case FROZEN:
            print_at_freq(1, "In safe state! Won’t react to any instructions! Rescue me!");
            break;
        case RETURN_HOME:
            print_at_freq(1, "Returning home, will reset me there");
            break;
        case PAUSE:
            print_at_freq(1, "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again");
            break;
        case WAIT:
            if (state->pos == POS_START_FIELD) {
                print_at_freq(1, "Pause .... zzzZZZzzzZZZzzz .... wake me up with P again");
            }else{
                print_at_freq(1, "Not on the starting field. Place me there please... Send ? for help.");
                LED_State ledState = LED_NONE;
                if(state->sensor_last == SENSOR_LEFT) {
                    ledState |= LED_LEFT;
                }
                if(state->sensor_last == SENSOR_CENTER) {
                    ledState |= LED_CENTER;
                }
                if(state->sensor_last == SENSOR_RIGHT) {
                    ledState |= LED_RIGHT;
                }
                LED_set(ledState);
            }
            break;
        default:
            break;
    }
}

void print_help(track_state* state){
    if(state->pos == POS_START_FIELD) {
        USART_print("-X Safe action_state\n-S 3 Rounds\n-P Pause\n-R Reset\n-C Home\n-? Help");
    }else{
        USART_print("Not on start field, please position on start field!");
    }
}

void print_fail(unsigned char byte){
    char *s = malloc(155 * sizeof(char));
    sprintf(s, "Received undefined Sign %c", byte);
    USART_print(s);
    free(s);
}

void on_action_change(action_state newAction){
    switch (newAction) {
        case RESET:
            USART_print("Will reset in 5 seconds...");
            break;
        default:
            break;
    }
}

void read_input(track_state* state) {
    if(!USART_canReceive()){
        return;
    }
    unsigned char byte = USART_receiveByte();
    switch (byte) {
        case 'S':
            state->action=ROUNDS;
            break;
        case 'X':
            state->action=FROZEN;
            break;
        case 'P':
            state->action=PAUSE;
            break;
        case 'C':
            state->action=RETURN_HOME;
            break;
        case 'R':
            state->action=RESET;
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
void update_position(track_state* trackState){
    if(check_freq(1)){
        // All sensors on, could be home field
        if(trackState->sensor_last == SENSOR_ALL){
            trackState->homeCache++;
            if(trackState->homeCache > 2){
                trackState->pos=POS_START_FIELD;
                trackState->homeCache = 2;
                return;
            }
        }
    }
    if(trackState->action==ROUNDS){
        trackState->pos=POS_TRACK;
    }else{
        trackState->pos=POS_UNKNOWN;
    }
}

void run(void) {
    track_state* trackState = malloc(sizeof(track_state));
    trackState->drive=PRE_DRIVE;
    trackState->action=ROUNDS;
    trackState->pos=POS_UNKNOWN;
    trackState->homeCache=0;
    while(1){
        read_input(trackState);
        show_state(trackState);
        switch (trackState->action) {
            case ROUNDS: {
                drive(trackState);
            }
            case RESET: {
                reset();
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
	
    // Set the duty cycles for PD5/PD6
    //setDutyCycle(PD5, 155); // left
    //setDutyCycle(PD6, 155); // right
    motor_set_speed(STATE_MIDDLE, STATE_MIDDLE);
    
    unsigned char lastLeft = 0;
    unsigned char lastCenter = 0;
    unsigned char lastRight = 0;
	unsigned char changeLeft = 0;
    unsigned char changeCenter = 0;
    unsigned char changeRight = 0;
    unsigned char oneActive = 0;
    unsigned char left_sensor = 0;
    unsigned char center_sensor = 0;
    unsigned char right_sensor = 0;
    
    while(1){
		changeCenter = 0;
		changeLeft = 0;
		changeRight = 0;
		/*oneActive = (IR_LF_R & (1 << IP_LF_R)) | (IR_LF_M & (1 << IP_LF_M)) | (IR_LF_L & (1 << IP_LF_L));
		if(oneActive){
			if(lastCenter != (IR_LF_M & (1 << IP_LF_M))) {
				changeCenter = 1;
				USART_print("Last middle\n\n");
			}
			if(lastLeft != (IR_LF_L & (1 << IP_LF_L))) {
				changeLeft = 1;
				USART_print("Last left\n\n");
			}
			if(lastRight != (IR_LF_R & (1 << IP_LF_R))) {
				changeRight = 1;
				USART_print("Last right\n\n");
			}
		}*/
		
		right_sensor = right_state();
		left_sensor = left_state();
		center_sensor = center_state();
		
		// Right Sensor
		if(right_sensor && !center_sensor && !left_sensor) {
            motor_drive_right();
		}
		
		// Center Sensor
		if (center_sensor && !right_sensor && !left_sensor) {
            motor_drive_forward();
		}
		
		// Left Sensor
		if (left_sensor && !center_sensor && !right_sensor) {
            motor_drive_left();
		}
		if(oneActive){
			lastLeft = left_state();
			lastCenter = center_state();
			lastRight = right_state();
		}
	}
	
	// Set IN1 to HIGH and don't set IN2 to HIGH (leave LOW) -> Left motors FORWARD
    //PORTD |= (1 << PD7); // Use OR, since overwriting will disable EN[A|B]!
    
    // Set IN2 to HIGH and don't set anything else to HIGH -> Left motors BACKWARD
    //PORTB |= (1 << PB0);
	
	// Set IN4 to HIGH and don't set anything else to HIGH -> Right motors FORWARD
    //PORTB |= (1 << PB3);
	
	// Set IN3 to HIGH and don't set anything else to HIGH -> Right motors BACKWARD
    //PORTB |= (1 << PB1);
}
