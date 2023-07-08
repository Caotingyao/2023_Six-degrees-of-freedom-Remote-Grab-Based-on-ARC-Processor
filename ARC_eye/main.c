/*
 * @ Author:	TianXianOF
 * @ Email:		810246013@qq.com
 * @ Company:	XIDIAN University
 * @ Date:		2023-06-05 14:34:43
 * @ LastEditTime:		2023-06-19 14:35:00
 * @ LastEditors:		TianXianOF
 * @ Module Name:		
 * @ Function:			
 */

#include "embARC.h"
#include "embARC_debug.h"
#include "SCCB.h"
#include "OV7670.h"
#include "cifar10.h"
#include "RGB565to888.h"
#include "OV7670_config.h"

#define SENDPORT_MASK1  (0x01<<1)  /*gpio mask1*/
#define SENDPORT_MASK2  (0x01<<2)  /*gpio mask2*/

static DEV_GPIO *send_port;       
uint8_t undertest_data[1024][3];  // camera data buff
void data_change(uint16_t * in_buf) /* change RGB565 pixel to RGB888 pixel*/
{
	uint8_t temp[3];    /*3 color R G B */
	for(int i=0;i<1024;i++)
	{
		Rgb565toRgb888(in_buf[i],temp); 
		undertest_data[i][0] = temp[0];
		undertest_data[i][1] = temp[1];
		undertest_data[i][2] = temp[2];
	}
}

void send_id_init(){     /* initial the gpio of send module */
	send_port = gpio_get_dev(DFSS_GPIO_8B1_ID);
    if(send_port->gpio_open(0x01<<2) == E_OPNED){
        send_port->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,(void *)SENDPORT_MASK2);
        send_port->gpio_control(GPIO_CMD_DIS_BIT_INT,(void *)SENDPORT_MASK2);
    }
	if(send_port->gpio_open(0x01<<1) == E_OPNED){
	send_port->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,(void *)SENDPORT_MASK1);
	send_port->gpio_control(GPIO_CMD_DIS_BIT_INT,(void *)SENDPORT_MASK1);
    }
}
void send_id(uint8_t id){  /* sample 3 kind of picture to test */
	if(id == 3){             //rechange the id 3 to 01 
		GPIO_Write(1,send_port,1);		
		GPIO_Write(0,send_port,2);
	}
	else if(id == 0){        //rechange the id 0 to 10 
		GPIO_Write(0,send_port,1);		
		GPIO_Write(1,send_port,2);
	}
	// else GPIO_Write(0,send_port,1);
	// if(id&0x02) 
	else if(id == 8){        //rechange the id 8 to 11
		GPIO_Write(1,send_port,1);		
		GPIO_Write(1,send_port,2);
	}
	else {                   // idle :   00
		GPIO_Write(0,send_port,1);		
		GPIO_Write(0,send_port,2);
	}

}
void  identifier_init(void)
{
	OV7670_Init();   /* OV7670 initial*/
	send_id_init();  /* initial the gpio of send module */
	VS_Intr_init();  /* initial the interrupt pin VS and control pin OE */
	int_handler_install(52, IRQHandler); /*interrupt process function*/
	int_pri_set(52, INT_PRI_MIN);        /* set the interrupt privity */
}


int main()
{
	uint8_t result = 0;
	cpu_lock();
	board_init(); /* board init */
	cpu_unlock(); /* unlock system */
	identifier_init();  /*identifier initial */
	printf("Init successfully\n");

	while(1)
	{
		result = 0;
		data_collection();  /* get picture from camera */
		Delay_us(10);
		data_change(CAMERA_BUF);  /* change RGB565 pixel to RGB888 pixel*/
		result = net_process(undertest_data); /* put the data into network*/
		printf("the max id is %x \n",result); 
		send_id(result);            /*send the identifier id to ARC2*/
		Delay_ms(500);
	}
}


