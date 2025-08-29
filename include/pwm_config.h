#ifndef PWM_CONFIG_H
#define PWM_CONFIG_H

#include "stm32f1xx_hal.h"

// PWM input channels configuration
#define PWM_INPUT_1_PIN GPIO_PIN_0 // PA0 - TIM2_CH1
#define PWM_INPUT_1_PORT GPIOA
#define PWM_INPUT_1_TIMER TIM2
#define PWM_INPUT_1_CHANNEL TIM_CHANNEL_1

#define PWM_INPUT_2_PIN GPIO_PIN_1 // PA1 - TIM2_CH2
#define PWM_INPUT_2_PORT GPIOA
#define PWM_INPUT_2_TIMER TIM2
#define PWM_INPUT_2_CHANNEL TIM_CHANNEL_2

#define PWM_INPUT_3_PIN GPIO_PIN_2 // PA2 - TIM2_CH3
#define PWM_INPUT_3_PORT GPIOA
#define PWM_INPUT_3_TIMER TIM2
#define PWM_INPUT_3_CHANNEL TIM_CHANNEL_3

// PWM output channels configuration
#define PWM_OUTPUT_1_PIN GPIO_PIN_6 // PA6 - TIM3_CH1
#define PWM_OUTPUT_1_PORT GPIOA
#define PWM_OUTPUT_1_TIMER TIM3
#define PWM_OUTPUT_1_CHANNEL TIM_CHANNEL_1

#define PWM_OUTPUT_2_PIN GPIO_PIN_7 // PA7 - TIM3_CH2
#define PWM_OUTPUT_2_PORT GPIOA
#define PWM_OUTPUT_2_TIMER TIM3
#define PWM_OUTPUT_2_CHANNEL TIM_CHANNEL_2

#define PWM_OUTPUT_3_PIN GPIO_PIN_8 // PA8 - TIM1_CH1
#define PWM_OUTPUT_3_PORT GPIOA
#define PWM_OUTPUT_3_TIMER TIM1
#define PWM_OUTPUT_3_CHANNEL TIM_CHANNEL_1

// PWM signal specifications
#define PWM_INPUT_MIN_PULSE_WIDTH 1000 // 1000μs minimum pulse width
#define PWM_INPUT_MAX_PULSE_WIDTH 2000 // 2000μs maximum pulse width
#define PWM_OUTPUT_PERIOD 1000         // 1ms period (1000Hz)
#define PWM_OUTPUT_MAX_COMPARE 1000    // Maximum compare value

// Timer prescaler for 1MHz clock (72MHz / 72 = 1MHz)
#define TIMER_PRESCALER 71

#endif // PWM_CONFIG_H
