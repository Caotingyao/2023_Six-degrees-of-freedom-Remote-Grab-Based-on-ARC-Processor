///*
// * @文件描述:
// * @作者: Q
// * @Date: 2023-02-13 14:01:12
// * @LastEditTime: 2023-03-28 09:32:51
// */
#include "app_sensor.h"
#include "sensor/y_sensor.h"
#include "tcs34725/y_tcs34725.h"
#include "ws2812b/y_ws2812b.h"
#include "./sensor/y_sensor.h"

void AI_yanse_shibie(void);
void AI_dingju_jiaqu(void);
void AI_shengkong_jiaqu(void);
void AI_hongwai_chufa(void);
void AI_chumo_jiaqu(void);

/**
 * @函数描述: 传感器相关设备控制初始化
 * @return {*}
 */
void app_sensor_init(void)
{
	
	  ws2812b_init(WS2812B_ARR);
	  ws2812b_test(3);
    AI_chumo_init();         /* 触摸模块初始化 */
	  AI_hongwai_init();         /* 红外模块初始化 */
		AI_shengyin_init();         /* 声音模块初始化 */
    ultrasonic_sensor_init(); /* SR超声波初始化 */
	
    timer1_init(30000, 71);            /* 使能定时器，用于超声波计算 */


    // TCS34725_Init();/* 颜色传感器初始化 */
    if (TCS34725_Init())
    {
        printf("TCS34725_Init succeed\r\n");
    }
    else
    {
        printf("TCS34725_Init fail\r\n");
    }
}

/**
 * @函数描述: 循环检测输出传感器引脚的AD值
 * @return {*}
 */
void app_sensor_run(void)
{
   static u8 AI_mode_bak;

    // 有动作执行，直接返回
    if (group_do_ok == 0)
        return;

    if (AI_mode == 0)
    {
    }
    else if (AI_mode == 1)
    {
			  AI_yanse_shibie(); /* 颜色识别 */
    }
    else if (AI_mode == 2)
    {
        AI_dingju_jiaqu();  /* 定距夹取 */
    }
    else if (AI_mode == 4)
    {
				AI_shengkong_jiaqu(); /* 声控夹取 */
    }
    else if (AI_mode == 5)
    {
				AI_hongwai_chufa();		/* 红外触发 */

    }
    else if (AI_mode == 6)
    {
				AI_chumo_jiaqu();			/* 触摸夹取 */
    }
    if (AI_mode_bak != AI_mode)
    {
        AI_mode_bak = AI_mode;
        group_do_ok = 1;
    }
}

/**
 * @函数描述: 声控抓取
 * @return {*}
 */
void AI_shengkong_jiaqu(void)
{
   // 静态识别夹取
        if (shengyin_Read() == 0) /* 如果要用触摸传感器请判断为1 */
        {
            mdelay(5);
            if (shengyin_Read() == 0)
            {
                parse_cmd((u8 *)"$DGT:11-20,1!");
                beep_on_times(1, 100);
            }
        }

}			
/**
 * @函数描述: 红外触发
 * @return {*}
 */
void AI_hongwai_chufa(void)
{
   // 静态识别夹取
        if (hongwai_READ() == 0) /* 如果要用触摸传感器请判断为1 */
        {
            mdelay(5);
            if (hongwai_READ() == 0)
            {
                parse_cmd((u8 *)"$DGT:11-20,1!");
                beep_on_times(1, 100);
            }
        }
}
				/**
 * @函数描述: 触摸夹取
 * @return {*}
 */
void AI_chumo_jiaqu(void)
{
   // 静态识别夹取
        if (chumo_READ() == 1) /* 如果要用触摸传感器请判断为1 */
        {
            mdelay(5);
            if (chumo_READ() == 1)
            {
                parse_cmd((u8 *)"$DGT:2-10,1!");
                beep_on_times(1, 100);
            }
        }
}
/*************************************************************
函数名称：AI_dingju_jiaqu()
功能介绍：识别物体距离夹取物体
函数参数：无
返回值：  无
*************************************************************/
void AI_dingju_jiaqu(void)
{
    float adc_csb;
    if (group_do_ok == 0)
        return;
	
    adc_csb = sensor_sr_ultrasonic_read(); // 获取a0的ad值，计算出距离
    if ((adc_csb > 14.0) && (adc_csb <= 16.0))
    {
        adc_csb = sensor_sr_ultrasonic_read(); // 获取a0的ad值，计算出距离
        if ((adc_csb > 14.0) && (adc_csb <= 15.5))
        {
            adc_csb = sensor_sr_ultrasonic_read(); // 获取a0的ad值，计算出距离
            if ((adc_csb > 14.0) && (adc_csb <= 15.5))
            {
                // 距离15cm左右就夹取
                beep_on_times(1, 100);
                parse_cmd((u8 *)"$DGT:31-39,1!");
            }
        }
    }
}

/*************************************************************
函数名称：AI_yanse_shibie()
功能介绍：识别木块颜色，夹取分别放到不同位置
函数参数：无
返回值：  无
******************************************************** *****/
void AI_yanse_shibie(void)
{
    static u32 systick_ms_yanse = 0;

    if (group_do_ok && millis() - systick_ms_yanse > 20)
    {
        systick_ms_yanse = millis();
        TCS34725_LedON(0); /* 关闭颜色识别传感器LED */
        TCS34725_GetRawData(&rgb); // 获取RGB
        if (rgb.c < 1)
        {
            beep_on_times(1, 100);
            TCS34725_LedON(1); // 打开LED
            delay_ms(800);
            TCS34725_GetRawData(&rgb);          // 获取RGB
						if (rgb.r > rgb.g && rgb.r > rgb.b) /* 红 前 */
            {
							  ws2812b_test(1);
                parse_cmd((u8 *)"$DGT:2-10,1!"); // 执行脱机存储动作组
            }
            else if (rgb.g > rgb.r && rgb.g > rgb.b) /* 绿 右 */
            {
							  ws2812b_test(3);
                parse_cmd((u8 *)"$DGT:11-20,1!"); // 执行脱机存储动作组
            }
            else if (rgb.b > rgb.g && rgb.b > rgb.r) /* 蓝 左 */
            {
							  ws2812b_test(5);
                parse_cmd((u8 *)"$DGT:21-30,1!"); // 执行脱机存储动作组
            }
        }
    }
}

