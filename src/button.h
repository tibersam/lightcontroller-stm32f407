#ifndef BUTTON_HEADER
#define BUTTON_HEADER


void setup_gpio_button(void);

int get_white(void);

uint64_t get_white_tick(void);

int get_button1(void);

int get_button2(void);

void set_led_button1(void);

void clear_led_button1(void);

void set_led_button2(void);

void clear_led_button2(void);

void process_button1(void);

void process_button(void);


#endif
