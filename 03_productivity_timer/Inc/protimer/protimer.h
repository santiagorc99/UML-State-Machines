#ifndef __PROTIMER_INC_
#define __PROTIMER_INC_

#include <stdint.h>

/** Signals of the application */
typedef enum protimer_signals_ {
    /* External activity signals */
    PROTIM_SIG_INC_TIME = 0x00,
    PROTIM_SIG_DEC_TIME,
    PROTIM_SIG_TIME_TICK,
    PROTIM_SIG_START_PAUSE,
    PROTIM_SIG_ABORT,

    /** Internal activity signals */
    PROTIM_SIG_ENTRY,
    PROTIM_SIG_EXIT,

} protimer_signals_t;

/** States of the application */
typedef enum protimer_states_ {
    PROTIM_ST_IDLE = 0x00,    
    PROTIM_ST_TIME_SET,
    PROTIM_ST_COUNTDOWN,
    PROTIM_ST_PAUSE,
    PROTIM_ST_STATS,

} protimer_states_t;

/** Generic (super) event structure */
typedef struct protimer_event_ {
    uint8_t signal;

} protimer_event_t;

/** For user generated events */
typedef struct protimer_user_event_ {
    protimer_event_t super;

} protimer_user_event_t;

/** For tick events */
typedef struct protimer_tick_event_ {
    protimer_event_t super;
    uint8_t ss;

} protimer_tick_event_t;

/** Define the possible results of processing an event */
typedef enum protimer_event_status_ {
    PROTIMER_EVENT_HANDLED = 0x00,
    PROTIMER_EVENT_IGNORED,
    PROTIMER_EVENT_TRANSITION,

} protimer_event_status_t;

/** Main application structure */
typedef struct protimer_ {
    uint32_t current_time;
    uint32_t elapsed_time;
    uint32_t worked_time;

    protimer_states_t active_state;

} protimer_t;

void protimer_init(protimer_t *const mobj);
void protimer_event_dispatcher(protimer_t *const mobj, protimer_event_t *const e);

#endif /* __PROTIMER_INC_ */
