#ifndef _DICT_EDITING_H_
#define _DICT_EDITING_H_

typedef enum {
    ED_IDLE,
    ED_ENTER_STROKES,
    ED_CONF_EDIT,
    ED_ENTER_TRANS,
    ED_ERROR,
} editing_state_t;

extern editing_state_t editing_state;
extern uint8_t entry_buf[128];
extern uint8_t entry_buf_len;

void dicted_update(void);
bool handle_dict_editing(const uint32_t stroke);

#endif
