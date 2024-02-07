#ifndef __BUTTON_INC_
#define __BUTTON_INC_

#define SIMULATE_BUTTONS_THROUGH_CONSOLE    (1)

typedef enum button_id_ {
    BUTTON_ID_PLUS = 0x00,
    BUTTON_ID_MINUS,
    BUTTON_ID_BOTH,
    BUTTON_ID_START_PAUSE,
    BUTTON_ID_NONE = 0xFF,

} button_id_t;

button_id_t button_get_pressed(void);

#endif /* __BUTTON_INC_ */
