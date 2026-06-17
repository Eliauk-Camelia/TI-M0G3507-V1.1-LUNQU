# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

基于**轮趣科技 WHEELTEC C07A 核心板**（TI MSPM0G3507）的电赛准备项目。芯片为 LQFP-64 封装，主频最高 80MHz（通过内部 PLL 从 32MHz SYSOSC 倍频），ARM Cortex-M0+ 内核。

官方资料路径：`/home/arch/Downloads/baidu-downloads/WHEELTEC C07A核心板(Ti-MSPM0G3507)附送资料/`

## 开发环境

- **IDE**: TI Code Composer Studio (CCS) Theia
- **编译器**: TI ARM Clang Compiler (ticlang)
- **SDK**: MSPM0 SDK v2.01.00.03
- **配置工具**: SysConfig 1.20.0+ (通过 `empty.syscfg` 配置引脚和时钟)

## 构建与烧录

在 CCS 中打开项目（`File → Import → CCS Projects → 选择此目录`），点击 Build 编译，Debug 烧录。CLI 构建不可用（需 TI 专用工具链）。

## 项目结构

```
├── empty.c / empty.h        # 主程序
├── empty.syscfg             # SysConfig 引脚/时钟配置（JS DSL）
├── Hardware/                # 外设驱动层
│   ├── board.h / board.c    # 系统时钟、延时、printf 重定向
│   ├── oled.h / oled.c      # SSD1306 OLED 驱动（软件模拟 4 线 SPI）
│   ├── oledfont.h           # ASCII 字库 (12x6 / 16x8) + 中文点阵
│   └── led.h / led.c        # 板载 LED (PB9) 控制
├── Debug/
│   ├── ti_msp_dl_config.h   # SysConfig 生成：引脚宏定义、时钟频率
│   └── ti_msp_dl_config.c   # SysConfig 生成：初始化函数
└── targetConfigs/
    └── MSPM0G3507.ccxml     # 调试器配置 (XDS110)
```

## 关键架构

### 配置生成流程
`empty.syscfg` → SysConfig 工具 → `Debug/ti_msp_dl_config.h/c`

**重要**: 修改引脚或外设必须在 `empty.syscfg` 中配置（通过 CCS 的 SysConfig GUI），然后重新生成 `ti_msp_dl_config.*`。这两个文件标记为 `DO NOT EDIT`。如果直接编辑了它们，再次打开 SysConfig 时会丢失。

### OLED 引脚映射 (4 线软件 SPI)

| OLED 引脚 | MSPM0G3507 引脚 | SysConfig 名称 | 宏定义 |
|-----------|----------------|---------------|--------|
| SCL (时钟) | PA28 | OLED_SCL | `OLED_SCL_PIN_SCL_PIN` |
| SDA (数据) | PA31 | OLED_SDA | `OLED_SDA_PIN_SDA_PIN` |
| DC (命令/数据) | PB15 | OLED_DC | `OLED_DC_PIN_DC_PIN` |
| RST (复位) | PB14 | OLED_RST | `OLED_RST_PIN_RST_PIN` |

### OLED 驱动原理
OLED 驱动芯片为 SSD1306 (128×64)。通信采用**软件模拟 SPI**（通过 GPIO 位操作实现），而非硬件 SPI 外设。关键函数：
- `OLED_WR_Byte(dat, cmd)` — 写入一个字节，cmd=0 为命令，cmd=1 为数据
- `OLED_GRAM[128][8]` — 全局显存数组，128 列 × 8 页（每页 8 像素 = 64 行）
- `OLED_Refresh_Gram()` — 将显存整体刷新到 OLED
- `OLED_ShowString(x, y, str)` — 在指定坐标显示 ASCII 字符串（12x6 字体）

### 系统时钟
- SYSOSC 32MHz → PLL (×5, qDiv=4, pDiv=2) → CLK2X = 80MHz
- `CPUCLK_FREQ = 80000000` (定义在 `ti_msp_dl_config.h`)
- SysTick 配置为 16777216 周期（约 209.72ms 溢出），用于 delay_ms/us

### 延时实现
`board.c` 中的 `delay_ms/ delay_us` 基于 SysTick 的 `VAL` 寄存器（向下计数器）。`delay_ms` 实际调用 1000 次 `delay_us(ms)`。

### printf 重定向
`board.c` 中的 `fputc()` 将 printf 输出重定向到 UART0 (PA10=TX, PA11=RX)，波特率 115200。

## 外设一览

| 外设 | 引脚 | 用途 |
|------|------|------|
| UART0 | PA10(TX), PA11(RX) | printf 调试输出 |
| GPIO | PB9 | 板载 LED |
| GPIO | PA28, PA31, PB15, PB14 | OLED 显示屏 |
| SWD | PA19(SWDIO), PA20(SWCLK) | 调试接口 |

## 添加新外设的步骤

1. 在 CCS 中打开 `empty.syscfg`（SysConfig GUI）
2. 添加 GPIO / UART / I2C 等外设模块并绑定引脚
3. 保存，SysConfig 会自动重新生成 `Debug/ti_msp_dl_config.*`
4. 在 `Hardware/` 下创建对应的驱动文件
5. 在 `empty.c` 中 include 并调用

## 已知注意事项

- 不要使用 ST-Link 烧录，会导致芯片锁死。使用板载 XDS110 或 J-Link
- OLED 的 DC 和 RST 引脚用 GPIO 模拟，非硬件 SPI
- `OLED_GRAM` 是全局显存，修改后必须调用 `OLED_Refresh_Gram()` 才会显示
- `memset(OLED_GRAM, 0, ...)` 后需要 `OLED_Refresh_Gram()` 才能清屏
