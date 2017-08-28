#ifndef _ROBOT_MODULE_DRIVER_H_
#define _ROBOT_MODULE_DRIVER_H_

#include "stm32f10x.h"
#include "CAN1.h"
#include "Motor.h"

enum ENUM_MOTOR_ID
{
	 MOTOR_ALL=0,
	 MOTOR0,
	 MOTOR1,
	 MOTOR2,
	 MOTOR3,
	 MOTOR4,
};


enum ENUM_MOTOR_MODE
{
	 MOTOR_OPEN_LOOP=1
};


extern enum ENUM_MOTOR_ID Enum_motor_id;
extern enum ENUM_MOTOR_MODE Enum_motor_mode;



void RoboModule_reset(uint8_t id);
void RoboModule_set_mode(uint8_t id , uint8_t mode);
void RoboModule_open_loop_mode_set_data(uint8_t id , int16_t data);
void RoboModule_config(uint8_t id , uint8_t cycle , uint8_t limit_position);
void RoboModule_detect_online(uint8_t id );
void RoboModule_rev_pro(void);



#endif