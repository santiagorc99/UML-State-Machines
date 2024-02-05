#include "protimer.h"

#include "lcd.h"
#include "main.h"

/** Helper functions.... Just to avoid some compilation errors */
void do_beep(void)
{
    HAL_GPIO_WritePin(USER_LED1_GPIO_Port, USER_LED1_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(USER_LED1_GPIO_Port, USER_LED1_Pin, GPIO_PIN_RESET);
}

static protimer_event_status_t pro_timer_idle_state_handler(protimer_t *const, protimer_event_t *const);
static protimer_event_status_t pro_timer_time_set_state_handler(protimer_t *const, protimer_event_t *const);
static protimer_event_status_t pro_timer_stats_state_handler(protimer_t *const, protimer_event_t *const);
static protimer_event_status_t pro_timer_pause_state_handler(protimer_t *const, protimer_event_t *const);
static protimer_event_status_t pro_timer_countdown_state_handler(protimer_t *const, protimer_event_t *const);

/** Handle an event when the status is IDLE */
static protimer_event_status_t pro_timer_idle_state_handler(protimer_t *const mobj, protimer_event_t *const e)
{
    switch (e->signal) {
        case PROTIM_SIG_ENTRY: {
            mobj->current_time = 0;
            mobj->elapsed_time = 0;
            display_time(0);
            display_message("Set Time");
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_INC_TIME: {
            mobj->current_time += 60;
            mobj->active_state = &pro_timer_time_set_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            mobj->active_state = &pro_timer_stats_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        /** We have to check the guard of the internal events, we cast to each specific event to
         *      get the parameters of the event.
        */
        case PROTIM_SIG_TIME_TICK: {
            static uint8_t tick_count = 0;
            static uint8_t total_beeps = 0;
            if (++tick_count == 5) {
                tick_count = 0;
                if (++total_beeps <= 20) {
                    do_beep();
                }
                return PROTIMER_EVENT_HANDLED;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        default:
            return PROTIMER_EVENT_IGNORED;
    }
}

/** Handle an event when the status is IDLE */
static protimer_event_status_t pro_timer_time_set_state_handler(protimer_t *const mobj, protimer_event_t *const e)
{
    switch (e->signal) {
        case PROTIM_SIG_ENTRY: {
            display_message("Set Time");
            display_time(mobj->current_time);
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_INC_TIME: {
            mobj->current_time += 60;
            display_time(mobj->current_time);
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_DEC_TIME: {
            if (mobj->current_time >= 60) {
                mobj->current_time -= 60;
                display_time(mobj->current_time);
                display_show();
                return PROTIMER_EVENT_HANDLED;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        case PROTIM_SIG_ABORT: {
            mobj->active_state = &pro_timer_idle_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            if (mobj->current_time > 0) {
                mobj->active_state = &pro_timer_countdown_state_handler;
                return PROTIMER_EVENT_TRANSITION;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        default:
            return PROTIMER_EVENT_IGNORED;
    }
}

static protimer_event_status_t pro_timer_stats_state_handler(protimer_t *const mobj, protimer_event_t *const e)
{
    switch (e->signal) {
        case PROTIM_SIG_ENTRY: {
            display_time(mobj->worked_time);
            display_message("Worked Time");
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_TIME_TICK: {
            static uint8_t tick_count = 0;
            if (++tick_count == 30) {
                tick_count = 0;
                mobj->active_state = &pro_timer_idle_state_handler;
                return PROTIMER_EVENT_TRANSITION;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        default:
            return PROTIMER_EVENT_IGNORED;
    }
}

static protimer_event_status_t pro_timer_pause_state_handler(protimer_t *const mobj, protimer_event_t *const e)
{
    switch (e->signal) {
        case PROTIM_SIG_ENTRY: {
            display_message("Paused");
            display_time(mobj->current_time);
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_INC_TIME: {
            mobj->current_time += 60;
            display_time(mobj->current_time);
            display_show();
            mobj->active_state = &pro_timer_time_set_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_DEC_TIME: {
            if (mobj->current_time >= 60) {
                mobj->current_time -= 60;
                display_time(mobj->current_time);
                display_show();
                mobj->active_state = &pro_timer_time_set_state_handler;
                return PROTIMER_EVENT_HANDLED;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        case PROTIM_SIG_ABORT: {
            mobj->active_state = &pro_timer_idle_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            mobj->active_state = &pro_timer_countdown_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        default:
            return PROTIMER_EVENT_IGNORED;
    }
}

static protimer_event_status_t pro_timer_countdown_state_handler(protimer_t *const mobj, protimer_event_t *const e)
{
    switch (e->signal) {
        case PROTIM_SIG_EXIT: {
            mobj->worked_time += mobj->elapsed_time;
            mobj->elapsed_time = 0;
            display_clear();
            display_show();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_ABORT: {
            mobj->active_state = &pro_timer_idle_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            mobj->active_state = &pro_timer_pause_state_handler;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_TIME_TICK: {
            static uint8_t tick_count = 0;
            if (++tick_count == 10) {
                tick_count = 0;
                mobj->current_time--;
                mobj->elapsed_time++;
                display_time(mobj->current_time);
                display_show();

                if (mobj->current_time == 0) {
                    mobj->active_state = &pro_timer_idle_state_handler;
                    return PROTIMER_EVENT_TRANSITION;
                }
                return PROTIMER_EVENT_HANDLED;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        default:
            return PROTIMER_EVENT_IGNORED;
    }
}


/** We change the order of transitions like this:
 * 1. Transition action.
 * 2. Exit action of the source state.
 * 3. Entry action of the target state.
*/
void protimer_event_dispatcher(protimer_t *const mobj, protimer_event_t *const e)
{
    protimer_states_t source_state = mobj->active_state;
    protimer_event_status_t status = mobj->active_state(mobj, e);

    if (status == PROTIMER_EVENT_TRANSITION) {
    	/** Save the target */
    	protimer_states_t target_state = mobj->active_state;

        /** Create an exit event (ee) for handling the exit and entry events */
        protimer_event_t ee;

        ee.signal = PROTIM_SIG_EXIT;
        source_state(mobj, &ee);

        ee.signal = PROTIM_SIG_ENTRY;
        target_state(mobj, &ee);
    }
}

/** Let's define the initial transition (FSM Init) */
void protimer_init(protimer_t *const mobj)
{
    /** Execute the initial action */
    mobj->worked_time = 0;
    /** The initial state */
    mobj->active_state = &pro_timer_idle_state_handler;

    /** Execute entry action */
    protimer_event_t ee = {.signal = PROTIM_SIG_ENTRY};

    /** Jump to idle state to execute entry action*/
    mobj->active_state(mobj, &ee);
}
