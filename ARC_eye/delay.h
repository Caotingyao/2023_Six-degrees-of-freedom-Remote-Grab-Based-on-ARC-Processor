#ifndef _DELAY_H_
#define _DELAY_H_

#include "embARC.h"
//#include "embARC_debug.h"
//#include "embARC_error.h"

#include "board.h"
#include "dev_common.h"


void     Delay_s(volatile uint32_t s);
void     Delay_ms(volatile uint32_t ms);
void     Delay_us(uint32_t us);

#endif