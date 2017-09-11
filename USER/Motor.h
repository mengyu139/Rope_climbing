#ifndef _MOTRO_H_
#define _MOTRO_H_

#include "stm32f10x.h"
#include "RobotModuleDriver.h"

#define MOTOR1_OUT(a) 	TIM_SetCompare1(TIM4,a);


#define MOTOR_OUT_MAX 3500
#define MOTOR_OUT_MIN (-3500)
#define POSITIVE_COMPASETION (450+300)
#define NEGTIVE_COMPASETION (-450)





struct S_ROBO_MODULE
{
	int16_t real_current;
	int16_t real_velocity;
	int32_t real_position;
	
	uint8_t can_tx_buff[8];
	
	
};

extern struct S_ROBO_MODULE s_robo_module;



struct S_PID_PARA//pid参数相关信息
{
	float target;
	
	float P;
	float I;
	float D;

	float intergral;
	
	float currentS;
	float delta;
	
	float out;
	
	int direction;

};

extern struct S_PID_PARA s_pid_para;


//void Motor_PWMMode_init(void);

void motor_out(float a);

void Motor_init(void);

void task_motor_control(uint8_t flag);

void Set_target( uint8_t id, float target);

void task_update_motor_data(void);

float abs (float a);


extern uint32_t Motor_Clock;



#endif
