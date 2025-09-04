#include "stm32f1xx_hal.h"
#include "gpio_config.h"
#include "pwm_output_config.h"
#include "pwm_input_config.h"
#include "flight_control.h"

// External timer handles from pwm_input.c
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim4;

int main(void)
{
  HAL_Init();

  GPIO_Init();
  PWM_Init();
  PWM_Input_Init();     // Initialize PWM input
  FlightControl_Init(); // Initialize flight control module

  // Start PWM output on all four channels
  PWM_Start();

  while (1)
  {
    // Update flight control - this handles all PWM input processing
    // and motor output calculations for differential thrust
    FlightControl_Update();

    HAL_Delay(20); // 50Hz
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
