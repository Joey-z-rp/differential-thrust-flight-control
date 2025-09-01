#ifndef PWM_INPUT_CONFIG_H
#define PWM_INPUT_CONFIG_H

#include "stm32f1xx_hal.h"

// PWM Input Configuration
extern TIM_HandleTypeDef htim3;
extern uint32_t pwm_input_frequency;
extern float pwm_input_duty_cycle;
extern uint32_t pwm_input_period;
extern uint32_t pwm_input_pulse_width;

// Function Declarations
void PWM_Input_Init(void);

#endif // PWM_INPUT_CONFIG_H
