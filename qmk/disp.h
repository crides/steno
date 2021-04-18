void disp_init(void);

void disp_tape_show_raw_stroke(const uint32_t stroke);
void disp_tape_show_star(void);
void disp_tape_show_strokes(const uint8_t *strokes, const uint8_t len);
void disp_tape_show_trans(const char *const trans);

#ifndef STENO_READONLY
void disp_trans_edit_back(const uint8_t len);
void disp_trans_edit_handle_char(const char c);
void disp_trans_edit_handle_str(const char *const s);
void disp_conf_entry(const char *const s);
void disp_prompt_strokes(void);
void disp_prompt_trans(void);
void disp_dicted_done(void);

void disp_show_abort(void);
void disp_show_entry_collision(void);
void disp_show_noentry(void);
void disp_show_nostorage(void);
void disp_show_removed(void);
void disp_unshow_error(void);

void disp_stroke_edit_add(const uint32_t stroke, const uint8_t num_strokes);
void disp_stroke_edit_remove(const uint32_t last, const uint8_t num_strokes);
#endif
