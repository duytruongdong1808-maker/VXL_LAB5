#ifndef __UART_COMMUNICATION_FSM_H__
#define __UART_COMMUNICATION_FSM_H__

#include "stm32f1xx_hal.h"
#include <stdint.h>

void uart_communiation_fsm(void);

extern ADC_HandleTypeDef   hadc1;
extern UART_HandleTypeDef  huart2;

#endif
