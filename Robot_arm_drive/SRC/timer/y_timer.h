/****************************************************************************
 *	@功能	：	存放定时器相关的函数
 ****************************************************************************/
#ifndef _Y_TIMER_H_
#define _Y_TIMER_H_

#include "main.h"

void SysTick_Init(void); /* 初始化systick */
u32 millis(void);/* 获取滴答时钟数值 */

void TIM2_init(u16 arr, u16 psc);
void timer1_init(u16 arr, u16 psc);
#endif
