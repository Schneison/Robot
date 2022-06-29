#include <avr/io.h>
#include "iesmotors.h"
#include <util/delay.h>
#include "iesusart.h"

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

void clear(void) {
	//Reset
	PORTB &= ~(1 << PB0);
	PORTB &= ~(1 << PB1);
	PORTB &= ~(1 << PB3);
	PORTD &= ~(1 << PD7);
}
	
void diveRight(void) {
	PORTD |= (1 << PD7); //Left Forward
	PORTB |= (1 << PB1); //Right Backward
	PORTB &= ~(1 << PB0);
	PORTB &= ~(1 << PB3);
}

void driveMiddle(void) {
	PORTD |= (1 << PD7);
	PORTB |= (1 << PB3);
	PORTB &= ~(1 << PB0);
	PORTB &= ~(1 << PB1);
}

void driveLeft(void) {
	PORTB |= (1 << PB0); //Left Backward
	PORTB |= (1 << PB3); //Right Forward	
	PORTB &= ~(1 << PB1);
	PORTB &= ~(1 << PD7);	
}
