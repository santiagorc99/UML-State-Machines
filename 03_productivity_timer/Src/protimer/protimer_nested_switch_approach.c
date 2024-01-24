#include "protimer.h"

#include "lcd.h"

/** Helper functions.... Just to avoid some compilation errors */
void do_beep(void) 
{

}

/** Handle an event when the status is IDLE */
static protimer_event_status_t pro_timer_idle_state_handler(protimer_t *const mobj, protimer_event_t *const e)
{
    switch (e->signal) {
        case PROTIM_SIG_ENTRY: {
            mobj->current_time = 0;
            mobj->elapsed_time = 0;
            display_time(0);
            display_message("Set Time");
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_INC_TIME: {
            mobj->current_time += 60;
            mobj->active_state = PROTIM_ST_TIME_SET;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            mobj->active_state = PROTIM_ST_STATS;
            return PROTIMER_EVENT_TRANSITION;
        }

        /** We have to check the guard of the internal events, we cast to each specific event to 
         *      get the parameters of the event.
        */
        case PROTIM_SIG_TIME_TICK: {
            if (((protimer_tick_event_t *)e)->ss == 5) {
                do_beep();
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
            display_time(mobj->current_time);
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_INC_TIME: {
            mobj->current_time += 60;
            display_time(mobj->current_time);
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_DEC_TIME: {
            if (mobj->current_time >= 60) {
                mobj->current_time -= 60;
                display_time(mobj->current_time);
                return PROTIMER_EVENT_HANDLED;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        case PROTIM_SIG_ABORT: {
            mobj->active_state = PROTIM_ST_IDLE;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            if (mobj->current_time > 0) {
                mobj->active_state = PROTIM_ST_COUNTDOWN;
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
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_TIME_TICK: {
            if (((protimer_tick_event_t *)e)->ss == 10) {
                mobj->active_state = PROTIM_ST_IDLE;
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
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_EXIT: {
            display_clear();
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_INC_TIME: {
            mobj->current_time += 60;
            display_time(mobj->current_time);
            mobj->active_state = PROTIM_ST_TIME_SET;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_DEC_TIME: {
            if (mobj->current_time >= 60) {
                mobj->current_time -= 60;
                display_time(mobj->current_time);
                mobj->active_state = PROTIM_ST_TIME_SET;
                return PROTIMER_EVENT_HANDLED;
            }
            return PROTIMER_EVENT_IGNORED;
        }

        case PROTIM_SIG_ABORT: {
            mobj->active_state = PROTIM_ST_IDLE;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            mobj->active_state = PROTIM_ST_COUNTDOWN;
            return PROTIMER_EVENT_IGNORED;
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
            return PROTIMER_EVENT_HANDLED;
        }

        case PROTIM_SIG_ABORT: {
            mobj->active_state = PROTIM_ST_IDLE;
            return PROTIMER_EVENT_TRANSITION;
        }

        case PROTIM_SIG_START_PAUSE: {
            mobj->active_state = PROTIM_ST_PAUSE;
            return PROTIMER_EVENT_IGNORED;
        }

        case PROTIM_SIG_TIME_TICK: {
            if (((protimer_tick_event_t *)e)->ss == 10) {
                mobj->current_time--;
                mobj->elapsed_time++;
                display_time(mobj->current_time);

                if (mobj->current_time == 0) {
                    mobj->active_state = PROTIM_ST_IDLE;
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

protimer_event_status_t protimer_state_machine(protimer_t *const mobj, protimer_event_t *const e)
{
    switch (mobj->active_state) {
        case PROTIM_ST_IDLE: {
            return pro_timer_idle_state_handler(mobj, e);
        }

        case PROTIM_ST_TIME_SET: {
        	return pro_timer_time_set_state_handler(mobj, e);
            break;
        }

        case PROTIM_ST_COUNTDOWN: {
        	return pro_timer_countdown_state_handler(mobj, e);
            break;
        }

        case PROTIM_ST_PAUSE: {
        	return pro_timer_pause_state_handler(mobj, e);
            break;
        }

        case PROTIM_ST_STATS: {
        	return pro_timer_stats_state_handler(mobj, e);
            break;
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
    protimer_event_status_t status = protimer_state_machine(mobj, e);
    protimer_states_t target_state = mobj->active_state;

    if (status == PROTIMER_EVENT_TRANSITION) {
        /** Create an exit event (ee) for handling the exit and entry events */
        protimer_event_t ee;

        ee.signal = PROTIM_SIG_EXIT;
        mobj->active_state = source_state;
        protimer_state_machine(mobj, &ee);

        ee.signal = PROTIM_SIG_EXIT;
        mobj->active_state = target_state;
        protimer_state_machine(mobj, &ee);
    }
}

/** Let's define the initial transition (FSM Init) */
void protimer_init(protimer_t *const mobj) 
{
    /** Execute the initial action */
    mobj->worked_time = 0;

    /** The initial state */
    mobj->active_state = PROTIM_ST_IDLE;
}
