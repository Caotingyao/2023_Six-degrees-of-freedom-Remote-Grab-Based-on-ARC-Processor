#include "control.h"

//蓝牙连接指令
char frame1[50]="AT+TPMODE=1\r\n";
char frame2[50]="AT+LECONN=AB12C44D33F10\r\n";
char frame4[30]="$KMS:0,100,200,500!";
//分拣指令
char Left_get[30]="$DGT:1-7,1!";
char Right_get[30]="$DGT:8-14,1!";
char Front_get[30]="$DGT:15-20,1!";
uint8_t Left_get_len = 11;
uint8_t Right_get_len = 12;
uint8_t Front_get_len = 13;

DEV_UART *dev_uart1 = NULL; //set a pointer of uart
DEV_UART *dev_uart2 = NULL; //set a pointer of uart

//图片识别ID
uint8_t id = 0;

void ID_Port_Init(void)
{
    id_port = gpio_get_dev(ID_PORT_ID);
    
    if(id_port->gpio_open(0) == E_OPNED){
        id_port->gpio_control(ID_MASK2,(void *)(ID_MASK2));
        id_port->gpio_control(GPIO_CMD_DIS_BIT_INT,(void *)(ID_MASK2));
    }

    id_port = gpio_get_dev(ID_PORT_ID);
    if(id_port->gpio_open(0) == E_OPNED){
        id_port->gpio_control(0,(void *)(ID_MASK1));
        id_port->gpio_control(GPIO_CMD_DIS_BIT_INT,(void *)(ID_MASK1));
    }

}

uint8_t read_id()
{
    uint32_t BitValue;
    id_port->gpio_open(0x00);
	id_port->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT, (void *)(0x01<<2 | 0x01<<3));
    //Delay_us(10);
    id_port->gpio_read(&BitValue,0x01<<2 | 0x01<<3);
    
    return (uint8_t)((BitValue & (0x01<<2 | 0x01<<3)) >> 2);
}

int uart1_init(uint32_t baudrate)
{
  dev_uart1 = uart_get_dev(DFSS_UART_1_ID); //set uart port as iot uart 1

  if (dev_uart1 == NULL)
    return -1;

  if (dev_uart1->uart_open(baudrate) == E_OPNED) //if uart was opened before this time,and the baudrate was different
  {
    dev_uart1->uart_control(UART_CMD_SET_BAUD, (void *)(baudrate)); //forc modify the baudrate
  }

  return 0;
}

int uart2_init(uint32_t baudrate)
{
  dev_uart2 = uart_get_dev(DFSS_UART_2_ID); //set uart port as iot uart 1

  if (dev_uart2 == NULL)
    return -1;

  if (dev_uart2->uart_open(baudrate) == E_OPNED) //if uart was opened before this time,and the baudrate was different
  {
    dev_uart2->uart_control(UART_CMD_SET_BAUD, (void *)(baudrate)); //forc modify the baudrate
  }

  return 0;
}

void System_init()
{
  //系统初始化
  ID_Port_Init();
  cpu_lock();   /* lock cpu to do initializations */
  board_init(); /* board level init */
  cpu_unlock(); /* unlock cpu to let interrupt work */
  uart1_init(UART_BAUDRATE_115200); // set master baudrate
  uart2_init(UART_BAUDRATE_115200); // set master baudr
}

void Bluetooth_connect()
{
  char a[1]=")";
  frame1_len = strlen(frame1);
  frame2_len = strlen(frame2);
  dev_uart1->uart_write(frame1, (uint32_t)(frame1_len));
  delay_s(1);
  dev_uart1->uart_write(frame2, (uint32_t)(frame2_len));
  delay_s(5);
  dev_uart1->uart_write(a, 1);
}

void Automatic_sorting()
{
    id = read_id();
    EMBARC_PRINTF("ID: %d \r\n", id);
    if (id == 1)
    {
       dev_uart2->uart_write(Left_get, (uint32_t)(Left_get_len));
       delay_s(10);
    }
    else if (id == 2)
    {
      dev_uart2->uart_write(Right_get, (uint32_t)(Right_get_len));
      delay_s(10);
    }
    else if (id == 3)
    {
      dev_uart2->uart_write(Front_get, (uint32_t)(Front_get_len));
      delay_s(10);
    }
}

void Gesture_control()
{
    dev_uart1->uart_read(byte, (uint32_t)(1));
    if (byte[0] == 0x11)
    {
      //提取包内有效数据，即三轴坐标
      for (int i=1; i<13; i++)
      {
        dev_uart1->uart_read(byte+i, (uint32_t)(1));
      }
      X = (S16)(((S16)byte[8]<<8) | byte[7]); // x坐标
      Y = (S16)(((S16)byte[10]<<8) | byte[9]); // y坐标
      Z = (S16)(((S16)byte[12]<<8) | byte[11]); // z坐标
      //过滤异常数据
      if (X>350 | X<-350 | Y<-350 | Y>350 | Z>350 | Z<-350)
      {
        X = 0;
        Y = 0;
        Z = 0;
      }
      //sprintf(z, "%-3d", Z);
      //sprintf(y, "%-3d", Y);
      //sprintf(x, "%-3d", X);
      //计算传感器位移
      X_dif = X<X2? X2-X : X-X2;
      Y_dif = Y<Y2? Y2-Y : Y-Y2;
      Z_dif = Z<Z2? Z2-Z : Z-Z2;
      //取三轴中的最大位移叠加到机械臂末端坐标上，计算新的机械臂坐标
      if ( X_dif >= Y_dif )
      {
        if ( X_dif >= Z_dif )
          X_new = X_old + X - X2;
        else
          Z_new = Z_old + Z - Z2;
      }
      else if ( Y_dif >= Z_dif )
      {
        Y_new = Y_old + Y - Y2;
      }
      else
      {
        Z_new = Z_old + Z - Z2;
      }
      //保存当前传感器坐标，用于计算下一时刻位移
      X2 = X;
      Y2 = Y;
      Z2 = Z;
      //传感器动作结束且位移满足约束方程则根据当前坐标发送机械臂运动指令，否则认为动作未结束，持续更新坐标。
      sprintf(frame4, "$KMS:%d,%d,%d,500!", X_old, Y_old, Z_old);
      if ( X==0 && Y==0 && Z==0 || ((X_new*X_new+Y_new*Y_new+Z_new*Z_new)>350*350) || X_new<0 || Y_new<0 || Z_new<20)
      {
        frame4_len = strlen(frame4);
        dev_uart2->uart_write(frame4, (uint32_t)(frame4_len));
      }
      else
      {
        X_old = X_new;
        Y_old = Y_new;
        Z_old = Z_new;
      }
      //id = read_id();
      EMBARC_PRINTF("坐标: x=%s, y=%s, z=%s \r\n", x, y, z);
      //MBARC_PRINTF(frame3);
      //EMBARC_PRINTF(frame4);
    }
}