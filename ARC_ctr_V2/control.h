#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "string.h"
#include "stdio.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_error.h"
#include "delay.h"
#include "math.h"
#include "dev_gpio.h"
#include "dfss_gpio_obj.h"

#define ID_PORT_ID	    (DFSS_GPIO_4B1_ID)
#define ID_MASK1	    ((0x1) << 2)  //scl 4b_1(2)
#define ID_MASK2	    ((0x1) << 3)  //sda 4b_1(3)
static DEV_GPIO *id_port;
#define scaleAngleSpeed  0.06103515625f

typedef signed char            S8;
typedef unsigned char          U8;
typedef signed short           S16;
typedef unsigned short         U16;
typedef signed long            S32;
typedef unsigned long          U32;
typedef float                  F32;

extern DEV_UART *dev_uart1; //set a pointer of uart
extern DEV_UART *dev_uart2; //set a pointer of uart

//变量定义
//调试打印信息
char x[5];
char y[5];
char z[5];
//传感器位移
S16 X_dif;
S16 Y_dif;
S16 Z_dif;
//机械臂新坐标
S16 X_new;
S16 Y_new;
S16 Z_new;
//传感器新坐标
S16 X;
S16 Y;
S16 Z;
//机械臂旧坐标
S16 X_old;
S16 Y_old;
S16 Z_old;
//传感器旧坐标
S16 X2;
S16 Y2;
S16 Z2;
//字符串长度
uint8_t frame1_len;
uint8_t frame2_len;
uint8_t frame4_len;
//数据包接收数组
uint8_t byte[19];
//图片识别ID
extern uint8_t id;
//蓝牙连接指令
extern char frame1[50];
extern char frame2[50];
extern char frame4[30];
//分拣指令
extern char Left_get[30];
extern char Right_get[30];
extern char Front_get[30];
extern uint8_t Left_get_len;
extern uint8_t Right_get_len;
extern uint8_t Front_get_len;

int uart1_init(uint32_t baudrate);//串口1初始化
int uart2_init(uint32_t baudrate);//串口2初始化
void System_init();//系统初始化
void Bluetooth_connect();//蓝牙连接
void Automatic_sorting();//自动分拣
void Gesture_control();//手势控制
void ID_Port_Init();//GPIO初始化
uint8_t read_id();//ID读取




#endif