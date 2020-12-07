/**
  ******************************************************************************

  * @file    main.c
  * @author  Mateusz Wieczorek
  * @version V1.0
  * @brief   Main function of motor control device
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "stdint.h"

#include "../lib/utils/general_configuration.h"
#include "../lib/utils/delay.h"
#include "../lib/utils/timing.h"
#include "../lib/utils/subroutines.h"

#include "../lib/communication/UART.h"

#include "../lib/device/device.h"




int main(void)
{
	/*	DEVICE CONFIGURATION	*/
	/*	GENERAL		*/
	mainClockInit();
	buttonInit();
	delayInit();
	UARTInit();
	InComm__Init();
	/*	STEPPER AXES CONTROL CONFIGURATION	*/
	control__Init(&h_control, HORIZONTAL);
	pulse__Init(&h_pulse, HORIZONTAL);
	stepper_control.axes.X_axis.control->portSetup();
	stepper_control.axes.X_axis.pulse->timerSetup();
	stepper_control.axes.X_axis.pulse->portSetup();

	control__Init(&v_control, VERTICAL);
	pulse__Init(&v_pulse, VERTICAL);
	stepper_control.axes.Z_axis.control->portSetup();
	stepper_control.axes.Z_axis.pulse->timerSetup();
	stepper_control.axes.Z_axis.pulse->portSetup();

	/*	LIMITS CONFIGURATION	*/
	limitSwitchInit();
	NVIC_Init();

	delay_ms(100);

	/*	MAIN STATE MACHINE LOOP	*/
	for(;;){
		switch(stepper_control.device_state){
		/*	DEFAULT IDLE STATE	*/
		case IDLE:
			break;

		/*	READY TO WORK STATE	*/
		case READY:
			break;

		/*	CALIBRATION STATE	*/
		case CALIBRATING:
			if(stepper_control.axes.Z_axis.pulse->status == NOT_CALIBRATED){
				stepper_control.axes.Z_axis.pulse->calibrate();
			}
			else if(stepper_control.axes.X_axis.pulse->status == NOT_CALIBRATED){
				stepper_control.axes.X_axis.pulse->calibrate();
			}

			if(stepper_control.axes.Z_axis.pulse->status == CALIBRATED && stepper_control.axes.X_axis.pulse->status == CALIBRATED)
			{
				h_pulse.cc1_limit = h_pulse.lower_limit + (h_pulse.upper_limit - h_pulse.lower_limit) / 3;
				h_pulse.cc2_limit = h_pulse.lower_limit + (h_pulse.upper_limit - h_pulse.lower_limit) / 3 * 2;
				h_pulse.center_pos = h_pulse.lower_limit + (h_pulse.upper_limit - h_pulse.lower_limit) / 2;
				while(h_pulse.current_position != h_pulse.lower_limit)
					stepper_control.axes.X_axis.pulse->home();
				while(v_pulse.current_position != v_pulse.lower_limit)
					stepper_control.axes.Z_axis.pulse->home();
				stepper_control.device_state = READY;
			}
			break;

		/*	REPORTING STOPPED STATE	*/
		case STOPPED:
			stepper_control.device_state = IDLE;
			break;

		/*	WORKING STATE	*/
		case WORKING:
			if(h_pulse.status == NOT_CALIBRATED){
				stepper_control.device_state = IDLE;
				break;
			}
			if(v_pulse.status == NOT_CALIBRATED){
				stepper_control.device_state = IDLE;
				break;
			}
			/*	TASK MANAGEMENT STATE MACHINE	*/
			switch(stepper_control.current_task){
			case TASK_NEXT_LAYER:
				InComm__PauseLaser();
				uint32_t Vmax = stepper_control.axes.X_axis.pulse->max_frq;
				uint16_t depth = 0;

				// TODO: add depth
				if(stepper_control.current_layer == DEFAULT_LAYER){
					depth = stepper_control.axes.Z_axis.pulse->default_layer_depth;
				}
				else if(stepper_control.current_layer == FIRST_LAYER_Z){
					depth = stepper_control.axes.Z_axis.pulse->first_layer_depth;
					stepper_control.current_layer = DEFAULT_LAYER;
				}
				else if(stepper_control.current_layer == FIRST_LAYER_X){
					depth = 0;
					stepper_control.current_layer = FIRST_LAYER_Z;
				}

				stepper_control.axes.Z_axis.pulse->run(depth);

				if(h_pulse.current_position == h_pulse.lower_limit){
					stepper_control.axes.X_axis.control->setDir(LEFT);
					stepper_control.axes.X_axis.pulse->run(Vmax);
					stepper_control.device_state = READY;
					stepper_control.current_task = TASK_NONE;
				}
				else if(h_pulse.current_position == h_pulse.upper_limit){
					stepper_control.axes.X_axis.control->setDir(RIGHT);
					stepper_control.axes.X_axis.pulse->run(stepper_control.axes.X_axis.pulse->max_frq);
					stepper_control.device_state = READY;
					stepper_control.current_task = TASK_NONE;
				}
				else{
					stepper_control.current_task = TASK_FAILED;
					break;
				}
				InComm__StartLaser();
				stepper_control.current_task = TASK_NONE;
				break;
			case TASK_HOME:
				stepper_control.device_state = FINISHED;
				break;
			case TASK_HOME_X:
				if(h_pulse.current_position == h_pulse.lower_limit){
					stepper_control.device_state = FINISHED;
				}
				else{
					stepper_control.axes.X_axis.pulse->home();
				}
				break;
			case TASK_HOME_Z:
				if(v_pulse.current_position == v_pulse.lower_limit){
					stepper_control.device_state = FINISHED;
				}
				else{
					stepper_control.axes.Z_axis.pulse->home();
				}
				break;

			case TASK_CLEAN:
				stepper_control.axes.Z_axis.pulse->special();
				stepper_control.device_state = FINISHED;
				break;
			case TASK_TOGGLE:
				stepper_control.axes.X_axis.pulse->special();
				stepper_control.device_state = FINISHED;
				break;
			case TASK_FINISH_WORK:
				stepper_control.device_state = FINISHED;
				break;
			case TASK_TEST:
//				Test signal from all limit switches
				stepper_control.current_task = TASK_NONE;
				stepper_control.device_state = FINISHED;
				break;
			case TASK_TEST_X:
				if(h_pulse.current_position == h_pulse.lower_limit){
					stepper_control.axes.X_axis.control->setDir(LEFT);
					stepper_control.axes.X_axis.pulse->run(stepper_control.axes.X_axis.pulse->max_frq);
					stepper_control.device_state = FINISHED;
					delay_ms(500);
				}
				if(h_pulse.current_position == h_pulse.upper_limit){
					stepper_control.axes.X_axis.control->setDir(RIGHT);
					stepper_control.axes.X_axis.pulse->run(stepper_control.axes.X_axis.pulse->max_frq);
					stepper_control.device_state = FINISHED;
					delay_ms(500);
				}
				else{
					stepper_control.current_task = TASK_FAILED;
					stepper_control.device_state = FINISHED;
				}
				break;
			case TASK_STRESSTEST_X:
				if(h_pulse.current_position == h_pulse.lower_limit){
					stepper_control.axes.X_axis.control->setDir(LEFT);
					stepper_control.axes.X_axis.pulse->run(stepper_control.axes.X_axis.pulse->max_frq);
//					stepper_control.device_state = FINISHED;
					delay_ms(200);
				}
				if(h_pulse.current_position == h_pulse.upper_limit){
					stepper_control.axes.X_axis.control->setDir(RIGHT);
					stepper_control.axes.X_axis.pulse->run(stepper_control.axes.X_axis.pulse->max_frq);
//					stepper_control.device_state = FINISHED;
					delay_ms(200);
				}
				break;
			case TASK_STRESSTEST_Z:
				break;
			case TASK_TEST_Z:
				stepper_control.axes.Z_axis.control->setDir(DOWN);
				stepper_control.axes.Z_axis.pulse->run(10000);
				stepper_control.device_state = READY;
				break;
			case TASK_FAILED:
				break;
			case TASK_NONE:
				break;
			default:
				break;
			}
			break;

		/*	WORK FINISHED REPORTING STATE	*/
		case FINISHED:
			stepper_control.current_layer = FIRST_LAYER_X;
			stepper_control.current_task = TASK_NONE;
			stepper_control.device_state = READY;
			break;

		/*	STATE MACHINE FAULT STATE	*/
		default:
			break;
		}
	}
}


