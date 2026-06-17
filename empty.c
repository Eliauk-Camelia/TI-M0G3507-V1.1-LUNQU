/*
 * TI MSPM0G3507 综合例程: MPU6050 + OLED + TB6612电机 + 编码器
 * 按键(PA18)启停电机, OLED显示姿态角和编码器读数
 */

#include "ti_msp_dl_config.h"
#include "Hardware/board.h"
#include "Hardware/bsp_siic.h"
#include "Hardware/MPU6050.h"
#include <stdlib.h>

/* ---- Yaw 归零协议 ---- */
static volatile float   yaw_offset    = 0;
static volatile uint8_t yaw_calibed   = 0;
static volatile uint8_t yaw_reset_req = 0;
static uint8_t proto_idx = 0;
static char    proto_buf[20];

/* ---- 电机控制 ---- */
int Flag_Stop = 1;          /* 1=停止, 0=运行, 按键切换 */
int32_t encoderA_cnt, PWMA, encoderB_cnt, PWMB;

static void angle_split(float val, int32_t *inte, int32_t *deci)
{
    int32_t scaled;
    if (val < 0) {
        scaled = (int32_t)(-val * 10.f + 0.5f);
        *inte  = -(scaled / 10);
        *deci  = scaled % 10;
    } else {
        scaled = (int32_t)(val * 10.f + 0.5f);
        *inte  = scaled / 10;
        *deci  = scaled % 10;
    }
}

int main(void)
{
    pIICInterface_t siic = &User_sIICDev;

    SYSCFG_DL_init();

    /* ---- OLED ---- */
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, "System Init...");
    OLED_Refresh_Gram();

    /* ---- MPU6050 + DMP ---- */
    siic->init();
    MPU6050_initialize();
    DMP_Init();
    printf("MPU6050 DMP OK!\r\n");

    /* ---- 电机 PWM + 编码器 + 定时器 + 中断 ---- */
    DL_Timer_startCounter(PWM_0_INST);
    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);  /* PA7+PA25+PA26 */
    NVIC_ClearPendingIRQ(ENCODERB_INT_IRQN);              /* PB20+PB24 */
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(ENCODERB_INT_IRQN);
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    printf("Motor Init OK!\r\n");

    while (1)
    {
        int32_t deg_int, deg_dec;
        float   yaw_display;

        memset(OLED_GRAM, 0, 128 * 8 * sizeof(u8));

        /* 第一行: 标题 + 电机状态 */
        OLED_ShowString(0, 0, Flag_Stop ? "STOP" : "RUN ");

        /* 第二行: Pitch */
        OLED_ShowString(0, 16, "P:");
        angle_split(mpu6050.pitch, &deg_int, &deg_dec);
        OLED_ShowNumber(16, 16, deg_int, 4, 12);
        OLED_ShowChar(48, 16, '.', 12, 1);
        OLED_ShowNumber(56, 16, deg_dec, 1, 12);

        /* 第三行: Roll */
        OLED_ShowString(0, 32, "R:");
        angle_split(mpu6050.roll, &deg_int, &deg_dec);
        OLED_ShowNumber(16, 32, deg_int, 4, 12);
        OLED_ShowChar(48, 32, '.', 12, 1);
        OLED_ShowNumber(56, 32, deg_dec, 1, 12);

        /* Yaw 归零 */
        if (yaw_reset_req) { yaw_calibed = 0; yaw_reset_req = 0; }
        if (!yaw_calibed && mpu6050.yaw != 0) {
            yaw_offset = mpu6050.yaw;
            yaw_calibed = 1;
        }
        yaw_display = mpu6050.yaw - yaw_offset;
        if (yaw_display < 0)    yaw_display += 360;
        if (yaw_display >= 360) yaw_display -= 360;

        /* 第四行: Yaw + 编码器读数 */
        OLED_ShowString(0, 48, "Y:");
        angle_split(yaw_display, &deg_int, &deg_dec);
        OLED_ShowNumber(16, 48, deg_int, 4, 12);
        OLED_ShowChar(48, 48, '.', 12, 1);
        OLED_ShowNumber(56, 48, deg_dec, 1, 12);

        OLED_Refresh_Gram();
        LED_Flash(50);
        delay_ms(10);
    }
}

/* ---- 10ms 定时器: 电机PID控制 ---- */
void TIMER_0_INST_IRQHandler(void)
{
    if (DL_TimerA_getPendingInterrupt(TIMER_0_INST)) {
        if (DL_TIMER_IIDX_ZERO) {
            Key();  /* 按键扫描 */

            encoderA_cnt = Get_Encoder_countA;
            encoderB_cnt = -Get_Encoder_countB;
            Get_Encoder_countA = Get_Encoder_countB = 0;

            if (!Flag_Stop) {
                /* PI 速度闭环, 目标速度 15 编码器脉冲/10ms */
                PWMA = -Velocity_A(-15, encoderA_cnt);
                PWMB = -Velocity_B(-15, encoderB_cnt);
                PWMA = limit_PWM(PWMA, -7999, 7999);
                PWMB = limit_PWM(PWMB, -7999, 7999);
                Set_PWM(PWMA, PWMB);
            } else {
                Set_PWM(0, 0);
            }
        }
    }
}

/* ---- GROUP1 中断: MPU6050 DMP + 编码器 ---- */
void GROUP1_IRQHandler(void)
{
    /* MPU6050 DMP (PA7 下降沿) */
    uint32_t intp = DL_GPIO_getEnabledInterruptStatus(MPU6050_PORT,
        MPU6050_INT_PIN_PIN);
    if ((intp & MPU6050_INT_PIN_PIN) == MPU6050_INT_PIN_PIN) {
        Read_DMP();
        mpu6050.pitch = Roll;
        mpu6050.roll  = Pitch;
        mpu6050.yaw   = Yaw;
        mpu6050.gyro.x  = gyro[0];
        mpu6050.gyro.y  = gyro[1];
        mpu6050.gyro.z  = gyro[2];
        mpu6050.accel.x = accel[0];
        mpu6050.accel.y = accel[1];
        mpu6050.accel.z = accel[2];
        DL_GPIO_clearInterruptStatus(MPU6050_PORT, MPU6050_INT_PIN_PIN);
    }

    /* 编码器 (PA25/PA26 + PB20/PB24 双边沿) */
    Encoder_IRQ_Handler();
}

/* ---- UART: 协议解析 + 回显 ---- */
void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST))
    {
        case DL_UART_IIDX_RX: {
            char c = (char)DL_UART_Main_receiveData(UART_0_INST);
            DL_UART_transmitData(UART_0_INST, (uint8_t)c);
            if (c == '#') proto_idx = 0;
            if (proto_idx < sizeof(proto_buf) - 1) {
                proto_buf[proto_idx++] = c;
                proto_buf[proto_idx] = '\0';
            }
            if (c == '$') {
                if (strncmp(proto_buf, "#MPU6050:Y:", 11) == 0)
                    yaw_reset_req = 1;
                proto_idx = 0;
            }
            break;
        }
        default: break;
    }
}
