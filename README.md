# Six degrees of freedom Remote Grab Based on ARC Processor

## 文档说明：
	ARC_ctr：
		内容：负责运动控制的ARC板卡的代码。
		功能：接收蓝牙信号或视觉识别结果，计算运动轨迹，发送运动指令给机械臂驱动板卡。

	ARC_eye：
		内容：负责视觉识别的ARC板卡的代码。
		功能：接收摄像头输入，预处理后传入卷积神经网络，获得识别结果，将结果ID发送给运动控制ARC板卡。

	Robot_arm_drive:
		内容：运动指令解析，运动学逆解以及机械臂驱动代码。
		功能：接收ARC运动控制板卡的运动指令，逆解出舵机传动角度，驱动机械臂完成指定动作。
  
## 流程图：



![流程](https://github.com/Caotingyao/2023_Six-degrees-of-freedom-Remote-Grab-Based-on-ARC-Processor/assets/135319814/119de3c8-0455-4d63-a734-e08d7e0210b3)

