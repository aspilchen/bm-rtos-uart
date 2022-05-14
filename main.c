// Light Bouncer
// Creates a bouncing light affect with the LEDS.
// Users can control the LED "bounce" speed via serial port or Zen Cape joystick.

// std
#include <stdint.h>

// Starterware
#include "consoleUtils.h"

// My hardware abstraction
#include "serial.h"
#include "ui.h"
#include "prm.h"
#include "wdtimer.h"
#include "timer.h"
#include "leds.h"
#include "joystick.h"

/******************************************************************************
 **              INTERNAL FUNCTIONS
 ******************************************************************************/
static void init(void)
{
	Serial_init();
	ui_init();
	joystick_init();
	Timer_init();
	leds_init();
	Watchdog_init();
}

static void main_handleWD(void)
{
	if(Timer_isIsrFlagSet()) {
		Timer_clearIsrFlag();
		Watchdog_hit();
	}
}

/******************************************************************************
 **              MAIN
 ******************************************************************************/
int main(void)
{
	init();
	ui_greet();
	ui_listResetEvents();
	ui_cmdHelp(0);
	prm_clear();
	while(1) {
		ui_doWork();
		main_handleWD();
	}
}