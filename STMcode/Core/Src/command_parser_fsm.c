#include "command_parser_fsm.h"
#include <string.h>
#include <ctype.h>

volatile uint8_t buffer_flag = 0;
volatile uint8_t buffer[MAX_BUFFER_SIZE];
volatile uint8_t index_buffer = 0;

static cmd_t cmd_q[8];
static uint8_t q_head = 0, q_tail = 0;

static inline void cmd_enqueue(cmd_t c){
    uint8_t next = (q_tail + 1) & 7;
    if (next != q_head) { cmd_q[q_tail] = c; q_tail = next; }
}

uint8_t command_take(void){
    if (q_head == q_tail) return CMD_NONE;
    uint8_t c = cmd_q[q_head];
    q_head = (q_head + 1) & 7;
    return c;
}

static volatile uint8_t read_index = 0;
static char cmd_buf[32];
static uint8_t cmd_idx = 0;

void buffer_push(uint8_t c){
    uint8_t next = index_buffer + 1;
    if (next == MAX_BUFFER_SIZE) next = 0;

    if (next == read_index){
        read_index++;
        if (read_index == MAX_BUFFER_SIZE) read_index = 0;
    }

    buffer[index_buffer] = c;
    index_buffer = next;
    buffer_flag = 1;
}

static inline void trim(char *s){
    size_t i = 0, j = strlen(s);
    while (isspace((unsigned char)s[i])) i++;
    while (j > i && isspace((unsigned char)s[j-1])) j--;
    s[j] = 0;
    memmove(s, s+i, j-i+1);
}

void command_parser_fsm(void){
    while (read_index != index_buffer){
        uint8_t c = buffer[read_index++];
        if (read_index == MAX_BUFFER_SIZE) read_index = 0;

        if (c=='\r' || c=='\n'){
            if (cmd_idx){
                cmd_buf[cmd_idx] = 0;
                trim(cmd_buf);
                if      (!strcmp(cmd_buf,"!RST#")) cmd_enqueue(CMD_RST);
                else if (!strcmp(cmd_buf,"!OK#"))  cmd_enqueue(CMD_OK);
            }
            cmd_idx = 0;
            continue;
        }

        if (c==0x08 || c==0x7F){
            if (cmd_idx) cmd_idx--;
            continue;
        }

        if (cmd_idx < sizeof(cmd_buf)-1) cmd_buf[cmd_idx++] = c;
        else cmd_idx = 0;
    }

    buffer_flag = (read_index != index_buffer);
}
