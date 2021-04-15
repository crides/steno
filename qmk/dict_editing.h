#ifndef _DICT_EDITING_H_
#define _DICT_EDITING_H_

typedef enum {
    ED_IDLE,
    ED_ACTIVE_ADD,
    ED_ACTIVE_ADD_TRANS,
    ED_ACTIVE_REMOVE,
    ED_ACTIVE_REMOVE_TRANS,
    ED_ACTIVE_EDIT_CONF_STROKES,
    ED_ACTIVE_EDIT_TRANS,
    ED_ACTIVE_EDIT_CONF_TRANS,
    ED_ERROR,
} editing_state_t;

extern editing_state_t editing_state;
extern uint8_t entry_buf[128];
extern uint8_t entry_buf_len;

void dicted_add_prompt_strokes(void);
void dicted_remove_prompt_strokes(void);
void dicted_edit_prompt_strokes(void);
bool handle_dict_editing(const uint32_t stroke);

#endif
