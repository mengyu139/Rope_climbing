#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "sys.h"


#define GREEN_BOARD 0


//////////green///////////////////
#if GREEN_BOARD

#define LED PAout(8)


/////blue///

#else

#define LED PBout(14)

#endif
//////////////////////////////////////////







#endif


