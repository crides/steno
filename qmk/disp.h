void disp_edit_conf_entry(const char *const s);
void disp_edit_done(void);
void disp_edit_prompt_strokes(void);
void disp_edit_prompt_trans(void);
void disp_edit_trans_back(const uint8_t len);
void disp_edit_trans_handle_char(const char c);
void disp_edit_trans_handle_str(const char *const s);

void disp_init(void);

void disp_add_done(void);
void disp_prompt_add_stroke(void);
void disp_prompt_trans(void);
void disp_remove_conf_entry(const char *const s);
void disp_remove_done(void);
void disp_remove_prompt_strokes(void);

void disp_show_abort(void);
void disp_show_entry_collision(void);
void disp_show_noentry(void);
void disp_show_nostorage(void);
void disp_unshow_error(void);

void disp_stroke_edit_add(const uint32_t stroke, const uint8_t num_strokes);
void disp_stroke_edit_remove(const uint32_t last, const uint8_t num_strokes);

void disp_tape_show_raw_stroke(const uint32_t stroke);
void disp_tape_show_star(void);
void disp_tape_show_stroke(const uint32_t stroke);
void disp_tape_show_stroke_sep(void);
void disp_tape_show_strokes_done(void);
void disp_tape_show_strokes_start(void);
void disp_tape_show_trans(const char *const trans);
