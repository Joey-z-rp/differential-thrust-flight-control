#include "stm32f1xx_hal.h"
#include "gpio_config.h"
#include "pwm_output_config.h"
#include "pwm_input_config.h"

// External timer handles from pwm_input.c
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;

int main(void)
{
  HAL_Init();

  GPIO_Init();
  PWM_Init();
  PWM_Input_Init(); // Initialize PWM input

  // Start PWM output on all three channels
  PWM_Start();

  // PWM output channels for LEDs
  uint32_t pwm_channels[3] = {PWM_PA0_CHANNEL, PWM_PA1_CHANNEL, PWM_PA2_CHANNEL};

  while (1)
  {
    // Use PWM input percentages to control LED duty cycles
    for (int i = 0; i < PWM_INPUT_CHANNEL_COUNT; i++)
    {
      float percentage = PWM_Input_GetPercentage(i);

      // Convert percentage to duty cycle (0-100)
      uint8_t duty_cycle = (uint8_t)percentage;

      // Set LED duty cycle based on PWM input
      PWM_SetDutyCycle(pwm_channels[i], duty_cycle);
    }

    HAL_Delay(1); // Increased delay for better readability
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
