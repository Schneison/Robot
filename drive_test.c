#include <avr/io.h>
#include "iesmotors.h"
#include <util/delay.h>
#include "iesusart.h"
#include "robot_sensor.h"

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
//IN2 Left Backward
//IN4 Right Forward
//IN3 Right Backward

void setup(void) {
	USART_init(UBRR_SETTING);
	ADC_init();
	
	// Delete everything on ports B and D
    DDRD = 0;
    DDRB = 0;
    
    // Set PD5 and PD6 as output (EN[A|B]!)
    DDRD = (1 << DD5) | (1 << DD6);
    
    // Set PD7 as output (IN1)
    DDRD |= (1 << DD7);

    // Make PWM work on PD[5|6]
    setupTimer0();

    // Set PB0, PB1, and PB3 as output (IN[2|3|4])
    DDRB = (1 << DD0) | (1 << DD1) | (1 << DD3); 
}

void set_speed(uint8_t a, uint8_t b) {
	setDutyCycle(PD5, a == STATE_HIGH ? 180 : a == STATE_MIDDLE ? 130 : 80); // left
	setDutyCycle(PD6, b == STATE_HIGH ? 180 : b == STATE_MIDDLE ? 130 : 80); // right
}

void clear(void) {
	//Reset
	PORTB &= ~(1 << PB0);
	PORTB &= ~(1 << PB1);
	PORTB &= ~(1 << PB3);
	PORTD &= ~(1 << PD7);
}

void driveRight(void) {
	set_speed(STATE_LOW, STATE_HIGH);
	PORTD |= (1 << PD7); //Left Forward
	PORTB |= (1 << PB1); //Right Backward
	PORTB &= ~(1 << PB0);
	PORTB &= ~(1 << PB3);
}

void driveMiddle(void) {
	set_speed(STATE_MIDDLE, STATE_MIDDLE);
	PORTD |= (1 << PD7);
	PORTB |= (1 << PB3);
	PORTB &= ~(1 << PB0);
	PORTB &= ~(1 << PB1);
}

void driveLeft(void) {
	set_speed(STATE_HIGH, STATE_LOW);
	PORTB |= (1 << PB0); //Left Backward
	PORTB |= (1 << PB3); //Right Forward	
	PORTB &= ~(1 << PB1);
	PORTD &= ~(1 << PD7);	
}

int main(void) {
	setup();
	
    // Set the duty cycles for PD5/PD6
    setDutyCycle(PD5, 155); // left
    setDutyCycle(PD6, 155); // right
    
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
			driveRight();
		}
		
		// Center Sensor
		if (center_sensor && !right_sensor && !left_sensor) {
			driveMiddle();
		}
		
		// Left Sensor
		if (left_sensor && !center_sensor && !right_sensor) {
			driveLeft();
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
