#include "OV7670.h"
#define TEST 0     /* switch of test:    1: test     0:run */

void VS_Intr_init()  /*interrupt initial  and OE-- control pin */
{
	ov7670_int = gpio_get_dev(SCCB_PORT_ID);  //same as the sccb port
	ov7670_int->gpio_open(0);
	ov7670_int->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT,(void *)(OV7670_MASK_INTR));
	ov7670_int->gpio_control(GPIO_CMD_ENA_BIT_INT,(void *)(OV7670_MASK_INTR));
	// OE init --------------- OE enable to write fifo
	ov7670_int->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,(void *)(0x01<<1));
}

void FIFO_W_WREN(uint8_t BitValue){   // fifo en
	GPIO_Write(BitValue,ov7670_ctl,1);
}
void FIFO_W_WRST(uint8_t BitValue){   // fifo write rst
	GPIO_Write(BitValue,ov7670_ctl,2);
}
void FIFO_W_RCK(uint8_t BitValue){    // fifo read clk
	GPIO_Write(BitValue,ov7670_ctl,0);
}
void FIFO_W_RRST(uint8_t BitValue){   // fifo read rst
	GPIO_Write(BitValue,ov7670_ctl,3);
}
void FIFO_W_OE(uint8_t BitValue){
	GPIO_Write(BitValue,ov7670_int,1);
}
uint8_t FIFO_R_DATA(){                // read the data from fifo    
	uint32_t temp;
	ov7670_data->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(OV7670_MASK_DATA));
	ov7670_data->gpio_read(&temp,OV7670_MASK_DATA);
	return (uint8_t)(temp&OV7670_MASK_DATA);
}

uint16_t OV7670_FIFO_OUT(uint8_t sel)  //FIFO  to  MCU/LCD   
{                        // sel = 1 return data
	uint16_t color;      // sel = 0 just make the timing not (receive data and return )
	if(sel){
		FIFO_W_RCK(0);
		Delay_us(1);
		color = FIFO_R_DATA(); 
		FIFO_W_RCK(1);
		color = color << 8;
		FIFO_W_RCK(0);
		Delay_us(1);
		color = color | FIFO_R_DATA();
		FIFO_W_RCK(1);
		return color;
	}else {
		FIFO_W_RCK(0);
		Delay_us(2);
		FIFO_W_RCK(1);
		Delay_us(1);
		FIFO_W_RCK(0);
		Delay_us(2);
		FIFO_W_RCK(1);
		return 0;
	}

}

void OV7670_Port_Init()     /* data port, ctrl port initial  */
{	
	ov7670_data = gpio_get_dev(OV7670_PORT_DATA);  //ARC: gpio8b2 0-7
	ov7670_ctl = gpio_get_dev(OV7670_PORT_CTL);    //ARC: gpio4b2 0-3
	ov7670_data->gpio_open(OV7670_MASK_DATA);
	ov7670_data->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT,(void*)OV7670_MASK_DATA);
	ov7670_ctl->gpio_open(0);
	ov7670_data->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT,(void*)(0x0f));

}


void OV7670_RegExample(void)  //config the ov7670 (example)
{
	for(int i=0;i<sizeof(OV7670_reg)/sizeof(OV7670_reg[0]);i++)
	{
		SCCB_W_Reg(OV7670_reg[i][0],OV7670_reg[i][1]);
	}
}

void OV7670_Configure(void)
{
	SCCB_W_Reg(0x12,0x80);  // reset the sys
	Delay_s(2);
	
	OV7670_RegExample();  //config the ov7670 (example)

	#if TEST == 1   //test ---- 8 color band
	SCCB_W_Reg(0x71, 0xB5);  
	SCCB_W_Reg(0x70, 0x3A);
	#endif
}

// OV7670 Init-- sccb , port and  configure
void OV7670_Init(void)
{
	SCCB_Init();//SCCB Init
	OV7670_Port_Init(); 
	OV7670_Configure();
}


void IRQHandler(){        // interrupt process function
	int_disable(52);      //disable the interrupt
	
	if(ov_state < FRAME){
		if(ov_state == FRAME-2){  //reset
			FIFO_W_RRST(0);
			Delay_us(5);
			FIFO_W_WRST(1);
			Delay_us(5);
			FIFO_W_WREN(1);
		}
		else if(ov_state == FRAME-1){ //start write data to fifo
			// FIFO_W_WREN(0);    //write enable
			// Delay_us(1);
			FIFO_W_WREN(0);    //write enable
			Delay_us(5);
			FIFO_W_WRST(0);
			Delay_us(5);
			FIFO_W_WRST(1);
			//Delay_us(1);
		}

		ov_state++;
	}else{
		FIFO_W_WREN(1);        //write disable
	}
}

void data_collection(){        /* get picture(image) from camera */
	uint32_t i,j;
	uint16_t temp;
	while(1){ // until the frame == FRAME, start run collection
		Delay_ms(1);
		if(ov_state == FRAME) break;
		else int_enable(52);
	}
	// control signal 
	FIFO_W_OE(0);
	FIFO_W_RCK(0);
	FIFO_W_RRST(0);
	FIFO_W_RCK(0);
	FIFO_W_RCK(1);
	FIFO_W_RCK(0);
	FIFO_W_RRST(1);
	FIFO_W_RCK(1);
	
	// data collection
	for(i=0;i<240;i++){
		for(j=0;j<320;j++){
			if(i>=88 && i<152) // sample 64*64 
				if(j>=128 && j<192){
					temp = OV7670_FIFO_OUT(1);
					if(i%2 && j%2) {            // convert to 32*32
						//printf("%4x",temp);
						CAMERA_BUF[((i-88)/2)*32+((j-128)/2)] = temp;
					}
				}
				else OV7670_FIFO_OUT(0);  // not (receive data and return )
			else OV7670_FIFO_OUT(0);      // just make the timing
		}
	}
	printf("\nFinish!!!\n");
	FIFO_W_RCK(0);
	FIFO_W_RCK(1);
	FIFO_W_RCK(0);
	FIFO_W_OE(1);  //disable
	ov_state = 0;  //clear the frame
}

