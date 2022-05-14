#include <stdint.h>
#include <stdbool.h>

#include "consoleUtils.h"

#include "ui.h"
#include "prm.h"
#include "serial.h"
#include "leds.h"
#include "joystick.h"
#include "timer.h"
#include "wdtimer.h"

/******************************************************************************
 **              INTERNAL FUNCTION PROTOTYPES
 ******************************************************************************/
static void           ui_serialISR         (uint8_t input);
static enum CmdOpCode ui_parseInput        (void);
static void           ui_handleSerialInput (void);
static void           ui_handleJoystick    (void);
static enum Led       ui_nextLed           (enum Led led);
static void           ui_handleLedDisplay  (void);
static void           ui_clockIncrement    (void);
static void           ui_clockReset        (void);
static void           ui_dmTimerISRCallback(void);

/******************************************************************************
 **              INTERNAL VARIABLES
 ******************************************************************************/
static uint8_t serialInputByte = 0;
static uint8_t ledFlashSpeed   = 0;
static uint32_t ledFlashPeriod = 0;
static uint32_t clockCounter   = 0;
static bool inputReady         = false;

static bool joystickPressed[] = {false, false};

static void (*COMMAND_FUNCS[]) (uint8_t) = {
	ui_cmdHelp
	,ui_cmdSetSpeed
	,ui_cmpStopWD
	,ui_cmdJoy
};

static const char const *COMMAND_DESCRIPTIONS[] = {
	"? : Display this help message"
	,"0-9 : Set speed 0 (slow) to 9 (fast)"
	,"x : Stop hitting the watchdog"
	,"JOY : Up (faster), Down (slower)"
};

/******************************************************************************
 **              FUNCTION DEFINITIONS
 ******************************************************************************/
void ui_init(void)
{
	const uint8_t INIT_LED_SPEED = 5;
	Serial_setRxIsrCallback(ui_serialISR);
	Timer_setTimerIsrCallback(ui_dmTimerISRCallback);
	ui_cmdSetSpeed(INIT_LED_SPEED);
}

void ui_doWork(void)
{
	ui_handleSerialInput();
	ui_handleJoystick();
	ui_handleLedDisplay();
}

void ui_greet(void)
{
	ConsoleUtilsPrintf("\nLightBouncer:\n");
	ConsoleUtilsPrintf("  by Adam Spilchen\n");
	ConsoleUtilsPrintf("------------------\n");
}

void ui_listResetEvents(void)
{
	uint32_t i = 0; 
	uint32_t nSrcs = 0;
	const enum PrmRststEvent const PRM_MASK_MAP[] = {
		ICEPICK_RST
		,EXTERNAL_WARM_RST
		,WDT1_RST
		,GLOBAL_WARM_SW_RST
		,GLOBAL_COLD_RST
	};

	const char const *RESET_EVENT_STRS[] = {
		"ICEPICK_RST"
		,"EXTERNAL_WARM_RST"
		,"WDT1_RST"
		,"GLOBAL_WARM_SW_RST"
		,"GLOBAL_COLD_RST"
	};

	for(i = 0; i < PRM_N_EVENTS; i++) {
		if(prm_isSet(PRM_MASK_MAP[i])) {
			nSrcs += 1;
			if(nSrcs > 1) {
				ConsoleUtilsPrintf(", ");
			}
			ConsoleUtilsPrintf("Reset source (0x%0x) = %s", PRM_MASK_MAP[i], RESET_EVENT_STRS[i]);
		}
	}

	ConsoleUtilsPrintf("\n");
}

void ui_cmdHelp(uint8_t arg)
{
	int i = 0;

	ConsoleUtilsPrintf("Commands:\n");
	for(i = 0; i < N_OPS; i++) {
		ConsoleUtilsPrintf(" %s\n", COMMAND_DESCRIPTIONS[i]);
	}
}

void ui_cmdSetSpeed(uint8_t arg)
{
	if(arg < LED_SPEED_MIN || LED_SPEED_MAX < arg) {
		return;
	}

	ConsoleUtilsPrintf("Setting LED speed to %d\n", arg);
	ledFlashSpeed = arg;
	ledFlashPeriod = 1 << arg;
}

void ui_cmpStopWD(uint8_t arg)
{
	ConsoleUtilsPrintf("No longer hitting the watchdog.\n");
	Watchdog_disable();
}

void ui_cmdJoy(uint8_t arg)
{
	switch (arg) {
	case JOY_UP:
		ConsoleUtilsPrintf("JOY : Up\n");
		ui_cmdSetSpeed(ledFlashSpeed + 1);
		break;
	case JOY_DOWN:
		ConsoleUtilsPrintf("JOY : Down\n");
		ui_cmdSetSpeed(ledFlashSpeed - 1);
	default:
		break;
	}
}

/******************************************************************************
 **              INTERNAL FUNCTIONS
 ******************************************************************************/
static enum CmdOpCode ui_parseInput(void)
{
	enum CmdOpCode opCode = OP_ERR;

	switch (serialInputByte) {
		case '?':
			opCode = OP_HELP;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			serialInputByte = serialInputByte - '0';
			opCode = OP_SPEED;
			break;
		case 'x':
		case 'X':
			opCode = OP_STOP_WD;
			break;
		default:
			opCode = OP_ERR;
	}

	return opCode;
}

static void ui_handleSerialInput(void)
{
	enum CmdOpCode opCode = 0;

	if(!inputReady) {
		return;
	}

	inputReady = false;
	opCode = ui_parseInput();
	ConsoleUtilsPutChar('\n');

	if(opCode == OP_ERR) {
		ConsoleUtilsPrintf("Invalid command.\n");
		return;
	}

	COMMAND_FUNCS[opCode](serialInputByte);
}

static void ui_handleJoystick(void)
{
	uint8_t dir = JOY_UP;
	bool pressed = false;

	for(dir = JOY_UP; dir < JOY_N_DIR; dir++) {
		pressed = joystick_isPressed(dir);

		if(joystickPressed[dir] && !pressed) { // stick was pressed, then released
			ui_cmdJoy(dir);
		}

		joystickPressed[dir] = pressed;
	}
}

static enum Led ui_nextLed(enum Led led)
{
	static int8_t modi = 1;

	if(led == N_LEDS - 1) {
		modi = -1;
	} else if(led == LED0) {
		modi = 1;
	}

	return led + modi;
}

static void ui_handleLedDisplay(void)
{
	static enum Led led = LED0;
	if(clockCounter < ledFlashPeriod) {
		return;
	}
	ui_clockReset();
	leds_turnOff(led);
	led = ui_nextLed(led);
	leds_turnOn(led);
}

static void ui_clockIncrement(void)
{
	clockCounter += 1;
}

static void ui_clockReset(void)
{
	clockCounter = 0;
}


static void ui_serialISR(uint8_t input)
{
	serialInputByte = input;
	inputReady = true;
}

static void ui_dmTimerISRCallback(void)
{
	clockCounter += 1;
}