#ifndef _DICT_EDITING_H_
#define _DICT_EDITING_H_

typedef enum {
    ED_IDLE,
    ED_ACTIVE_ADD,
    ED_ACTIVE_ADD_TRANS,
    ED_ACTIVE_REMOVE,
    ED_ACTIVE_EDIT,
    ED_ACTIVE_REMOVE_TRANS,
    ED_ACTIVE_EDIT_TRANS,
    ED_ACTIVE_EDIT_TRANS_2,
    ED_ACTIVE_EDIT_TRANS_3,
    ED_ERROR,
} editing_state_t;

extern editing_state_t editing_state;
extern uint8_t page_buffer[FLASH_PP_SIZE];
extern uint8_t entry_buf_len;

void prompt_user(void);
void set_Stroke(uint32_t stroke);
void prompt_user_translation(void);
void set_Trans(char trans[]);
void add_finished(void);
void remove_stroke(void);
void display_stroke_to_remove(void);
void prompt_user_remove(void);
void prompt_user_edit(void);
void display_stroke_to_edit(void);
void prompt_user_edit_translation(void);
void edit_finished(void);


#endif
