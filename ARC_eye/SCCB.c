#include "SCCB.h"

void GPIO_Write(uint8_t BitValue, DEV_GPIO *port_ptr, uint8_t num)  /* GPIO write (one pin)*/
{
    port_ptr->gpio_open(0x01<<num);
	port_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_OUTPUT, CONV2VOID(0x01<<num));
	port_ptr->gpio_write(BitValue << num, 0x01<<num);
}
uint8_t GPIO_Read(DEV_GPIO *port_ptr, uint8_t num)             /* GPIO read (one pin)*/
{
    uint32_t BitValue;
    port_ptr->gpio_open(0x00);
	port_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(0x01<<num));
    port_ptr->gpio_read(&BitValue,0x01<<num);
    return (uint8_t)((BitValue & (0x01 << num)) >> num);
}


void SCCB_W_SCL(uint8_t BitValue) /* write the scl */
{
    GPIO_Write(BitValue,sccb_port,2);
}
void SCCB_W_SDA(uint8_t BitValue) /* write the sda */
{
    GPIO_Write(BitValue,sccb_port,3);
}

uint8_t SCCB_R_SDA(void)    /* read the sda */
{
    return GPIO_Read(sccb_port,3);
}

void SCCB_Init(void) /* SCCB initial */
{
    sccb_port = gpio_get_dev(SCCB_PORT_ID);
    
    if(sccb_port->gpio_open(SCCB_MASK_SDA) == E_OPNED){
        sccb_port->gpio_control(SCCB_MASK_SDA,(void *)(SCCB_MASK_SDA));
        sccb_port->gpio_control(GPIO_CMD_DIS_BIT_INT,(void *)(SCCB_MASK_SDA));
    }

    sccb_port = gpio_get_dev(SCCB_PORT_ID);
    if(sccb_port->gpio_open(0x0) == E_OPNED){
        sccb_port->gpio_control(0,(void *)(SCCB_MASK_SCL));
        sccb_port->gpio_control(GPIO_CMD_DIS_BIT_INT,(void *)(SCCB_MASK_SCL));
    }

}


void SCCB_Start(void) /* start serials */
{
	SCCB_W_SDA(1);
	SCCB_W_SCL(1);
	Delay_us(UNIT1);
	SCCB_W_SDA(0);
	Delay_us(UNIT1);
	SCCB_W_SCL(0);
}
void SCCB_Stop(void)  /* stop serials */
{
    SCCB_W_SDA(0);
	Delay_us(UNIT1);
	SCCB_W_SCL(1);
	Delay_us(UNIT1);
	SCCB_W_SDA(1);
	Delay_us(UNIT1);
}
void SCCB_SendNA(void) /* no ack */
{
	Delay_us(UNIT1);
	SCCB_W_SDA(1);
	SCCB_W_SCL(1);
	Delay_us(UNIT1);
	SCCB_W_SCL(0);
	Delay_us(UNIT1);
	SCCB_W_SDA(0);//new
	Delay_us(UNIT1);
}

uint8_t SCCB_W_Byte(uint8_t Byte)  /* read byte*/
{
	uint8_t i,res;
    res = Byte;
	for (i = 0; i < 8; i ++)
	{	
        if(res&0x80) SCCB_W_SDA(1);
        else SCCB_W_SDA(0);
        res = res << 1;
		Delay_us(UNIT1);
		SCCB_W_SCL(1);
		Delay_us(UNIT1);
		SCCB_W_SCL(0);
	}
    Delay_us(UNIT1);
    SCCB_W_SCL(1);
    Delay_us(UNIT1);
    if(SCCB_R_SDA())res = 1;   //receive 9 bit  if(bit ==1) send fail
    else res = 0;
    SCCB_W_SCL(0);
    return res;
}
uint8_t SCCB_R_Byte(void)   /* write byte*/
{
	uint8_t i, temp = 0;
	for (i = 8; i > 0; i--)
	{
        Delay_us(UNIT1);
		SCCB_W_SCL(1);
        temp = temp << 1;
		if (SCCB_R_SDA()) {
            temp++;
            printf("test: temp[%d] = 1\n",i-1);
        } 
        else printf("test: temp[%d] = 0\n",i-1);
        Delay_us(UNIT1);
		SCCB_W_SCL(0);
	}
	return temp;
}
uint8_t SCCB_W_Reg(uint8_t reg,uint8_t data)
{                                 /*write reg:   reg is the address of reg*/
    uint8_t res = 0;
    SCCB_Start();
    if(SCCB_W_Byte(SCCB_ID)) res = 1;
    Delay_us(UNIT2);
    if(SCCB_W_Byte(reg)) res = 1;
    Delay_us(UNIT2);
    if(SCCB_W_Byte(data)) res = 1;
    SCCB_Stop();
    return res;

}
uint8_t SCCB_R_Reg(uint8_t reg)
{                        /*read reg:    reg is the address of reg*/
    uint8_t val = 0;
    SCCB_Start();
    SCCB_W_Byte(SCCB_ID);
    Delay_us(UNIT2);
    SCCB_W_Byte(reg);
    Delay_us(UNIT2);
    SCCB_Stop();  
    Delay_us(UNIT2);
    ///////////
    SCCB_Start();
    SCCB_W_Byte(SCCB_ID|0x01);
    Delay_us(UNIT2);
    val = SCCB_R_Byte();
    SCCB_SendNA();
    SCCB_Stop();
    return val;
}




