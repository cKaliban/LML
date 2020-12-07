/*
 * driver.h
 *
 *      Author: ckali
 */

#ifndef DEVICE_PULSEGEN_H_
#define DEVICE_PULSEGEN_H_

#include "stm32f4xx.h"
#include "stdint.h"
#include "math.h"

#include "axis_types.h"
#include "control_signals.h"

/* GENERAL pulse settings */
// Pulse Width setting
#define PULSE		100-1

// Calibration margins from absolute limits
#define H_MARGIN 	50
#define V_MARGIN	4000

typedef enum busy_t{
	PULSE_BUSY, PULSE_IDLE
}busy_t;



typedef struct pulse_config_t{
	axis_t axis;
	axis_state_t status;
	busy_t busy;

	uint32_t max_frq;

	uint32_t current_position;
	uint32_t lower_limit;
	uint32_t center_pos;
	uint32_t cc1_limit;		// X axis specific
	uint32_t cc2_limit; 	// X axis specific
	uint32_t upper_limit;	// X axis specific

	uint16_t first_layer_depth;		// Z axis specific
	uint16_t default_layer_depth;	// Z axis specific

	void (*portSetup)(void);
	void (*timerSetup)(void);

	void (*calibrate)(void);
	void (*home)(void);
	void (*run)(uint32_t);
	void (*special)(void);

	void (*disable)(void);
	void (*enable)(void);

	void(*lock)(void);
	void(*unlock)(void);
}pulse_config_t;


extern pulse_config_t h_pulse;
extern pulse_config_t v_pulse;

/*	GLOBAL WRAPPERS	*/
void pulse__Init(pulse_config_t*, axis_t);


//===================================
/* WORK IN PROGRESS */
// HORIZONTAL
void pulse__H_Calibrate(void);
void pulse__H_ConstPulse(uint32_t, uint16_t);
void pulse__H_SinglePass(uint32_t);


// VERTICAL
void pulse__V_Calibrate(void);
void pulse__V_ConstPulse(uint32_t, uint16_t);
void pulse__V_SinglePass(uint32_t);



#endif /* DEVICE_PULSEGEN_H_ */
