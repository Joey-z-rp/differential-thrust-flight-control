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
  uint32_t period;
  uint32_t pulse_width;
  uint32_t last_update_time; // Timestamp of last interrupt for this channel
} pwm_input_data_t;

// Unified PWM range for all channels (in microseconds)
#define PWM_MIN_US 889
#define PWM_MAX_US 1778

// Dead zone percentage (3% on each end)
#define PWM_DEAD_ZONE_PERCENT 3.0f

// Function Declarations
void PWM_Input_Init(void);
float PWM_Input_GetPercentage(uint8_t channel);
uint8_t PWM_Input_IsChannelLost(uint8_t channel, uint32_t timeout_ms);
uint8_t PWM_Input_IsAnyChannelLost(uint32_t timeout_ms);

#endif // PWM_INPUT_CONFIG_H
