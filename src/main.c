#include "stm32f1xx_hal.h"
#include "gpio_config.h"
#include "pwm_output_config.h"
#include "pwm_input_config.h"
#include <stdio.h>

// External timer handles from pwm_input.c
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;

// Helper function to cycle PWM duty cycle
void cycle_pwm_duty(uint8_t *duty, uint8_t *direction, uint8_t min_val, uint8_t max_val)
{
  if (*direction == 0)
  {
    (*duty)++;
    if (*duty >= max_val)
    {
      *duty = max_val;
      *direction = 1;
    }
  }
  else
  {
    (*duty)--;
    if (*duty <= min_val)
    {
      *duty = min_val;
      *direction = 0;
    }
  }
}

int main(void)
{
  HAL_Init();

  GPIO_Init();
  PWM_Init();
  PWM_Input_Init(); // Initialize PWM input

  // Start PWM output on all three channels
  PWM_Start();

  // PWM configuration arrays
  uint8_t duty_cycles[3] = {0, 0, 0};  // Current duty cycles
  uint8_t directions[3] = {0, 0, 0};   // Direction flags (0=increasing, 1=decreasing)
  uint8_t update_rates[3] = {1, 2, 3}; // Update rates (every N iterations)
  uint8_t counters[3] = {0, 0, 0};     // Counters for update timing
  uint32_t pwm_channels[3] = {PWM_PA0_CHANNEL, PWM_PA1_CHANNEL, PWM_PA2_CHANNEL};

  while (1)
  {
    // Update PWM duty cycles for all channels
    for (int i = 0; i < 3; i++)
    {
      if (++counters[i] >= update_rates[i])
      {
        counters[i] = 0;
        cycle_pwm_duty(&duty_cycles[i], &directions[i], 0, 100);
        PWM_SetDutyCycle(pwm_channels[i], duty_cycles[i]);
      }
    }
    // Display PWM input data for all 3 channels
    for (int i = 0; i < PWM_INPUT_CHANNEL_COUNT; i++)
    {
      if (PWM_Input_IsValid(i))
      {
        volatile float duty_cycle = PWM_Input_GetDutyCycle(i);
        volatile float frequency = PWM_Input_GetFrequency(i);
        volatile float period = PWM_Input_GetPeriod(i);
        volatile float pulse_width = PWM_Input_GetPulseWidth(i);
        printf("PWM Ch%d - Freq: %lu Hz, Duty: %.1f%%, Period: %lu us, Pulse: %lu us\r\n",
               i, frequency, duty_cycle, period, pulse_width);
      }
      else
      {
        printf("PWM Ch%d - No signal detected\r\n", i);
      }
    }

    HAL_Delay(100);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

// TIM1 Interrupt Handler (for PA8 and PA10 PWM input)
void TIM1_CC_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim1);
}

// TIM4 Interrupt Handler (for PB6 PWM input)
void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim4);
}
