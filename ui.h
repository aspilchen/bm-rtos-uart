/**
 * ui.h
 * 
 * User interface for Light Bouncer application.
 * 
 * Handles I/O for joystick, leds and serial port.
 */

#ifndef UI_H
#define UI_H

enum CmdOpCode {
	OP_HELP
	,OP_SPEED
	,OP_STOP_WD
	,OP_JOY
	,N_OPS
	,OP_ERR
};

#define LED_SPEED_MIN (0)
#define LED_SPEED_MAX (9)

void ui_init(void);
void ui_doWork(void);
void ui_greet(void);
void ui_cmdHelp(uint8_t arg);
void ui_cmdSetSpeed(uint8_t arg);
void ui_cmpStopWD(uint8_t arg);
void ui_cmdJoy(uint8_t arg);

#endif