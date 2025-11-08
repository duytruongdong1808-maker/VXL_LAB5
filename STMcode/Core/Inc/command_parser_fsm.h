#ifndef __COMMAND_PARSER_FSM_H__
#define __COMMAND_PARSER_FSM_H__

#include "stdint.h"

#define MAX_BUFFER_SIZE 30

extern volatile uint8_t buffer_flag;
extern volatile uint8_t buffer[MAX_BUFFER_SIZE];
extern volatile uint8_t index_buffer;

void command_parser_fsm(void);
void buffer_push(uint8_t c);

typedef enum {
    CMD_NONE = 0,
    CMD_RST  = 1,
    CMD_OK   = 2
} cmd_t;

uint8_t command_take(void);

#endif
