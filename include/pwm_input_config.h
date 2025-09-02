#ifndef PWM_INPUT_CONFIG_H
#define PWM_INPUT_CONFIG_H

#include "stm32f1xx_hal.h"

// PWM Input Channel Definitions
#define PWM_INPUT_CHANNEL_PA8 0  // PA8 - TIM1_CH1
#define PWM_INPUT_CHANNEL_PA10 1 // PA10 - TIM1_CH3
#define PWM_INPUT_CHANNEL_PB6 2  // PB6 - TIM4_CH1
#define PWM_INPUT_CHANNEL_COUNT 3

// PWM Input Data Structure
typedef struct
{
  uint32_t frequency;
  float duty_cycle;
  uint32_t period;
  uint32_t pulse_width;
  uint8_t valid;
} pwm_input_data_t;

// Function Declarations
void PWM_Input_Init(void);
pwm_input_data_t *PWM_Input_GetData(uint8_t channel);
uint32_t PWM_Input_GetFrequency(uint8_t channel);
float PWM_Input_GetDutyCycle(uint8_t channel);
uint32_t PWM_Input_GetPeriod(uint8_t channel);
uint32_t PWM_Input_GetPulseWidth(uint8_t channel);
uint8_t PWM_Input_IsValid(uint8_t channel);

#endif // PWM_INPUT_CONFIG_H
