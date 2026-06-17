# TI-M0G3507 电赛小车

基于**轮趣科技 WHEELTEC C07A 核心板**（TI MSPM0G3507, Cortex-M0+, 80MHz）的综合例程。底板 S28A，集成 MPU6050 姿态解算、TB6612 双电机 PI 速度闭环、SSD1306 OLED 显示、感为灰度传感器巡线。

## 硬件

| 模块 | 型号/方案 | 接口 |
|------|-----------|------|
| 主控 | WHEELTEC C07A (MSPM0G3507, LQFP-64) | — |
| 底板 | S28A | — |
| IMU | MPU6050 (InvenSense, DMP 姿态解算) | I2C0 (PA0/PA1, 400kHz) + PA7 中断 |
| 电机驱动 | TB6612 双 H 桥 | TIMA1 PWM (PB2/PB3, 10kHz) + GPIO 方向 (PA13/14, PA16/17) |
| 编码器 | MG513 ×2 | GPIO 中断 (PA25/26, PB20/24) |
| 显示 | SSD1306 OLED (128×64) | 软件 SPI (PA28/PA31/PB14/PB15) |
| 灰度 | 感为 2 线串行 | GPIO (PA24 CLK, PA27 DAT) |
| 按键 | 单击启停电机 | PA18 |
| 调试串口 | UART0 (115200) | PA10 TX, PA11 RX |

## 功能

- **姿态显示**：OLED 实时显示 Pitch / Roll / Yaw（DMP 200Hz 更新）
- **Yaw 归零**：串口发送 `#MPU6050:Y:0$` 一键归零偏航角
- **电机 PI 闭环**：增量式 PI 速度控制，10ms 控制周期
- **按键启停**：PA18 单击切换电机运行/停止
- **灰度巡线**：串口 printf 输出灰度传感器读数（500ms 周期）
- **板载 LED**：PB9 周期性闪烁，指示系统运行

## 开发环境

- **IDE**：TI Code Composer Studio (CCS) Theia 20.5.1
- **编译器**：TI ARM Clang Compiler 4.0.4.LTS (ticlang)
- **SDK**：MSPM0 SDK 2.10.00.04
- **配置工具**：SysConfig 1.26.2
- **调试器**：XDS110 (SWD)

## 构建和烧录

1. 用 CCS 打开项目，点击 **Build**（不要 Clean）
2. 点击 **Debug** 烧录并启动调试（XDS110 SWD 模式）
3. **不要用 ST-Link 或 Uniflash BSL 模式烧录**

> SysConfig 1.26.2 不直接支持 MSPM0G3507 器件，项目用 `--board "/ti/boards/LP_MSPM0G3507"` 替代。详见 `CLAUDE.md`。

## 串口协议

输出（100ms 周期）：
```
#MPU6050:Y:12.5$
```

输入命令：
```
#MPU6050:Y:0$    → Yaw 归零
```

`$` 为帧结束符，`#` 为帧起始符。

## 目录结构

```
├── empty.c                  # 主程序
├── empty.syscfg             # SysConfig 引脚/时钟/外设配置
├── Hardware/                # 外设驱动层
│   ├── board.c/h            # 系统时钟、delay、printf 重定向
│   ├── motor.c/h            # TB6612 PWM + 增量式 PI
│   ├── encoder.c/h          # MG513 编码器
│   ├── MPU6050.c/h          # 姿态传感器封装
│   ├── inv_mpu*.c/h         # InvenSense I2C + DMP 底层
│   ├── oled.c/h             # SSD1306 OLED 驱动
│   ├── key.c/h              # 按键扫描
│   ├── grey.c/h             # 灰度传感器
│   ├── led.c/h              # 板载 LED
│   └── bsp_iic.c/siic.h     # I2C 接口抽象
├── Debug/                   # CCS 构建输出（含 SysConfig 生成文件）
└── targetConfigs/           # XDS110 调试配置
```

## 参考

- 官方资料：`~/Downloads/baidu-downloads/WHEELTEC C07A核心板(Ti-MSPM0G3507)附送资料/`
- 轮趣科技：https://wheeltec.net
- 详细开发说明见 `CLAUDE.md`
