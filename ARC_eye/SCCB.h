#ifndef __SCCB_H
#define __SCCB_H
#include "sys_constant.h"
#define SCCB_PORT_ID	    (DFSS_GPIO_4B1_ID)

#define SCCB_MASK_SCL	    ((0x1) << 2)  //scl gpio4b_1(2)
#define SCCB_MASK_SDA	    ((0x1) << 3)  //sda gpio4b_1(3)

#define SCCB_ID             0x42 /* camera address*/
#define UNIT1               5    /* time uint1 */
#define UNIT2               10   /* time uint2 */


static DEV_GPIO *sccb_port;

void GPIO_Write(uint8_t BitValue, DEV_GPIO *port_ptr, uint8_t num);   /* GPIO write (one pin)*/
uint8_t GPIO_Read(DEV_GPIO *port_ptr, uint8_t num);   /* GPIO read (one pin)*/

void SCCB_W_SDA(uint8_t BitValue);    /* write the sda */
void SCCB_W_SCL(uint8_t BitValue);    /* write the scl */
uint8_t SCCB_R_SDA(void);             /* read the sda */


void     SCCB_Init(void);             /* SCCB initial */
void     SCCB_Start(void);            /* start serials */
void     SCCB_Stop(void);             /* stop serials */
void     SCCB_SendNA(void);           
uint8_t  SCCB_R_Byte(void);           /* read byte*/
uint8_t  SCCB_W_Byte(uint8_t Byte);   /* write byte*/

uint8_t  SCCB_W_Reg(uint8_t reg,uint8_t data); /*write reg:   reg is the address of reg*/
uint8_t  SCCB_R_Reg(uint8_t reg);              /*read reg:    reg is the address of reg*/



#endif
