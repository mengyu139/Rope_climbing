#include "protocal.h"
#include "uart.h"
#include "CAN1.h"

#include "Motor.h"



struct S_COMMUNICATION s_communication;



////flag
////0: set target
////1: share info 
void Proto_send(uint8_t flag )
{
	if( flag == 0 )
	{
		Float_to_Byte( s_communication.s_car[0].target_height , s_can_msg.tx_buff);
		CAN1_Send( ((MY_ID+0x20)<<4)+flag , s_can_msg.tx_buff );
	}
	
	else if( flag == 1 )
	{
			Float_to_Byte( s_communication.s_car[MY_ID].height , s_can_msg.tx_buff);
			Float_to_Byte( s_communication.s_car[MY_ID].speed , s_can_msg.tx_buff+4);
			
			CAN1_Send( ((MY_ID+0x20)<<4)+flag , s_can_msg.tx_buff );
	}
	
	
}

void protocol_rev(void)
{
	uint16_t id;
	float rev_target_height;
	
	
	//set target
	if( MY_ID !=0 )
	{
		if( s_can_msg.RxMessage.StdId == 0x200 )
		{	
				Byte_to_Float( &rev_target_height , s_can_msg.RxMessage.Data  );
				
			//not only change target height , and direction update
				Set_target( MY_ID,rev_target_height );
			
			// in slavers , , only update target height in master 
				//Byte_to_Float( &s_communication.s_car[0].target_height , s_can_msg.RxMessage.Data  );	
			
			return;
		}	
	}

	
	// robot share info
	if( (s_can_msg.RxMessage.StdId & 0xf00) == 0x200 )
	{
		//we onely have 4 robot at most currently
		id = (s_can_msg.RxMessage.StdId & 0x0f0)>>4;
		if( (  id< MAX_CAR_NUM ) &&( (s_can_msg.RxMessage.StdId & 0x00f) == 1 ) )
		{
			Byte_to_Float( &s_communication.s_car[id].height , s_can_msg.RxMessage.Data );
			Byte_to_Float( &s_communication.s_car[id].speed , s_can_msg.RxMessage.Data+4 );
		}
	}
	
	// robot module driver msg
	else if( ((s_can_msg.RxMessage.StdId & 0xf00) == 0x000)&&\
		( (s_can_msg.RxMessage.StdId & 0x00f) == 0x0b)&& \
		( ((s_can_msg.RxMessage.StdId & 0x0f0)>>4) == ( MY_ID+1 ) )
			)
	{
		RoboModule_rev_pro();
		
	}
	
}






uint8_t protocol_crc(uint8_t * buff,int num)
{
	int i=0;
	uint8_t result = 0;
	for( i=0;i<num;i++ )
	{
		result += (*(buff+i));
	}
	return result;
}

void Float_to_Byte(float f,unsigned char byte[])  
{  
    FloatLongType fl;  
    fl.fdata=f;  
    byte[0]=(unsigned char)fl.ldata;  
    byte[1]=(unsigned char)(fl.ldata>>8);  
    byte[2]=(unsigned char)(fl.ldata>>16);  
    byte[3]=(unsigned char)(fl.ldata>>24);  
}  
/* 

*/  
void Byte_to_Float(float *f,unsigned char byte[])  
{  
    FloatLongType fl;  
    fl.ldata=0;  
    fl.ldata=byte[3];  
    fl.ldata=(fl.ldata<<8)|byte[2];  
    fl.ldata=(fl.ldata<<8)|byte[1];  
    fl.ldata=(fl.ldata<<8)|byte[0];  
    *f=fl.fdata;  
}  
  


