// GPIO LED demo

#include <stdint.h>

#include "consoleUtils.h"

#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"

#include "leds.h"

/*****************************************************************************
 **                INTERNAL MACRO DEFINITIONS
 *****************************************************************************/
#define LED_GPIO_BASE           (SOC_GPIO_1_REGS)

/******************************************************************************
 **              INTERNAL VARIABLES
 ******************************************************************************/

static const uint32_t const LED_PINS[] = {21, 22, 23 ,24};
static bool ledStatus[] = {false, false, false, false};

/******************************************************************************
 **              FUNCTION DEFINITIONS
 ******************************************************************************/

void leds_init(void)
{
	uint32_t led = 0;
	/* Enabling functional clocks for GPIO1 instance. */
	GPIO1ModuleClkConfig();

	/* Selecting GPIO1[23] pin for use. */
	//GPIO1Pin23PinMuxSetup();

	/* Enabling the GPIO module. */
	GPIOModuleEnable(LED_GPIO_BASE);

	/* Resetting the GPIO module. */
	GPIOModuleReset(LED_GPIO_BASE);

	/* Setting the GPIO pin as an output pin. */
	// GPIODirModeSet(LED_GPIO_BASE, 21, GPIO_DIR_OUTPUT);
	for(led = 0; led < N_LEDS; led++) {
		GPIODirModeSet(LED_GPIO_BASE, LED_PINS[led], GPIO_DIR_OUTPUT);
	}
}

bool leds_isOn(enum Led led)
{
	if(led < LED0 || N_LEDS <= led) {
		return false;
	}

	return ledStatus[led];
}

void leds_turnOn(enum Led led)
{
	if(led < LED0 || N_LEDS <= led) {
		return;
	}

	ledStatus[led] = true;
	GPIOPinWrite(LED_GPIO_BASE, LED_PINS[led], GPIO_PIN_HIGH);
}

void leds_turnOff(enum Led led)
{
	if(led < LED0 || N_LEDS <= led) {
		return;
	}

	ledStatus[led] = false;
	GPIOPinWrite(LED_GPIO_BASE, LED_PINS[led], GPIO_PIN_LOW);
}