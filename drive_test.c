#include <avr/io.h>
#include "iesmotors.h"
#include <util/delay.h>
#include "iesusart.h"
#include "robot_sensor.h"
#include "robot_controller.h"
#include "brain.h"
#include <stdio.h>
#include <stdlib.h>

void setup(void) {
    motor_clear();
    ADC_clear();

    USART_init(UBRR_SETTING);
	ADC_init();
    motor_init();
}

void drive(SensorState* lastState) {
    SensorState current = sensor_get();

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
    *lastState=current;
}

void print(State state){

}

void print_help(void){
    USART_print("-X Safe State\n-S 3 Rounds\n-P Pause\n-R Reset\n-C Home\n-? Help");
}

void read_input(State* state) {
    if(!USART_canReceive()){
        return;
    }
    unsigned char byte = USART_receiveByte();
    switch (byte) {
        case 'S':
            *state=ROUNDS;
            return;
        case 'X':
            *state=FROZEN;
            return;
        case 'P':
            *state=PAUSE;
            return;
        case 'C':
            *state=RETURN_HOME;
            return;
        case 'R':
            *state=RESET;
            return;
        case '?':
            print_help();
            return;
        default:
            break;
    }
    char *s = malloc(155 * sizeof(char));
    sprintf(s, "Received undefined Sign %c", byte);
    USART_print(s);
}

void run(void) {
    State state = ROUNDS;
    SensorState lastState = 0;
    while(1){
        read_input(&state);
        print(state);
        switch (state) {
            case ROUNDS: {
                drive(&lastState);
            }
            default:
                break;
        }
    }
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
