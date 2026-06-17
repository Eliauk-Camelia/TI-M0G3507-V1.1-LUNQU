#ifndef _ENCODER_H
#define _ENCODER_H
#include "ti_msp_dl_config.h"
#include "board.h"

extern int Get_Encoder_countA,Get_Encoder_countB;
void Encoder_IRQ_Handler(void);

#endif
