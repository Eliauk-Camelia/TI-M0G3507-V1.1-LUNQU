#include "motor.h"
float Velcity_Kp=1.0,  Velcity_Ki=0.4,  Velcity_Kd; //相关速度PID参数
/***********************************************
公司：轮趣科技（东莞）有限公司
品牌：WHEELTEC
官网：wheeltec.net
淘宝店铺：shop114407458.taobao.com 
速卖通: https://minibalance.aliexpress.com/store/4455017
版本：V1.0
修改时间：2024-07-019

Brand: WHEELTEC
Website: wheeltec.net
Taobao shop: shop114407458.taobao.com 
Aliexpress: https://minibalance.aliexpress.com/store/4455017
Version: V1.0
Update：2024-07-019

All rights reserved
***********************************************/
int limit_PWM(int value,int low,int high)
{
	if(value>high) return high;
	else if(value<low) return low;
	else return value;
}


void Set_PWM(int pwmA,int pwmB)
{
	 if(pwmA>0)
    {
        DL_GPIO_setPins(AIN_PORT,AIN_AIN2_PIN);
        DL_GPIO_clearPins(AIN_PORT,AIN_AIN1_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmA),GPIO_PWM_0_C0_IDX);
    }
    else
    {
        DL_GPIO_setPins(AIN_PORT,AIN_AIN1_PIN);
        DL_GPIO_clearPins(AIN_PORT,AIN_AIN2_PIN);
		DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmA),GPIO_PWM_0_C0_IDX);
    }
    if(pwmB>0)
    {
		DL_GPIO_setPins(BIN_PORT,BIN_BIN2_PIN);
        DL_GPIO_clearPins(BIN_PORT,BIN_BIN1_PIN);
        DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmB),GPIO_PWM_0_C1_IDX);
    }
    else
    {
		DL_GPIO_setPins(BIN_PORT,BIN_BIN1_PIN);
        DL_GPIO_clearPins(BIN_PORT,BIN_BIN2_PIN);
		 DL_Timer_setCaptureCompareValue(PWM_0_INST,ABS(pwmB),GPIO_PWM_0_C1_IDX);
    }
   

}

/***************************************************************************
函数功能：电机的PID闭环控制
入口参数：左右电机的编码器值
返回值  ：电机的PWM
***************************************************************************/
int Velocity_A(int TargetVelocity, int CurrentVelocity)
{  
    int Bias;  //定义相关变量
	static int ControlVelocityA, Last_biasA; //静态变量，函数调用结束后其值依然存在
	Bias=TargetVelocity-CurrentVelocity; //求速度偏差
	ControlVelocityA+=Velcity_Ki*(Bias-Last_biasA)+Velcity_Kp*Bias;  //增量式PI控制器
                                                               //Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
	                                                             //Velcity_Ki*Bias             速度控制值由Bias不断积分得到 偏差越大加速度越大
	Last_biasA=Bias;	
	if(ControlVelocityA>7000) ControlVelocityA=7000;
	else if(ControlVelocityA<-7000) ControlVelocityA=-7000;
	return ControlVelocityA; //返回速度控制值
}

/***************************************************************************
函数功能：电机的PID闭环控制
入口参数：左右电机的编码器值
返回值  ：电机的PWM
***************************************************************************/
int Velocity_B(int TargetVelocity, int CurrentVelocity)
{  
	int Bias;  //定义相关变量
	static int ControlVelocityB, Last_biasB; //静态变量，函数调用结束后其值依然存在
	Bias=TargetVelocity-CurrentVelocity; //求速度偏差
	ControlVelocityB+=Velcity_Ki*(Bias-Last_biasB)+Velcity_Kp*Bias;  //增量式PI控制器
                                                               //Velcity_Kp*(Bias-Last_bias) 作用为限制加速度
	                                                            //Velcity_Ki*Bias             速度控制值由Bias不断积分得到 偏差越大加速度越大
	Last_biasB=Bias;	
	if(ControlVelocityB>7000) ControlVelocityB=7000;
	else if(ControlVelocityB<-7000) ControlVelocityB=-7000;
	return ControlVelocityB; //返回速度控制值
}

