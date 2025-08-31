#include "stm32f1xx_hal.h"
#include "gpio_config.h"
#include "pwm_output_config.h"

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

    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(1);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}
