#ifndef LIMIT_SWITCHES_H_
#define LIMIT_SWITCHES_H_

#include "stm32f4xx.h"

typedef struct switch_control_t{
	void (*switchSetup)(void);
	void (*debugX)(void);
	void (*debugY)(void);
}switch_control_t;


extern switch_control_t h_switch;
extern switch_control_t v_switch;


#define SWITCHPORT		GPIOC
#define RSWPIN			0
#define LSWPIN			1
#define USWPIN			2
#define DSWPIN			3

void debug_XSwitchInit(void);
void debug_YSwitchInit(void);

void limitSwitchInit(void);

#endif /* LIMIT_SWITCHES_H_ */
