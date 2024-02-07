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

static protimer_event_status_t pro_timer_idle_entry(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->current_time = 0;
    mobj->elapsed_time = 0;
    display_time(0);
    display_message("Set Time");
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_idle_exit(protimer_t *const mobj, protimer_event_t *const e)
{
    display_clear();
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_idle_inc_time(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->current_time += 60;
    mobj->active_state = PROTIM_ST_TIME_SET;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_idle_start_pause(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->active_state = PROTIM_ST_STATS;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_idle_time_tick(protimer_t *const mobj, protimer_event_t *const e)
{
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

static protimer_event_status_t pro_timer_time_set_entry(protimer_t *const mobj, protimer_event_t *const e)
{
    display_message("Set Time");
    display_time(mobj->current_time);
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_time_set_exit(protimer_t *const mobj, protimer_event_t *const e)
{
    display_clear();
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_time_set_inc_time(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->current_time += 60;
    display_time(mobj->current_time);
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_time_set_dec_time(protimer_t *const mobj, protimer_event_t *const e)
{
    if (mobj->current_time >= 60) {
        mobj->current_time -= 60;
        display_time(mobj->current_time);
        display_show();
        return PROTIMER_EVENT_HANDLED;
    }
    return PROTIMER_EVENT_IGNORED;
}

static protimer_event_status_t pro_timer_time_set_abort(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->active_state = PROTIM_ST_IDLE;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_time_set_start_pause(protimer_t *const mobj, protimer_event_t *const e)
{
    if (mobj->current_time > 0) {
        mobj->active_state = PROTIM_ST_COUNTDOWN;
        return PROTIMER_EVENT_TRANSITION;
    }
    return PROTIMER_EVENT_IGNORED;
}

static protimer_event_status_t pro_timer_stats_entry(protimer_t *const mobj, protimer_event_t *const e)
{
    display_time(mobj->worked_time);
    display_message("Worked Time");
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_stats_exit(protimer_t *const mobj, protimer_event_t *const e)
{
    display_clear();
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_stats_time_tick(protimer_t *const mobj, protimer_event_t *const e)
{
    static uint8_t tick_count = 0;
    if (++tick_count == 30) {
        tick_count = 0;
        mobj->active_state = PROTIM_ST_IDLE;
        return PROTIMER_EVENT_TRANSITION;
    }
    return PROTIMER_EVENT_IGNORED;
}

static protimer_event_status_t pro_timer_pause_entry(protimer_t *const mobj, protimer_event_t *const e)
{
    display_message("Paused");
    display_time(mobj->current_time);
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_pause_exit(protimer_t *const mobj, protimer_event_t *const e)
{
    display_clear();
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_pause_inc_time(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->current_time += 60;
    display_time(mobj->current_time);
    display_show();
    mobj->active_state = PROTIM_ST_TIME_SET;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_pause_dec_time(protimer_t *const mobj, protimer_event_t *const e)
{
    if (mobj->current_time >= 60) {
        mobj->current_time -= 60;
        display_time(mobj->current_time);
        display_show();
        mobj->active_state = PROTIM_ST_TIME_SET;
        return PROTIMER_EVENT_HANDLED;
    }
    return PROTIMER_EVENT_IGNORED;
}

static protimer_event_status_t pro_timer_pause_abort(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->active_state = PROTIM_ST_IDLE;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_pause_start_pause(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->active_state = PROTIM_ST_COUNTDOWN;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_countdown_exit(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->worked_time += mobj->elapsed_time;
    mobj->elapsed_time = 0;
    display_clear();
    display_show();
    return PROTIMER_EVENT_HANDLED;
}

static protimer_event_status_t pro_timer_countdown_abort(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->active_state = PROTIM_ST_IDLE;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_countdown_start_pause(protimer_t *const mobj, protimer_event_t *const e)
{
    mobj->active_state = PROTIM_ST_PAUSE;
    return PROTIMER_EVENT_TRANSITION;
}

static protimer_event_status_t pro_timer_countdown_time_tick(protimer_t *const mobj, protimer_event_t *const e)
{
    static uint8_t tick_count = 0;
    if (++tick_count == 10) {
        tick_count = 0;
        mobj->current_time--;
        mobj->elapsed_time++;
        display_time(mobj->current_time);
        display_show();

        if (mobj->current_time == 0) {
            mobj->active_state = PROTIM_ST_IDLE;
            return PROTIMER_EVENT_TRANSITION;
        }
        return PROTIMER_EVENT_HANDLED;
    }
    return PROTIMER_EVENT_IGNORED;
}

static event_handler_t protimer_state_table[PROTIM_ST_TOTAL][PROTIM_SIG_TOTAL] = {
	[PROTIM_ST_IDLE]      = {&pro_timer_idle_inc_time,     NULL,                         &pro_timer_idle_time_tick,      &pro_timer_idle_start_pause,      NULL,                       &pro_timer_idle_entry,     &pro_timer_idle_exit      },
	[PROTIM_ST_TIME_SET]  = {&pro_timer_time_set_inc_time, &pro_timer_time_set_dec_time, NULL,                           &pro_timer_time_set_start_pause,  &pro_timer_time_set_abort,  &pro_timer_time_set_entry, &pro_timer_time_set_exit  },
	[PROTIM_ST_COUNTDOWN] = {NULL,                         NULL,                         &pro_timer_countdown_time_tick, &pro_timer_countdown_start_pause, &pro_timer_countdown_abort, NULL,                      &pro_timer_countdown_exit },
	[PROTIM_ST_PAUSE]     = {&pro_timer_pause_inc_time,    &pro_timer_pause_dec_time,    NULL,                           &pro_timer_pause_start_pause,     &pro_timer_pause_abort,     &pro_timer_pause_entry,    &pro_timer_pause_exit     },
	[PROTIM_ST_STATS]     = {NULL,                         NULL,                         &pro_timer_stats_time_tick,     NULL,                             NULL,                       &pro_timer_stats_entry,    &pro_timer_stats_exit     },
};

static void protimer_state_table_init(protimer_t *const mobj)
{
	mobj->state_table = (event_handler_t *) protimer_state_table;
}

/** Let's define the initial transition (FSM Init) */
void protimer_init(protimer_t *const mobj)
{
	/** Init the state table */
    protimer_state_table_init(mobj);

    /** Execute the initial action */
    mobj->worked_time = 0;
    /** The initial state */
    mobj->active_state = PROTIM_ST_IDLE;

    /** Execute entry action */
    protimer_event_t ee = {.signal = PROTIM_SIG_ENTRY};

    /** Jump to idle state to execute entry action*/
    event_handler_t e_handler = mobj->state_table[(mobj->active_state*PROTIM_SIG_TOTAL) + ee.signal];
    if (e_handler) {e_handler(mobj, &ee);}
}

/** We change the order of transitions like this:
 * 1. Transition action.
 * 2. Exit action of the source state.
 * 3. Entry action of the target state.
*/
void protimer_event_dispatcher(protimer_t *const mobj, protimer_event_t *const e)
{
    protimer_states_t source_state = mobj->active_state;

    event_handler_t e_handler = mobj->state_table[(mobj->active_state * PROTIM_SIG_TOTAL) + e->signal];

    if (e_handler == NULL) {return;}

    protimer_event_status_t status = e_handler(mobj, e);

    if (status == PROTIMER_EVENT_TRANSITION) {
    	/** Save the target */
    	protimer_states_t target_state = mobj->active_state;

        /** Create an exit event (ee) for handling the exit and entry events */
        protimer_event_t ee;
        ee.signal = PROTIM_SIG_EXIT;
        e_handler = mobj->state_table[(source_state * PROTIM_SIG_TOTAL) + ee.signal];
        if (e_handler != NULL) {e_handler(mobj, &ee);}

        ee.signal = PROTIM_SIG_ENTRY;
        e_handler = mobj->state_table[(target_state * PROTIM_SIG_TOTAL) + ee.signal];
        if (e_handler != NULL) {e_handler(mobj, &ee);}
    }
}
