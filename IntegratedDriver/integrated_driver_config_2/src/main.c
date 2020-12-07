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

#include "../lib/communication/communication.h"
#include "../lib/utils/general_configuration.h"
#include "../lib/utils/delay.h"
#include "../lib/utils/timing.h"
#include "../lib/utils/subroutines.h"

#include "../lib/device/device.h"
#include "../lib/device/laser/pyro_comm.h"
#include "../lib/device/laser/periph_conf.h"







int main(void)
{
	/*	DEVICE CONFIGURATION	*/
	/*	GENERAL		*/
	mainClockInit();
	buttonInit(); //TODO change to button plus input
	delayInit();
	UARTInit();

	//Deprecated
	InComm__Init();

	//Laser configuration
	Pyro__Init();
	PWM__Init();
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
	watch__init();
	NVIC_Init();


	watch__start();
	delay_ms(100);

	uint8_t lay_num = 0;

	for(;;){
		switch(stepper_control.device_state){
		/*	DEFAULT IDLE STATE	*/
		case IDLE:
//			if(PWMpid.status == PIDS_WARMUP){
//				PID__Reset(&PWMpid);
//				PWM__Start();
//				PWMpid.status = PIDS_IDLE;
//			}
//			if(PWMpid.status == PIDS_IDLE){
//
//			}
//			if(PWMpid.status == PIDS_STARTING){
//				PID__Reset(&PWMpid);
//				PWM__Update(PWMpid.out_init);
////				watch__start();
//				delay_ms(300);
//				PWMpid.status = PIDS_RUN;
//			}
//			if(PWMpid.status == PIDS_RUN){
//				if(PWMpid.mode == PID_ON){
//					if(PWMpid.stable_count < 1000){
////						PID__Step(&PWMpid);
//					}
//					else
//						PWMpid.status = PIDS_LOCK;
//				}
//				else if(PWMpid.mode == PID_OFF){
//					delay_ms(100);
//				}
//			}
//			if(PWMpid.status == PIDS_LOCK){
//				delay_ms(100);
//			}
//			if(PWMpid.status == PIDS_STOP){
//				PWM__Stop();
//				PWMpid.status = PIDS_IDLE;
//				if(stepper_control.device_state == WORKING){
////					stepper_control.current_task = TASK_NEXT_LAYER;
//				}
//				else{
////					stepper_control.device_state = READY;
//				}
//			}
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
				PID__Reset(&PWMpid);
				PWM__Start();
				PWMpid.status = PIDS_IDLE;
			}
			break;

		/*	REPORTING STOPPED STATE	*/
		case STOPPED:
			PWM__Stop();
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
			case TASK_NEXT_LAYER: ;
				if (lay_num < 2){
					lay_num++;
//				InComm__PauseLaser();
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
					stepper_control.device_state = WORKING;
					stepper_control.current_task = TASK_LASER_CONTROL;
					PWMpid.status = PIDS_STARTING;
				}
				else if(h_pulse.current_position == h_pulse.upper_limit){
					stepper_control.axes.X_axis.control->setDir(RIGHT);
					stepper_control.axes.X_axis.pulse->run(stepper_control.axes.X_axis.pulse->max_frq);
					stepper_control.device_state = WORKING;
					stepper_control.current_task = TASK_LASER_CONTROL;
					PWMpid.status = PIDS_STARTING;
				}
				else{
					stepper_control.current_task = TASK_FAILED;
					break;
				}
//				InComm__StartLaser();
				InComm__StartWork();
				uint8_t comm[] = "LAYREADY";
				USART_PutString(comm);
//				stepper_control.current_task = TASK_NONE;
				}
				else
					stepper_control.current_task = TASK_FINISH_WORK;
				break;
			case TASK_LASER_CONTROL:
				if(PWMpid.status == PIDS_WARMUP){
					PID__Reset(&PWMpid);
					PWM__Start();
					PWMpid.status = PIDS_STARTING;
				}
				if(PWMpid.status == PIDS_IDLE){

				}
				if(PWMpid.status == PIDS_STARTING){
					PID__Reset(&PWMpid);
					PWM__Update(PWMpid.out_init);
					watch__start();
					delay_ms(300);
					PWMpid.status = PIDS_RUN;
				}
				if(PWMpid.status == PIDS_RUN){
					if(PWMpid.mode == PID_ON){
						if(PWMpid.stable_count < 100)
							PID__Step(&PWMpid);
						else
							PWMpid.status = PIDS_LOCK;
					}
					else if(PWMpid.mode == PID_OFF){
						delay_ms(100);
					}
				}
				if(PWMpid.status == PIDS_LOCK){
					delay_ms(100);
				}
				if(PWMpid.status == PIDS_STOP){
					PWM__Stop();
					PWMpid.status = PIDS_IDLE;
					if(stepper_control.device_state == WORKING){
						stepper_control.current_task = TASK_NEXT_LAYER;
					}
					else{
						PWM__Stop();
						stepper_control.device_state = READY;
					}
				}


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
				stepper_control.current_layer = FIRST_LAYER_X;
				lay_num = 0;
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


