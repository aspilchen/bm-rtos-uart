// joystick.h
// Hardware abstraction module: Zen Cape Joystick

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdbool.h>

enum JoystickDirection {
	JOY_UP
	,JOY_DOWN
	,JOY_N_DIR
};

void joystick_init(void);
bool joystick_isPressed(enum JoystickDirection dir);

#endif