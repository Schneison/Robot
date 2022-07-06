#include <avr/io.h>
#include "iesmotors.h"
#include <util/delay.h>
#include "iesusart.h"
#include "robot_sensor.h"
#include "robot_controller.h"

// Direction Register = DR
// Input Register = IR
// Output Registter = OR

// Robot function/peripheral RIGHT LF.
#define DR_LF_R DDRC
#define DP_LF_R DDC0
#define IR_LF_R PINC
#define IP_LF_R PINC0

// Robot function/peripheral MIDDLE LF.
#define DR_LF_M DDRC
#define DP_LF_M DDC1
#define IR_LF_M PINC
#define IP_LF_M PINC1

// Robot function/peripheral LEFT LF.
#define DR_LF_L DDRC
#define DP_LF_L DDC2
#define IR_LF_L PINC
#define IP_LF_L PINC2

//IN1 Left Forward
#define DR_M_LF DDRD
#define DP_M_LF DD7
#define OR_M_LF PORTD
#define OP_M_LF PD7

//IN2 Left Backward
#define DR_M_LB DDRB
#define DP_M_LB DD0
#define OR_M_LB PORTB
#define OP_M_LB PB0

//IN4 Right Forward
#define DR_M_RF DDRB
#define DP_M_RF DD3
#define OR_M_RF PORTB
#define OP_M_RF PB3

//IN3 Right Backward
#define DR_M_RB DDRB
#define DP_M_RB DD1
#define OR_M_RB PORTB
#define OP_M_RB PB1

// Left Enable
#define DR_M_LE DDRD
#define DP_M_LE DD5
#define OR_M_LE PORTD
#define OP_M_LE PD5

// Right Enable Motor
#define DR_M_RE DDRD
#define DP_M_RE DD6
#define OR_M_RE PORTD
#define OP_M_RE PD6


void setup(void) {
    motor_clear();

    USART_init(UBRR_SETTING);
	ADC_init();
    motor_init();
}

int main(void) {
	setup();
	
    // Set the duty cycles for PD5/PD6
    //setDutyCycle(PD5, 155); // left
    //setDutyCycle(PD6, 155); // right
    set_speed(STATE_MIDDLE, STATE_MIDDLE);
    
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
		oneActive = (IR_LF_R & (1 << IP_LF_R)) | (IR_LF_M & (1 << IP_LF_M)) | (IR_LF_L & (1 << IP_LF_L));
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
		}
		
		right_sensor = right_state();
		left_sensor = left_state();
		center_sensor = center_state();
		
		// Right Sensor
		if(right_sensor && !center_sensor && !left_sensor) {
			drive_right();
		}
		
		// Center Sensor
		if (center_sensor && !right_sensor && !left_sensor) {
			drive_forward();
		}
		
		// Left Sensor
		if (left_sensor && !center_sensor && !right_sensor) {
			drive_left();
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
