#ifndef _KEY_H
#define _KEY_H
#include "ti_msp_dl_config.h"
#include "board.h"
#define KEY  DL_GPIO_readPins(KEY_PORT,KEY_key_PIN)
uint8_t click_N_Double (uint8_t time);  //单击按键扫描和双击按键扫描
uint8_t click(void);               //单击按键扫描
uint8_t Long_Press(void);           //长按扫描
void Key(void);
#endif
