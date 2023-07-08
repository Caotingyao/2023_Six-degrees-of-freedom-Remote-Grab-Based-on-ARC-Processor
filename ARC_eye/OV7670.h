#ifndef __OV7670_H
#define __OV7670_H
#include "SCCB.h"
#include "OV7670_config.h"

#define OV7670_PORT_DATA     DFSS_GPIO_8B2_ID
#define OV7670_PORT_CTL      DFSS_GPIO_4B2_ID
#define OV7670_MASK_DATA     (0xff)     //io4-11  data0-7
#define OV7670_MASK_RCK      (0x01)     //io0    RCK
#define OV7670_MASK_WREN     (0x01<<1)  //io1    WR
#define OV7670_MASK_WRST     (0x01<<2)  //io2    WRST
#define OV7670_MASK_RRST     (0x01<<3)  //io3    RRST
#define OV7670_MASK_INTR	 (0x01<<0)  //scl 4b_1(0)  VS

#define FRAME 10 

static DEV_GPIO *ov7670_data;  // fifo port
static DEV_GPIO *ov7670_ctl;   // control signal port
static DEV_GPIO *ov7670_int;   // interrupt  port

static uint8_t ov_state=0;     // record the frame after running

uint16_t CAMERA_BUF[1024];     // camera buf


void OV7670_Init(void);
void VS_Intr_init();     // Interrupt Init
void OV7670_Port_Init(); 
void FIFO_W_WREN(uint8_t BitValue);  //write wren    BitValue = 0 or 1
void FIFO_W_RCK(uint8_t BitValue);   //write RCK
void FIFO_W_WRST(uint8_t BitValue);  //write WRST
void FIFO_W_RRST(uint8_t BitValue);  //write RRST
void FIFO_W_OE(uint8_t BitValue);    //write OE
uint8_t FIFO_R_DATA();               // write data[0-7] to fifo
uint16_t OV7670_FIFO_OUT(uint8_t sel);      // read data[0-7]( half pix)
void IRQHandler();                   // interrupt process function
void data_collection();              /* get picture from camera */

#endif
