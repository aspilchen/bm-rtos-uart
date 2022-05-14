// GPIO Button demo
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"
#include "uart_irda_cir.h"
#include "consoleUtils.h"
#include <stdint.h>

#include "joystick.h"

/*****************************************************************************
**                INTERNAL MACRO DEFINITIONS
*****************************************************************************/
// Boot btn on BBB: SOC_GPIO_2_REGS, pin 8
// Down on Zen cape: SOC_GPIO_1_REGS, pin 14  NOTE: Must change other "2" constants to "1" for correct initialization.
// Left on Zen cape: SOC_GPIO_2_REGS, pin 1
// --> This code uses left on the ZEN:
#define BUTTON_GPIO_BASE     (SOC_GPIO_2_REGS)
#define BUTTON_PIN           (1)

// P8_14 10 0x828/028 26 GPIO0_26 gpio0[26] UP
// P8_16 14 0x838/038 46 GPIO1_14 gpio1[14] DOWN

/*****************************************************************************
**                INTERNAL VARIABLES
*****************************************************************************/
static const uint32_t const GPIO_BASES[] = {SOC_GPIO_0_REGS, SOC_GPIO_1_REGS};
static const uint8_t const GPIO_PINS[] = {26, 14};

/*****************************************************************************
**                FUNCTION DEFINITIONS
*****************************************************************************/
void joystick_init(void)
{
      uint32_t dir = 0;
      GPIO0ModuleClkConfig();
      GPIO1ModuleClkConfig();
      for (dir = 0; dir < JOY_N_DIR; dir++) {
            GPIOModuleEnable(GPIO_BASES[dir]);
            GPIOModuleReset(GPIO_BASES[dir]);
            GPIODirModeSet(GPIO_BASES[dir], GPIO_PINS[dir], GPIO_DIR_INPUT);
      }
}

bool joystick_isPressed(enum JoystickDirection dir)
{
      return GPIOPinRead(GPIO_BASES[dir], GPIO_PINS[dir]) == 0;
}

/*****************************************************************************
**                INTERNAL FUNCTION DEFINITIONS
*****************************************************************************/

static _Bool readButtonWithStarteWare(void)
{
}

static _Bool readButtonWithBitTwiddling(void)
{
	uint32_t regValue = HWREG(BUTTON_GPIO_BASE + GPIO_DATAIN);
	uint32_t mask     = (1 << BUTTON_PIN);

	return (regValue & mask) == 0;
}