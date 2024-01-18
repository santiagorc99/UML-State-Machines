#include <user_pwm.h>
#include "main.h"

extern TIM_HandleTypeDef htim1;

/**
 * Bear in mind that:
 * - The timer clock is 1MHz (Precalar is 72).
 * - The frequency is 10kHz (Autorealod is 100).
 * - Duty cycle (%) is CCR value * 100 / ARR (Autoreload) -> CCR = Duty_cycle(%) * ARR / 100
 */
void user_pwm1_start(void)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void user_pwm1_set_duty_cylcle(uint8_t duty)
{
	TIM1->CCR1 = duty * (htim1.Instance->ARR + 1)/100;
}
