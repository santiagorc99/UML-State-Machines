#include "mealy.h"
#include <stdint.h>
#include "user_pwm.h"
#include "user_uart.h"

typedef enum light_controller_intensity_ {
	LIGHT_CONTROLLER_INTENSITY_OFF = 0,
	LIGHT_CONTROLLER_INTENSITY_LOW = 20,
	LIGHT_CONTROLLER_INTENSITY_MEDIUM = 50,
	LIGHT_CONTROLLER_INTENSITY_HIGH = 100,

} light_controller_intensity_t;

typedef enum light_controller_events_ {
	LIGHT_CONTROLLER_EV_NONE = 0x00,
	LIGHT_CONTROLLER_EV_OFF,
	LIGHT_CONTROLLER_EV_ON,

} light_controller_events_t;

typedef enum light_controller_states_ {
	LIGHT_CONTROLLER_ST_OFF = 0x00,
	LIGHT_CONTROLLER_ST_DIM,
	LIGHT_CONTROLLER_ST_MEDIUM,
	LIGHT_CONTROLLER_ST_BRIGHT,

} light_controller_states_t;

static light_controller_states_t state = LIGHT_CONTROLLER_ST_OFF;

static void light_controller_set_intensity(light_controller_intensity_t intensity)
{
	user_pwm1_set_duty_cylcle(intensity);
}

void light_controller_state_machine_run(light_controller_events_t event)
{
	switch (state) {
		case LIGHT_CONTROLLER_ST_OFF: {
			switch (event) {
				case LIGHT_CONTROLLER_EV_OFF:
					/** Ignore this event */
					break;

				case LIGHT_CONTROLLER_EV_ON:
					light_controller_set_intensity(LIGHT_CONTROLLER_INTENSITY_LOW);
					state = LIGHT_CONTROLLER_ST_DIM;
					break;

				default:
					break;
			}
			break;
		}

		case LIGHT_CONTROLLER_ST_DIM:{
			switch (event) {
				case LIGHT_CONTROLLER_EV_OFF:
					light_controller_set_intensity(LIGHT_CONTROLLER_INTENSITY_OFF);
					state = LIGHT_CONTROLLER_ST_OFF;
					break;

				case LIGHT_CONTROLLER_EV_ON:
					light_controller_set_intensity(LIGHT_CONTROLLER_INTENSITY_MEDIUM);
					state = LIGHT_CONTROLLER_ST_MEDIUM;
					break;

				default:
					break;
			}
			break;
		}

		case LIGHT_CONTROLLER_ST_MEDIUM:{
			switch (event) {
				case LIGHT_CONTROLLER_EV_OFF:
					light_controller_set_intensity(LIGHT_CONTROLLER_INTENSITY_OFF);
					state = LIGHT_CONTROLLER_ST_OFF;
					break;

				case LIGHT_CONTROLLER_EV_ON:
					light_controller_set_intensity(LIGHT_CONTROLLER_INTENSITY_HIGH);
					state = LIGHT_CONTROLLER_ST_BRIGHT;
					break;

				default:
					break;
			}
			break;
		}

		case LIGHT_CONTROLLER_ST_BRIGHT:{
			switch (event) {
				case LIGHT_CONTROLLER_EV_OFF:
					light_controller_set_intensity(LIGHT_CONTROLLER_INTENSITY_OFF);
					state = LIGHT_CONTROLLER_ST_OFF;
					break;

				case LIGHT_CONTROLLER_EV_ON:
					light_controller_set_intensity(LIGHT_CONTROLLER_INTENSITY_LOW);
					state = LIGHT_CONTROLLER_ST_DIM;
					break;

				default:
					break;
			}
			break;
		}

	default:
		break;

	}
}

void mealy_run_example(void)
{
	static light_controller_events_t event = LIGHT_CONTROLLER_EV_NONE;
	static uint8_t received_command = 0;
	user_pwm1_start();
	while(1) {
		if (!user_uart1_receive_data(&received_command, 1)) {
			continue;
		}

		if      (received_command == '1') {event = LIGHT_CONTROLLER_EV_ON;}
		else if (received_command == '0') {event = LIGHT_CONTROLLER_EV_OFF;}

		if (event != LIGHT_CONTROLLER_EV_NONE) {
			light_controller_state_machine_run(event);
			event = LIGHT_CONTROLLER_EV_NONE;
		}
	}
}
