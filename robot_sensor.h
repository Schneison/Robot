#ifndef RO_SIGNALS
#define RO_SIGNALS

// ADC Channels
#define ADMUX_CHN_ADC0 0 // 0000 0000
#define ADMUX_CHN_ADC1 1 // 0000 0001
#define ADMUX_CHN_ADC2 2 // 0000 0010
#define ADMUX_CHN_ADC3 3 // 0000 0011
#define ADMUX_CHN_ALL 7  // 0000 0111

// Average some measurements to reduce probable noise.
#define ADC_AVG_WINDOW 20

// Seonsor state
#define STATE_LOW 0
#define STATE_MIDDLE 1
#define STATE_HIGH 2

#define SIGNAL_RIGHT_UPPER 600
#define SIGNAL_CENTER_UPPER 600
#define SIGNAL_LEFT_UPPER 600

uint16_t ADC_read(uint8_t channel);

uint16_t ADC_read_avg(uint8_t channel, uint8_t nsamples);

uint8_t left_state();

uint8_t center_state();

uint8_t right_state();

void ADC_init(void);

#endif
