# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

基于**轮趣科技 WHEELTEC C07A 核心板**（TI MSPM0G3507）的电赛准备项目。芯片 LQFP-64 封装，Cortex-M0+ 内核，主频 80MHz（PLL 从 32MHz SYSOSC 倍频）。底板为 S28A，搭载 TB6612 双电机驱动、MPU6050 六轴传感器、SSD1306 OLED、感为灰度传感器。

官方资料路径：`/home/arch/Downloads/baidu-downloads/WHEELTEC C07A核心板(Ti-MSPM0G3507)附送资料/`

### 时钟树

SysConfig 配置的完整 PLL 链路（`empty.syscfg`）：

```
32MHz SYSOSC → PLL(×5) → PLL_PDIV(÷2) → UDIV(÷2) → MCLK=40MHz
                                                    → CLK2X=80MHz → HSCLKMUX 选择 SYSPLL2X
```

- **HSCLK (CPU)**：80MHz
- **10ms 定时器 (TIMG0)**：prescaler=32, period=10ms → `80MHz/32 = 2.5MHz` 时基
- **PWM (TIMA1)**：80MHz / 8000 = 10kHz
- **SYSTICK**：period=0xFFFFFF（约 210ms），仅用作 `delay_ms/us` 参考时钟

## 开发环境

- **IDE**: TI Code Composer Studio (CCS) Theia 20.5.1
- **编译器**: TI ARM Clang Compiler 4.0.4.LTS (ticlang)
- **SDK**: MSPM0 SDK 2.10.00.04 (`/home/arch/ti/mspm0_sdk_2_10_00_04/`)
- **配置工具**: SysConfig 1.26.2 (`/home/arch/ti/sysconfig_1.26.2/`)
- **LSP/clangd**: `Debug/.clangd/compile_commands.json`（CCS 自动生成，可用 clangd 做代码补全/跳转）

> **注意：** SysConfig 文件头中记录的 `--product "mspm0_sdk@2.01.00.03"` 与实际 SDK 2.10.00.04 不一致，这是 SysConfig 的显示问题，不影响实际构建。

## 构建

在 CCS 中 Build。**不要 Clean**——Clean 后 CCS 会丢失 `device.opt` 且可能无法自动重新生成。

如需手动运行 SysConfig（比如 CCS 构建元数据损坏时）：
```bash
cd Debug
/home/arch/ti/sysconfig_1.26.2/sysconfig_cli.sh \
  -s "/home/arch/ti/mspm0_sdk_2_10_00_04/.metadata/product.json" \
  --script "../empty.syscfg" -o "." --compiler ticlang
```
运行后需检查 `device_linker.cmd` 的 FLASH 是否为 `0x00020000`（见下文）。

## 项目结构

```
├── empty.c                  # 主程序 (MPU6050+OLED+电机+编码器+灰度)
├── empty.syscfg             # SysConfig 配置源 (引脚/时钟/外设)
├── Hardware/                # 外设驱动层
│   ├── board.c/h            # 系统时钟、delay、printf→UART0 重定向 + 全局类型定义 (s32/s16/u8 等 STM32 风格别名)
│   ├── oled.c/h             # SSD1306 OLED (软件模拟 4 线 SPI)
│   ├── oledfont.h           # ASCII 字库 (12x6/16x8) + 中文点阵
│   ├── led.c/h              # 板载 LED (PB9)
│   ├── motor.c/h            # TB6612 PWM 驱动 + 增量式 PI 速度闭环
│   ├── encoder.c/h          # MG513 编码器脉冲计数 (GROUP1 中断, 输出 Get_Encoder_countA/B)
│   ├── key.c/h              # PA18 按键扫描 (单击启停/长按)
│   ├── grey.c/h             # 感为灰度传感器 (2 线串行协议)
│   ├── MPU6050.c/h          # WHEELTEC MPU6050 封装层
│   ├── inv_mpu.c/h          # InvenSense 底层 I2C 驱动 (~100KB)
│   ├── inv_mpu_dmp_motion_driver.c/h  # DMP 固件加载/FIFO 配置 (~60KB)
│   ├── dmpKey.h/dmpmap.h    # DMP 固件密钥/内存映射
│   ├── bsp_siic.h           # I2C 接口抽象 (IICInterface_t 函数指针)
│   └── bsp_iic.c            # 硬件 I2C 实现 + User_sIICDev 挂载
├── Debug/                   # 构建输出 (CCS 管理)
│   ├── ti_msp_dl_config.h/c # SysConfig 生成 (引脚宏/初始化)
│   ├── device_linker.cmd    # 链接脚本 (SysConfig 生成)
│   ├── device.opt           # 编译器宏 (-D__MSPM0G3507__ 等)
│   └── *.mk / makefile      # CCS 自动生成, 会被覆盖
└── targetConfigs/
    └── MSPM0G3507.ccxml     # XDS110 调试器配置
```

## 关键架构

### SysConfig 器件选择 (极其重要)

**SysConfig 1.26.2 不认识 `MSPM0G3507`**。`empty.syscfg` 中用 `--board "/ti/boards/LP_MSPM0G3507"` 替代 `--device`，这样 SysConfig 会从板级元数据解析出正确的 128KB Flash 和 `__MSPM0G3507__` 宏。

**永远不要改用 `--device "MSPM0G350X"`** — SysConfig 会降级为 MSPM0G3505（32KB Flash），导致 `.text` 段放不下。

`device.opt` 应该包含 `-D__MSPM0G3507__`（不是 G3505）。`device_linker.cmd` 的 FLASH 应为 `length = 0x00020000`。

### 完整引脚分配

| 引脚 | 外设 | 功能 |
|------|------|------|
| PA0 | I2C0 SDA | MPU6050 通信 (硬件 I2C, 400kHz) |
| PA1 | I2C0 SCL | MPU6050 通信 |
| PA7 | GPIO INT | MPU6050 DMP 数据就绪 (下降沿) |
| PA10 | UART0 TX | printf 调试输出 (115200) |
| PA11 | UART0 RX | 串口协议输入 |
| PA13 | GPIO OUT | TB6612 AIN2 (A 电机方向) |
| PA14 | GPIO OUT | TB6612 AIN1 |
| PA16 | GPIO OUT | TB6612 BIN1 (B 电机方向) |
| PA17 | GPIO OUT | TB6612 BIN2 |
| PA18 | GPIO IN | 按键 (单击启停电机) |
| PA19 | SWD | 调试 SWDIO |
| PA20 | SWD | 调试 SWCLK |
| PA24 | GPIO OUT | 灰度传感器 CLK |
| PA25 | GPIO IN | 编码器 A 相 (E1A, 中断) |
| PA26 | GPIO IN | 编码器 A 相 (E1B, 中断) |
| PA27 | GPIO IN | 灰度传感器 DAT |
| PA28 | GPIO OUT | OLED SCL (软件 SPI 时钟) |
| PA31 | GPIO OUT | OLED SDA (软件 SPI 数据) |
| PB2 | TIMA1 CCP0 | TB6612 PWMA (10kHz) |
| PB3 | TIMA1 CCP1 | TB6612 PWMB (10kHz) |
| PB9 | GPIO OUT | 板载 LED |
| PB14 | GPIO OUT | OLED RST |
| PB15 | GPIO OUT | OLED DC (命令/数据) |
| PB20 | GPIO IN | 编码器 B 相 (E2A, 中断) |
| PB24 | GPIO IN | 编码器 B 相 (E2B, 中断) |

### GROUP1 中断合并

MPU6050 INT (PA7)、编码器 A (PA25/PA26)、编码器 B (PB20/PB24) **全部共用 GROUP1**。SysConfig 会将同端口的中断合并为 `GPIO_MULTIPLE_GPIOA_INT_IRQN` 等宏。

**实际生成的中断名称（不要猜测，读 `ti_msp_dl_config.h` 确认）：**
- `GPIO_MULTIPLE_GPIOA_INT_IRQN` — 覆盖 PA7+PA25+PA26
- `ENCODERB_INT_IRQN` — 覆盖 PB20+PB24
- `TIMER_0_INST_INT_IRQN` — 10ms 定时器
- `UART_0_INST_INT_IRQN` — 串口中断

在 `GROUP1_IRQHandler` 中同时处理 MPU6050 DMP 读取和编码器计数。编码器的 `GROUP1_IRQHandler` 已重命名为 `Encoder_IRQ_Handler()`。

### 中断架构

```
GROUP1_IRQHandler          ← PA7(MPU6050) + PA25/26(EncA) + PB20/24(EncB)
  ├── Read_DMP()           ← 200Hz, 更新 mpu6050.pitch/roll/yaw
  └── Encoder_IRQ_Handler() ← 编码器脉冲计数

TIMER_0_INST_IRQHandler    ← TIMG0, 10ms
  ├── Key()                ← 按键扫描
  ├── encoderA_cnt/B_cnt   ← 读取编码器增量
  └── Velocity_A/B() → Set_PWM() ← PI 速度闭环

UART_0_INST_IRQHandler     ← 串口接收
  └── 协议解析 "#MPU6050:Y:0$" → yaw 归零
```

### 电机控制流

```
按键 PA18 单击 → Flag_Stop 取反
  Flag_Stop=0 (运行):
    10ms 定时器 → Velocity_A/B(target=+15, current=编码器增量)
    → 增量式 PI 控制器 → Set_PWM(A, B) → TB6612 驱动电机
  Flag_Stop=1 (停止):
    Set_PWM(0, 0)
```

`motor.c` 中 `Set_PWM(pwmA, pwmB)`：正值 = 正转，负值 = 反转。

**TB6612 方向控制逻辑（`Set_PWM`）：**
- 正转 (pwm>0)：AIN1 低、AIN2 高（或 BIN1 低、BIN2 高）
- 反转 (pwm<0)：AIN1 高、AIN2 低（或 BIN1 高、BIN2 低）
- PWM 频率 10kHz（TIMA1, 8000 计数），占空比 = `ABS(pwm)/8000`

**增量式 PI 速度闭环参数（`Velocity_A/B`）：**
- `Velcity_Kp = 1.0`, `Velcity_Ki = 0.4`（位于 `motor.c:2`）
- PI 输出限幅：**±7000**（不是 8000），防止占空比饱和
- 当前目标速度：`target = +15`（编码器脉冲/10ms, 正值=前进）

**编码器速度换算：**
- 10ms 定时器中读取 `encoderA_cnt` / `encoderB_cnt`（脉冲增量）
- MG513 编码器：电机每转 11 个脉冲（减速后），具体取决于减速比

### 串口协议

输出（100ms 周期）：`#MPU6050:Y:12.5$\r\n`

输入命令：`#MPU6050:Y:0$` → yaw 立即归零（`$` 为结束符）

### OLED 显示布局

```
┌──────────────────────────────┐
│ RUN  G:00110011              │  ← 状态 + 灰度8路 (bit0~bit7, 1=黑线)
│ P:  12.5                     │  ← Pitch (度, 1 位小数)
│ R:  -3.2                     │  ← Roll
│ Y:045.0 E1:+15 E2:+12        │  ← Yaw + 编码器A/B (脉冲/10ms)
└──────────────────────────────┘
```

- 第一行：电机启停状态 (STOP/RUN) + 灰度传感器 8 路二进制 (G: 后 8 个 0/1)
- 第二行：Pitch (-180~180°)
- 第三行：Roll (-90~90°)
- 第四行：Yaw (0~360°) + 编码器 A/B 脉冲增量 (正=正向, 负=反向)
- `angle_split(val, &int, &dec)` 处理负数取模 + 符号分离显示

## 添加新模块的标准流程

1. 从官方 WHEELTEC 资料中找到对应外设的 CCS 例程，解压查看
2. 将驱动 `.c/.h` 复制到 `Hardware/`
3. 在 `empty.syscfg` 中添加 GPIO/外设实例并绑定引脚
4. 手动运行 SysConfig CLI 生成新配置
5. 验证 `device_linker.cmd` FLASH = 128KB, `device.opt` = `-D__MSPM0G3507__`
6. 在 `Hardware/board.h` 中添加 `#include`
7. 更新 `Debug/Hardware/subdir_vars.mk`（C_SRCS + OBJS + C_DEPS 所有 6 个节）
8. 更新 `Debug/makefile`（ORDERED_OBJS + clean 目标）
9. 在 `empty.c` 中集成调用
10. 处理中断冲突（检查是否与 GROUP1 已有外设冲突）

## 已知注意事项

- **不要 Clean Build**：CCS Clean 后可能无法自动重新生成 SysConfig 文件。如需强制重编译，直接 Build
- **不要用 ST-Link 或 Uniflash BSL 模式烧录**：只用 CCS Debug 按钮（XDS110 SWD 模式）
- **不要手动编辑 Debug/ 下的生成文件**：`ti_msp_dl_config.*`、`device_linker.cmd`、`*.mk`、`makefile` 都会被 CCS/SysConfig 覆盖
- **IRQ 名称必须读 `ti_msp_dl_config.h` 确认**：每次 SysConfig 重新生成后名称可能变化（如 GPIO 合并导致名称变更）
- **OLED_ShowString 的参数类型**：字符串字面量会触发 `-Wpointer-sign` 警告（`char*` vs `uint8_t*`），无害
- **DMP_Init()/MPU6050_initialize() 失败 = 芯片软件复位**：传感器未连接或 I2C 通信失败时 MSPM0 会自行复位
- **printf 浮点格式 (%f) 可能不可用**：嵌入式 ticlang 默认不链接浮点 printf。用 `angle_split()` 拆分整数/小数后 `%d.%d` 输出
- **所有 Hardware/ 源文件的 include**：使用裸文件名（如 `"oled.h"`），编译器从源文件所在目录开始搜索，跨目录引用用 `"Hardware/xxx.h"`（仅 `empty.c` 需要）
