/*
 * subroutines.c
 *
 */
#include "subroutines.h"

void NVIC_Init(void){
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0);
	NVIC_SetPriority(TIM3_IRQn, 0);

	NVIC_SetPriority(EXTI0_IRQn, 1);
	NVIC_SetPriority(EXTI1_IRQn, 1);
	NVIC_SetPriority(EXTI2_IRQn, 1);
	NVIC_SetPriority(EXTI3_IRQn, 1);
	NVIC_SetPriority(EXTI15_10_IRQn, 3);

	NVIC_SetPriority(DMA1_Stream5_IRQn, 2);

	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	NVIC_EnableIRQ(DMA1_Stream5_IRQn);

	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
}

/* Communication Handler */
void DMA1_Stream5_IRQHandler(){
	if ((DMA1->HISR & DMA_HISR_TCIF5) != 0){
		uint16_t query_frame = frames__packFrames_uint16(USART_rx_buffer,0);
		DMA1->HIFCR |= DMA_HIFCR_CTCIF5;

		switch(query_frame)
		{
		case FRAME_STOP:
			stepper_control.device_state = STOPPED;
			h_control.disable();
			v_control.disable();
			break;
		case FRAME_IDN: ;
			uint8_t str[] = "STEPPER";
			USART_PutString(str);

			break;
		case FRAME_SUBMIT: ;
			uint16_t speed = frames__packFrames_uint16(USART_rx_buffer,2);
			uint16_t first_layer_depth = frames__packFrames_uint16(USART_rx_buffer,4); 		//TODO add first layer depth handler
			uint16_t default_layer_depth = frames__packFrames_uint16(USART_rx_buffer,6);	//TODO add first layer depth handler
			stepper_control.axes.Z_axis.pulse->first_layer_depth = first_layer_depth;
			stepper_control.axes.Z_axis.pulse->default_layer_depth = default_layer_depth;
			stepper_control.axes.X_axis.pulse->max_frq = speed;
			break;
		case FRAME_NEXT_LAYER:
			if(stepper_control.device_state == READY){
				stepper_control.device_state = WORKING;
				stepper_control.current_task = TASK_NEXT_LAYER;
			}
			break;
		case FRAME_FINISH_WORK:
			if(stepper_control.device_state == WORKING)
				stepper_control.device_state = FINISHED;
			break;
		case FRAME_CALIBRATE:
			if(stepper_control.device_state == IDLE){
				stepper_control.axes.X_axis.pulse->unlock();
				stepper_control.axes.Z_axis.pulse->unlock();
			}

			h_control.enable();
			v_control.enable();
			v_control.setDir(UP);
			h_pulse.status = NOT_CALIBRATED;
			v_pulse.status = NOT_CALIBRATED;
			stepper_control.device_state = CALIBRATING;
			break;
		case FRAME_CALIBRATE_X:
			if(stepper_control.device_state == IDLE)
				stepper_control.axes.X_axis.pulse->unlock();
			stepper_control.device_state = CALIBRATING;
//			stepper_control.current_task = TASK_
			h_pulse.status = NOT_CALIBRATED;
			h_control.enable();
			h_pulse.lower_limit = 0;
			h_pulse.upper_limit = 10000;
//			h_control.setDir(LEFT);
			break;
		case FRAME_CALIBRATE_Z:
//			if(stepper_control.device_state == IDLE)
//				stepper_control.axes.Z_axis.pulse->unlock();
			v_control.enable();
			v_control.setDir(UP);
			v_pulse.status = NOT_CALIBRATED;
			stepper_control.device_state = CALIBRATING;
			break;
		case FRAME_TEST:
			if(stepper_control.device_state == READY){
				stepper_control.device_state = WORKING;
				stepper_control.current_task = TASK_TEST;
			}
			break;
		case FRAME_TEST_X:
//			h_control.disable();
			if(stepper_control.device_state == READY){
				stepper_control.device_state = WORKING;
				uint16_t option = frames__packFrames_uint16(USART_rx_buffer,2);
				if(option == FRAME_TEST_X)
					stepper_control.current_task = TASK_STRESSTEST_X;
				else
					stepper_control.current_task = TASK_TEST_X;
			}
			break;
		case FRAME_TEST_Z: ;
//			h_control.enable();
			if(stepper_control.device_state == READY){
				stepper_control.device_state = WORKING;
				stepper_control.current_task = TASK_TEST_Z;
			}
			break;
		case FRAME_HOME:
			if(stepper_control.device_state == READY){
				stepper_control.device_state = WORKING;
				stepper_control.current_task = TASK_HOME;
			}
			break;
		case FRAME_HOME_X: ;
			if(stepper_control.device_state == READY){
				stepper_control.device_state = WORKING;
				stepper_control.current_task = TASK_HOME_X;
			}
			break;
		case FRAME_HOME_Z: ;
			if(stepper_control.device_state == READY){
				stepper_control.device_state = WORKING;
				stepper_control.current_task = TASK_HOME_Z;
			}
			break;
		case FRAME_TOGGLE: ;
			stepper_control.device_state = WORKING;
			stepper_control.current_task = TASK_TOGGLE;
			break;
		case FRAME_CLEAN: ;
			stepper_control.device_state = WORKING;
			// TODO: add Z axis cleaning routine
			stepper_control.current_task = TASK_HOME_Z;
			break;

		default:
			break;
		}
	}
}


/*	LEFT LIMIT SWITCH	*/
void EXTI0_IRQHandler(){
	if ((EXTI->PR & EXTI_PR_PR0_Msk) != 0){
		TIM1->CR1 &= ~TIM_CR1_CEN;
		switch(h_pulse.status){
		case  NOT_CALIBRATED:
			h_pulse.disable();
			h_control.setDir(RIGHT);
			if((h_pulse.current_position > H_MARGIN) && (h_pulse.lower_limit != 0))
				pulse__H_ConstPulse(H_MARGIN, 10000);
			EXTI->PR |= EXTI_PR_PR0;
			h_pulse.upper_limit = h_pulse.current_position;

			h_pulse.status = CALIBRATED;
			break;
		case CALIBRATED:
//			HARDFAULT, TURN OFF THE OUTPUTS, REMOVE CALIBRATION
//			h_control.disable();
//			h_pulse.disable();
//			stepper_control.axes.X_axis.pulse->lock(); // MOE lock
//
			EXTI->PR |= EXTI_PR_PR0;
//			h_pulse.status = NOT_CALIBRATED;
			break;
		default:
			break;
		}
	}
}

/*	RIGHT LIMIT SWITCH	*/
void EXTI1_IRQHandler(){
	if ((EXTI->PR & EXTI_PR_PR1_Msk) != 0){
		TIM1->CR1 &= ~TIM_CR1_CEN;
		switch(h_pulse.status){
		case  NOT_CALIBRATED:
			h_pulse.disable();
			h_control.setDir(LEFT);
			h_pulse.current_position = 0;
			pulse__H_ConstPulse(H_MARGIN, 10000);
			EXTI->PR |= EXTI_PR_PR1;
			h_pulse.lower_limit = h_pulse.current_position;
			// Check IDR value
			// save position
			break;
		case CALIBRATED:
//			HARDFAULT, TURN OFF THE OUTPUTS, REMOVE CALIBRATION
//			h_control.disable();
//			h_pulse.disable();
//			stepper_control.axes.X_axis.pulse->lock();
			EXTI->PR |= EXTI_PR_PR1;
//			h_pulse.status = NOT_CALIBRATED;
			break;
		default:
			break;
		}
	}
}

/*	UPPER LIMIT SWITCH	*/
void EXTI2_IRQHandler(){

	if ((EXTI->PR & EXTI_PR_PR2_Msk) != 0){
		TIM3->CR1 &= ~TIM_CR1_CEN;
		v_control.disable();
		switch (v_pulse.status){
		case NOT_CALIBRATED:
			v_pulse.disable();
			v_control.setDir(DOWN);
			v_pulse.current_position = 0;
			v_control.enable();
			pulse__V_ConstPulse(V_MARGIN, 1000);
			v_pulse.lower_limit = v_pulse.current_position;
			EXTI->PR |= (EXTI_PR_PR2);
			v_pulse.status = CALIBRATED;
			//TODO calibrate towards upper limit
			break;
		case CALIBRATED:
//			v_control.disable();
//			v_pulse.disable();
////			stepper_control.axes.Z_axis.pulse->lock();
			EXTI->PR |= (EXTI_PR_PR2);
//			v_pulse.status = NOT_CALIBRATED;
			break;
		default:
			break;
		}
	}
}

/*	LOWER LIMIT SWITCH	*/
void EXTI3_IRQHandler(){
	if ((EXTI->PR & EXTI_PR_PR3_Msk) != 0){
		v_control.disable();
//		v_pulse.disable();
//		v_pulse.status = NOT_CALIBRATED;
		EXTI->PR |= (EXTI_PR_PR3);
//		stepper_control.axes.Z_axis.pulse->lock();
		//TODO: Should never be reached, so break if switched
	}
}

void TIM1_UP_TIM10_IRQHandler(){
	if (TIM1->SR & TIM_SR_UIF) {
	    TIM1->SR &= ~(TIM_SR_UIF);
	    stepper_control.axes.X_axis.pulse->busy = PULSE_IDLE;
	}
}

void TIM3_IRQHandler(){
	if (TIM3->SR & TIM_SR_UIF) {
		TIM3->SR &= ~(TIM_SR_UIF);
//    	TIM3->CR1 |= TIM_CR1_CEN; // DO NOT USE, LEFT FOR TESTING ONLY
		stepper_control.axes.Z_axis.pulse->busy = PULSE_IDLE;
	}
}





void EXTI15_10_IRQHandler(){
	if ((EXTI->PR & EXTI_PR_PR13_Msk) != 0){
		EXTI->PR |= (EXTI_PR_PR13);
		if(stepper_control.device_state == READY){
			stepper_control.device_state = WORKING;
			stepper_control.current_task = TASK_NEXT_LAYER;
		}
	}
}
