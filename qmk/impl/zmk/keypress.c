#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

#include <dt-bindings/zmk/keys.h>
#include <sys/ring_buffer.h>
#include <kernel.h>

typedef struct __attribute__((packed)) {
    bool down;
    uint32_t key;
} key_event_t;

RING_BUF_DECLARE(macro_key_queue, CONFIG_ZMK_EMBEDDED_STENO_KEY_BUF_SIZE * sizeof(key_event_t));

static void macro_key_cb(struct k_timer *const timer) {
    key_event_t ev;
    if (ring_buf_get(&macro_key_queue, (uint8_t *) &ev, sizeof(key_event_t))) {
        ZMK_EVENT_RAISE(zmk_keycode_state_changed_from_encoded(ev.key, ev.down, k_uptime_get()));
    }
}

K_TIMER_DEFINE(macro_key_timer, macro_key_cb, NULL);

void register_code(const uint32_t keycode) {
    const key_event_t ev = { .down = true, .key = keycode };
    ring_buf_put(&macro_key_queue, (const uint8_t *) &ev, sizeof(key_event_t));
}

void unregister_code(const uint32_t keycode) {
    const key_event_t ev = { .down = false, .key = keycode };
    ring_buf_put(&macro_key_queue, (const uint8_t *) &ev, sizeof(key_event_t));
}
 
void tap_code(const uint32_t keycode) {
    register_code(keycode);
    unregister_code(keycode);
}

void steno_macro_init(void) {
    k_timer_start(&macro_key_timer, K_MSEC(CONFIG_ZMK_EMBEDDED_STENO_KEY_INTERVAL),
            K_MSEC(CONFIG_ZMK_EMBEDDED_STENO_KEY_INTERVAL));
}

// !!! FROM QMK !!!
const char *decode_utf8(const char *str, int32_t *const code_point) {
    const char *next;

    if (str[0] < 0x80) {  // U+0000-007F
        *code_point = str[0];
        next        = str + 1;
    } else if ((str[0] & 0xE0) == 0xC0) {  // U+0080-07FF
        *code_point = ((int32_t)(str[0] & 0x1F) << 6) | ((int32_t)(str[1] & 0x3F) << 0);
        next        = str + 2;
    } else if ((str[0] & 0xF0) == 0xE0) {  // U+0800-FFFF
        *code_point = ((int32_t)(str[0] & 0x0F) << 12) | ((int32_t)(str[1] & 0x3F) << 6) | ((int32_t)(str[2] & 0x3F) << 0);
        next        = str + 3;
    } else if ((str[0] & 0xF8) == 0xF0 && (str[0] <= 0xF4)) {  // U+10000-10FFFF
        *code_point = ((int32_t)(str[0] & 0x07) << 18) | ((int32_t)(str[1] & 0x3F) << 12) | ((int32_t)(str[2] & 0x3F) << 6) | ((int32_t)(str[3] & 0x3F) << 0);
        next        = str + 4;
    } else {
        *code_point = -1;
        next        = str + 1;
    }

    // part of a UTF-16 surrogate pair - invalid
    if (*code_point >= 0xD800 && *code_point <= 0xDFFF) {
        *code_point = -1;
    }

    return next;
}
static const uint32_t ascii_to_keycode_lut[128] = {
    // NUL   SOH      STX      ETX      EOT      ENQ      ACK      BEL
    0, 0, 0, 0, 0, 0, 0, 0,
    // BS    TAB      LF       VT       FF       CR       SO       SI
    BSPC, TAB,  RET,  0, 0, 0, 0, 0,
    // DLE   DC1      DC2      DC3      DC4      NAK      SYN      ETB
    0, 0, 0, 0, 0, 0, 0, 0,
    // CAN   EM       SUB      ESC      FS       GS       RS       US
    0, 0, 0, ESC,  0, 0, 0, 0,

    //       !        "        #        $        %        &        '
    SPACE,  EXCL,    DQT, HASH,    DOLLAR,    PERCENT,    AMPS,    SQT,
    // (     )        *        +        ,        -        .        /
    LPAR,    RPAR,    STAR,    PLUS,  COMMA, MINUS, DOT,  SLASH,
    // 0     1        2        3        4        5        6        7
    N0,    N1,    N2,    N3,    N4,    N5,    N6,    N7,
    // 8     9        :        ;        <        =        >        ?
    N8,    N9,    COLON, SEMI, LT, EQL,  GT,  QMARK,
    // @     A        B        C        D        E        F        G
    AT,    LS(A),    LS(B),    LS(C),    LS(D),    LS(E),    LS(F),    LS(G),
    // H     I        J        K        L        M        N        O
    LS(H),    LS(I),    LS(J),    LS(K),    LS(L),    LS(M),    LS(N),    LS(O),
    // P     Q        R        S        T        U        V        W
    LS(P),    LS(Q),    LS(R),    LS(S),    LS(T),    LS(U),    LS(V),    LS(W),
    // X     Y        Z        [        \        ]        ^        _
    LS(X),    LS(Y),    LS(Z),    LBKT, BSLH, RBKT, CARET,    UNDER,
    // `     a        b        c        d        e        f        g
    GRAVE,  A,    B,    C,    D,    E,    F,    G,
    // h     i        j        k        l        m        n        o
    H,    I,    J,    K,    L,    M,    N,    O,
    // p     q        r        s        t        u        v        w
    P,    Q,    R,    S,    T,    U,    V,    W,
    // x     y        z        {        |        }        ~        DEL
    X,    Y,    Z,    LBRC, PIPE, RBRC, TILDE,  DEL
};

void send_char(const char ascii_code) {
    tap_code(ascii_to_keycode_lut[(uint8_t) ascii_code]);
}
