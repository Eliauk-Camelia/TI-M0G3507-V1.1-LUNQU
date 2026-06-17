/*
 * TI MSPM0G3507 OLED 显示例程
 * 驱动 SSD1306 128x64 OLED 显示屏
 *
 * OLED 接线 (参考 WHEELTEC C07A 接线说明):
 *   SCL -> PA28
 *   SDA -> PA31
 *   DC  -> PB15
 *   RST -> PB14
 *   VCC -> 3.3V
 *   GND -> GND
 */

#include "Hardware/board.h"

uint32_t counter = 0;

int main(void)
{
    SYSCFG_DL_init();

    OLED_Init();
    printf("OLED Init OK!\r\n");

    while (1)
    {
        /* 清空显存 */
        memset(OLED_GRAM, 0, 128 * 8 * sizeof(u8));

        /* 第一行: Hello World */
        OLED_ShowString(0, 0, "Hello World!");

        /* 第二行: 显示计数器数值 */
        OLED_ShowString(0, 16, "Count:");
        OLED_ShowNumber(48, 16, counter, 6, 12);

        /* 刷新到 OLED 屏幕 */
        OLED_Refresh_Gram();

        counter++;

        /* LED闪烁 — 系统运行指示 */
        LED_Flash(100);

        delay_ms(100);
    }
}
