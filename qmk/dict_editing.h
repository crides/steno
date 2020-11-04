#ifndef _DICT_EDITING_H_
#define _DICT_EDITING_H_

typedef enum {
    ED_IDLE,
    ED_ACTIVE_ADD,
    ED_ACTIVE_REMOVE,
    ED_ACTIVE_CHANGE,
} editing_state_t;

extern editing_state_t editing_state;

void prompt_user(void);
void set_Stroke(char stroke[]);

#endif
