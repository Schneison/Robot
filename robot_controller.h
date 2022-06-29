#ifndef RO_DRIVE
#define RO_DRIVE

//Right LF
#define DR_ADC0 DDRC
#define DP_ADC0 DDC0
#define IR_ADC0 PC0
#define IP_ADC0 PC0

// Center LF
#define DR_ADC1 DDRC
#define DP_ADC1 DDC1
#define IR_ADC1 PC1
#define IP_ADC1 PC1

// Left LF
#define DR_ADC2 DDRC
#define DP_ADC2 DDC2
#define IR_ADC2 PC2
#define IP_ADC2 PC2

void clear(void);

void set_speed(uint8_t, uint8_t);

void diveRight(void);

void driveMiddle(void);

void driveLeft(void);

#endif
