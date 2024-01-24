#ifndef __PRO_TIMER_INC_
#define __PRO_TIMER_INC_

#include <stdint.h>
#include "pro_timer_fsm.h"

/** Main application structure */
typedef struct pro_timer_ {
    uint32_t current_time;
    uint32_t elapsed_time;
    uint32_t worked_time;

    pro_timer_states_t active_state;

} pro_timer_t;

#endif /* __PRO_TIMER_INC_ */
