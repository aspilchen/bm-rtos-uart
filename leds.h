// leds.h
// Hardware abstraction module: BBG Led control

#ifndef LEDS_H
#define LEDS_H

#include <stdbool.h>

enum Led {
	LED0
	,LED1
	,LED2
	,LED3
	,N_LEDS
};

void leds_init(void);
bool leds_isOn(enum Led led);
void leds_turnOn(enum Led led);
void leds_turnOff(enum Led led);

#endif