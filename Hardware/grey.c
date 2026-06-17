/**
 * @file  grey.c
 * @brief 感为灰度传感器 串行接口驱动 (MSPM0)
 *
 * 2 线串行协议:
 *   CLK → PA24 (Grey_AD0), MCU 输出
 *   DAT → PA27 (Grey_DAT), MCU 输入 (外部上拉)
 *
 * 时序: CLK 下降沿 → 读 DAT → CLK 上升沿 → 传感器更新下一位
 * 8 个时钟周期读完 8 路探头的数字量 (bit0~bit7)
 *
 * 参考: 感为 STM32 例程 gw_gray_serial.c
 */

#include "ti_msp_dl_config.h"
#include "grey.h"

#define CLK_PORT  Grey_PORT
#define CLK_PIN   Grey_AD0_PIN        /* PA24, 输出 */

/* ~5μs 延迟 @80MHz */
#define SERIAL_DELAY  400

static void serial_delay(void)
{
    delay_cycles(SERIAL_DELAY);
}

/**
 * @brief 读取 8 路探头数字量 (串行协议)
 *
 * CLK 空闲高 → 拉低 → 读 DAT → 拉高 → 传感器准备下一位
 * bit0=探头1, bit7=探头8
 */
uint8_t grey_read_digital(void)
{
    uint8_t ret = 0;

    /* 首时钟丢弃: 辅助板首周期数据不稳定, 读完 9 位取后 8 位 */
    DL_GPIO_clearPins(CLK_PORT, CLK_PIN);
    serial_delay();
    DL_GPIO_setPins(CLK_PORT, CLK_PIN);
    serial_delay();

    for (uint8_t i = 0; i < 8; i++) {
        /* 下降沿: 锁存当前位 */
        DL_GPIO_clearPins(CLK_PORT, CLK_PIN);
        serial_delay();

        /* 读 DAT */
        if (DL_GPIO_readPins(Grey_PORT, Grey_DAT_PIN)) {
            ret |= (1 << i);
        }

        /* 上升沿: 准备下一位 */
        DL_GPIO_setPins(CLK_PORT, CLK_PIN);
        serial_delay();
    }

    return ret;
}

/**
 * @brief 初始化传感器 (串行模式)
 *
 * 发送特殊时序使传感器进入串行模式
 * CLK 拉低 100μs → 拉高 → 等待稳定
 */
void grey_init(void)
{
    /* 确保 CLK 初始为高 */
    DL_GPIO_setPins(CLK_PORT, CLK_PIN);
    delay_cycles(8000);  /* 100μs @80MHz */

    /* 发一个脉冲: 低→高, 唤醒传感器串行模式 */
    DL_GPIO_clearPins(CLK_PORT, CLK_PIN);
    delay_cycles(8000);  /* 100μs @80MHz */
    DL_GPIO_setPins(CLK_PORT, CLK_PIN);
    delay_cycles(80000); /* 1ms 稳定 */
}
