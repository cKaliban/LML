#ifndef DEVICE_DEVICE_STATE_H_
#define DEVICE_DEVICE_STATE_H_

//TODO: copy it to the Laser Driver
typedef enum dev_name_t{
	MOTOR = 0xAA,
	POWER = 0xBB
}dev_name_t;

typedef enum dev_state_t{
	IDLE,
	READY,
	CALIBRATING,
	WORKING,
	FINISHED,
	STOPPED,
}dev_state_t;




#endif /* DEVICE_DEVICE_STATE_H */
