#ifndef _BOARD_H_
#define _BOARD_H_
#include "stdio.h"
#include "string.h"
#include "ti_msp_dl_config.h"
#include "oled.h"
#include "led.h"
#define ABS(a)      (a>0 ? a:(-a))
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */

#define SysTickMAX_COUNT 0xFFFFFF

#define SysTickFre 80000000

#define SysTick_MS(x)  ((SysTickFre/1000U)*(uint32_t)(x))
#define SysTick_US(x)  ((SysTickFre/1000000U)*(uint32_t)(x))

uint32_t Systick_getTick(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

void delay_1us(unsigned long __us);
void delay_1ms(unsigned long ms);
#endif  /* #ifndef _BOARD_H_ */