#ifndef __PRO_TIMER_FSM_INC_
#define __PRO_TIMER_FSM_INC_

/** Signals of the application */
typedef enum pro_timer_signals_ {
    /* External activity signals */
    PRO_TIM_SIG_INC_TIME = 0x00,
    PRO_TIM_SIG_DEC_TIME,
    PRO_TIM_SIG_TIME_TICK,
    PRO_TIM_SIG_START_PAUSE,
    PRO_TIM_SIG_ABORT,

    /** Internal activity signals */
    PRO_TIM_SIG_ENTRY,
    PRO_TIM_SIG_EXIT,

} pro_timer_signals_t;

/** States of the application */
typedef enum pro_timer_states_ {
    PRO_TIM_ST_IDLE = 0x00,    
    PRO_TIM_ST_TIME_SET,
    PRO_TIM_ST_COUNTDOWN,
    PRO_TIM_ST_PAUSE,
    PRO_TIM_ST_STATS,

} pro_timer_states_t;

/** Generic (super) event structure */
typedef struct pro_timer_event_ {
    uint8_t signal;

} pro_timer_event_t;

/** For user generated events */
typedef struct pro_timer_user_event_ {
    pro_timer_event_t super;

} pro_timer_user_event_t;

/** For tick events */
typedef struct pro_timer_tick_event_ {
    pro_timer_event_t super;
    uint8_t ss;

} pro_timer_tick_event_t;

#endif /* __PRO_TIMER_FSM_INC_ */