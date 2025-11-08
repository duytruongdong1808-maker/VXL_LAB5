#include "uart_communication_fsm.h"
#include "command_parser_fsm.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

typedef enum { UART_IDLE = 0, UART_WAIT_OK } uart_state_t;

static uart_state_t state = UART_IDLE;
static uint32_t adc_snapshot = 0;
static uint32_t t_start = 0;
static char txbuf[32];

#define ACK_TIMEOUT_MS 3000

static inline uint32_t read_adc_once(void){
    HAL_ADC_PollForConversion(&hadc1, 10);
    return HAL_ADC_GetValue(&hadc1);
}

static inline void send_adc(void){
    int n = snprintf(txbuf, sizeof(txbuf), "!ADC=%lu#\r\n", (unsigned long)adc_snapshot);
    HAL_UART_Transmit(&huart2, (uint8_t*)txbuf, n, 100);
}

void uart_communiation_fsm(void){
    for (;;){
        uint8_t cmd = command_take();
        if (cmd == CMD_NONE) break;

        if (state == UART_IDLE){
            if (cmd == CMD_RST){
                HAL_UART_Transmit(&huart2,(uint8_t*)"State: RST\r\n",12,100);
                adc_snapshot = read_adc_once();
                send_adc();
                t_start = HAL_GetTick();
                state = UART_WAIT_OK;
            }
        }
        else if (state == UART_WAIT_OK){
            if (cmd == CMD_OK){
                HAL_UART_Transmit(&huart2,(uint8_t*)"State: OK\r\n",11,100);
                state = UART_IDLE;
            }
            else if (cmd == CMD_RST){
                HAL_UART_Transmit(&huart2,(uint8_t*)"State: RST again\r\n",18,100);
                adc_snapshot = read_adc_once();
                send_adc();
                t_start = HAL_GetTick();
            }
        }
    }

    if (state == UART_WAIT_OK){
        if (HAL_GetTick() - t_start >= ACK_TIMEOUT_MS){
            send_adc();
            t_start = HAL_GetTick();
        }
    }
}
