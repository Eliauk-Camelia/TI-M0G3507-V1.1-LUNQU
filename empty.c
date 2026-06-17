/*
 * TI MSPM0G3507 综合例程: MPU6050 + OLED + TB6612电机 + 编码器 + 灰度
 * OLED 四行布局: 状态+灰度 | Pitch | Roll | Yaw+编码器
 * 按键(PA18)启停电机, 串口协议 Yaw 归零
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

    /* ---- 灰度传感器 ---- */
    grey_init();
    printf("Grey Init OK!\r\n");

    while (1)
    {
        int32_t deg_int, deg_dec;
        float   yaw_display;
        uint8_t grey_val;

        memset(OLED_GRAM, 0, 128 * 8 * sizeof(u8));

        /* 读取灰度传感器 (每帧更新) */
        grey_val = grey_read_digital();

        /* ---- 第一行 (y=0): 状态 + 灰度8路二进制 ---- */
        OLED_ShowString(0, 0, Flag_Stop ? "STOP" : "RUN ");
        OLED_ShowString(36, 0, "G:");
        for (uint8_t i = 0; i < 8; i++) {
            OLED_ShowChar(48 + i * 6, 0,
                (grey_val & (1 << i)) ? '1' : '0', 12, 1);
        }

        /* ---- 第二行 (y=16): Pitch ---- */
        OLED_ShowString(0, 16, "P:");
        angle_split(mpu6050.pitch, &deg_int, &deg_dec);
        if (deg_int < 0) {
            OLED_ShowChar(16, 16, '-', 12, 1);
            OLED_ShowNumber(24, 16, (uint32_t)(-deg_int), 3, 12);
        } else {
            OLED_ShowNumber(16, 16, (uint32_t)deg_int, 4, 12);
        }
        OLED_ShowChar(48, 16, '.', 12, 1);
        OLED_ShowNumber(56, 16, (uint32_t)deg_dec, 1, 12);

        /* ---- 第三行 (y=32): Roll ---- */
        OLED_ShowString(0, 32, "R:");
        angle_split(mpu6050.roll, &deg_int, &deg_dec);
        if (deg_int < 0) {
            OLED_ShowChar(16, 32, '-', 12, 1);
            OLED_ShowNumber(24, 32, (uint32_t)(-deg_int), 3, 12);
        } else {
            OLED_ShowNumber(16, 32, (uint32_t)deg_int, 4, 12);
        }
        OLED_ShowChar(48, 32, '.', 12, 1);
        OLED_ShowNumber(56, 32, (uint32_t)deg_dec, 1, 12);

        /* Yaw 归零 */
        if (yaw_reset_req) { yaw_calibed = 0; yaw_reset_req = 0; }
        if (!yaw_calibed && mpu6050.yaw != 0) {
            yaw_offset = mpu6050.yaw;
            yaw_calibed = 1;
        }
        yaw_display = mpu6050.yaw - yaw_offset;
        if (yaw_display < 0)    yaw_display += 360;
        if (yaw_display >= 360) yaw_display -= 360;

        /* ---- 第四行 (y=48): Yaw + 编码器A/B ---- */
        OLED_ShowString(0, 48, "Y:");
        angle_split(yaw_display, &deg_int, &deg_dec);
        OLED_ShowNumber(12, 48, (uint32_t)deg_int, 3, 12);
        OLED_ShowChar(30, 48, '.', 12, 1);
        OLED_ShowNumber(36, 48, (uint32_t)deg_dec, 1, 12);

        /* E1: 编码器A (x=48, 最多3位) */
        OLED_ShowString(48, 48, "E1:");
        if (encoderA_cnt < 0) {
            OLED_ShowChar(66, 48, '-', 12, 1);
            OLED_ShowNumber(72, 48, (uint32_t)(-encoderA_cnt), 2, 12);
        } else {
            OLED_ShowNumber(66, 48, (uint32_t)encoderA_cnt, 3, 12);
        }

        /* E2: 编码器B (x=84, 最多3位) */
        OLED_ShowString(84, 48, "E2:");
        if (encoderB_cnt < 0) {
            OLED_ShowChar(102, 48, '-', 12, 1);
            OLED_ShowNumber(108, 48, (uint32_t)(-encoderB_cnt), 2, 12);
        } else {
            OLED_ShowNumber(102, 48, (uint32_t)encoderB_cnt, 3, 12);
        }

        OLED_Refresh_Gram();
        LED_Flash(50);

        /* 灰度传感器: 每500ms串口输出一次 */
        {
            static uint16_t grey_cnt = 0;
            if (++grey_cnt >= 50) {
                grey_cnt = 0;
                printf("Grey:0x%02X\r\n", grey_val);
            }
        }

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
