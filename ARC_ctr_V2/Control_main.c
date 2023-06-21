/*
 * @ Author:	WYC
 * @ Email:		810246013@qq.com
 * @ Company:	XIDIAN University
 * @ Date:		2023-06-05 14:34:43
 * @ LastEditTime:		2023-06-05 14:35:00
 * @ LastEditors:		WYC
 * @ Module Name:		
 * @ Function:			
 */
#include "string.h"
#include "stdio.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_error.h"
#include "delay.h"
#include "math.h"
#include "control.h"
//#include "dfss_uart_obj.h"

void main()
{
  //系统初始化
  System_init();

  //蓝牙连接
  Bluetooth_connect();

  while(1)
  {
    //使用其中一个功能时需要关闭另一个功能

    //启动自动分拣功能***************************************************************
    //Automatic_sorting();

    //启动手势控制功能***************************************************************
    Gesture_control();
  }
  
}