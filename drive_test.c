#include <avr/io.h>
#include "iesmotors.h"
#include <util/delay.h>
#include "iesusart.h"

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

int main(void) {
	setup();
	
    // Set the duty cycles for PD5/PD6
    setDutyCycle(PD5, 155);
    setDutyCycle(PD6, 155);
    
    unsigned char lastLeft = 0;
    unsigned char lastMiddle = 0;
    unsigned char lastRight = 0;
	unsigned char changeLeft = 0;
    unsigned char changeMiddle = 0;
    unsigned char changeRight = 0;
    unsigned char oneActive = 0;
    
    while(1){
		//Reset
		PORTB &= ~(1 << PB0);
		PORTB &= ~(1 << PB1);
		PORTB &= ~(1 << PB3);
		PORTD &= ~(1 << PD7);
		changeMiddle = 0;
		changeLeft = 0;
		changeRight = 0;
		oneActive = (IR_LF_R & (1 << IP_LF_R)) | (IR_LF_M & (1 << IP_LF_M)) | (IR_LF_L & (1 << IP_LF_L));
		if(oneActive){
			if(lastMiddle != (IR_LF_M & (1 << IP_LF_M))) {
				changeMiddle = 1;
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
		// Right Sensor
		if(IR_LF_R & (1 << IP_LF_R) || lastRight) {
			PORTD |= (1 << PD7); //Left Forward
			PORTB |= (1 << PB1); //Right Backward
		}
		
		// Middle Sensor
		if (IR_LF_M & (1 << IP_LF_M)) {
			PORTD |= (1 << PD7);
			PORTB |= (1 << PB3);	
		}
		
		// Left Sensor
		if (IR_LF_L & (1 << IP_LF_L) || lastLeft) {
			PORTB |= (1 << PB0); //Left Backward
			PORTB |= (1 << PB3); //Right Forward		
		}
		if(oneActive){
			lastLeft = IR_LF_L & (1 << IP_LF_L);
			lastMiddle = IR_LF_M & (1 << IP_LF_M);
			lastRight = IR_LF_R & (1 << IP_LF_R);
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
