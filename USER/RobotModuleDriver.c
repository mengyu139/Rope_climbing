#include "RobotModuleDriver.h"
#include "uart.h"
#include "protocal.h"

struct S_ROBO_MODULE s_robo_module;
enum ENUM_MOTOR_ID Enum_motor_id;
enum ENUM_MOTOR_MODE Enum_motor_mode;




//reset RoboModule driver
// fixed on group 0
// there will be beep last for 300ms after successfully reseted
void RoboModule_reset(uint8_t id)
{
	uint16_t send_id = 0;
	
	send_id = (0<<8) + (id<<4) + (0<<0);
	//send_id = 0x010;
	
	s_robo_module.can_tx_buff[0] = 0x55;
	s_robo_module.can_tx_buff[1] = 0x55;
	s_robo_module.can_tx_buff[2] = 0x55;
	s_robo_module.can_tx_buff[3] = 0x55;
	s_robo_module.can_tx_buff[4] = 0x55;
	s_robo_module.can_tx_buff[5] = 0x55;
	s_robo_module.can_tx_buff[6] = 0x55;
	s_robo_module.can_tx_buff[7] = 0x55;
	
	CAN1_Send( send_id , s_robo_module.can_tx_buff);
	
}

// After setting mode successfully , the there will be beep
// lasting for aroud 70ms
// fixed on group 0
// id: 1 ~ f
// mode : 1 ~ 8
void RoboModule_set_mode(uint8_t id , uint8_t mode)
{
	uint16_t send_id = 0;
	
	send_id = (0<<8) + (id<<4) + (1<<0);
	
	s_robo_module.can_tx_buff[0] = mode;
	s_robo_module.can_tx_buff[1] = 0x55;
	s_robo_module.can_tx_buff[2] = 0x55;
	s_robo_module.can_tx_buff[3] = 0x55;
	s_robo_module.can_tx_buff[4] = 0x55;
	s_robo_module.can_tx_buff[5] = 0x55;
	s_robo_module.can_tx_buff[6] = 0x55;
	s_robo_module.can_tx_buff[7] = 0x55;
	
	CAN1_Send( send_id , s_robo_module.can_tx_buff);
	
}


// fixed on group 0
// data -5000 ~ +5000
// id 1~f
// best  at 100hz send frequency
void RoboModule_open_loop_mode_set_data(uint8_t id , int16_t data)
{
	uint16_t send_id = 0;
	
	send_id = (0<<8) + (id<<4) + (2<<0);
	
	s_robo_module.can_tx_buff[0] = (uint8_t)( (data>>8)&0xff );
	s_robo_module.can_tx_buff[1] = (data&0xff);
	s_robo_module.can_tx_buff[2] = 0x55;
	s_robo_module.can_tx_buff[3] = 0x55;
	s_robo_module.can_tx_buff[4] = 0x55;
	s_robo_module.can_tx_buff[5] = 0x55;
	s_robo_module.can_tx_buff[6] = 0x55;
	s_robo_module.can_tx_buff[7] = 0x55;
	
	CAN1_Send( send_id , s_robo_module.can_tx_buff);
	
}

// fixed on group 0
// cycle : beter be 10ms or upper
// id 1~f
//limit_position:0, not used
void RoboModule_config(uint8_t id , uint8_t cycle , uint8_t limit_position)
{
	uint16_t send_id = 0;
	
	send_id = (0<<8) + (id<<4) + (0x0a<<0);
	
	s_robo_module.can_tx_buff[0] = cycle;//ms
	s_robo_module.can_tx_buff[1] = limit_position;// not used , be 0
	s_robo_module.can_tx_buff[2] = 0x55;
	s_robo_module.can_tx_buff[3] = 0x55;
	s_robo_module.can_tx_buff[4] = 0x55;
	s_robo_module.can_tx_buff[5] = 0x55;
	s_robo_module.can_tx_buff[6] = 0x55;
	s_robo_module.can_tx_buff[7] = 0x55;
	
	CAN1_Send( send_id , s_robo_module.can_tx_buff);
	
}


// id 1~f
void RoboModule_detect_online(uint8_t id )
{
	uint16_t send_id = 0;
	
	send_id = (0<<8) + (id<<4) + (0x0f<<0);
	
	s_robo_module.can_tx_buff[0] = 0x55;//ms
	s_robo_module.can_tx_buff[1] = 0x55;// not used , be 0
	s_robo_module.can_tx_buff[2] = 0x55;
	s_robo_module.can_tx_buff[3] = 0x55;
	s_robo_module.can_tx_buff[4] = 0x55;
	s_robo_module.can_tx_buff[5] = 0x55;
	s_robo_module.can_tx_buff[6] = 0x55;
	s_robo_module.can_tx_buff[7] = 0x55;
	
	CAN1_Send( send_id , s_robo_module.can_tx_buff);
	
}


void RoboModule_rev_pro(void)
{
	s_robo_module.real_current = (s_can_msg.RxMessage.Data[0]<<8) + s_can_msg.RxMessage.Data[1];
	s_robo_module.real_velocity = (s_can_msg.RxMessage.Data[2]<<8) + s_can_msg.RxMessage.Data[3];
	s_robo_module.real_position = (s_can_msg.RxMessage.Data[4]<<24) \
	+ (s_can_msg.RxMessage.Data[5]<<16)+\
	(s_can_msg.RxMessage.Data[6]<<8)+\
	(s_can_msg.RxMessage.Data[7]);
	
}


void task_update_motor_data(void)
{
	s_communication.s_car[MY_ID].speed = s_robo_module.real_velocity; //round per minute
	s_communication.s_car[MY_ID].height = ((float)s_robo_module.real_position)/48000*25.4/1000.0*1.5;//  -> meters
}
