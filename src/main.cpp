#include "stm32f1xx_hal.h"
#include "system_config.h"
#include "gpio_manager.h"
#include "timer_manager.h"
#include "pwm_processor.h"

// Forward declaration to ensure Error_Handler is available to all modules
void Error_Handler(void);

int main(void)
{
  // Initialize HAL
  HAL_Init();

  // Configure system clock
  SystemClock_Config();

  // Initialize GPIO
  GPIO_Init();

  // Initialize timers
  TIM_Init();

  // Initialize PWM processor
  PWM_Processor_Init();

  // Start PWM input capture
  TIM_StartPWMInputs();

  // Start PWM output
  TIM_StartPWMOutputs();

  // Main loop
  while (1)
  {
    // Update PWM outputs based on input signals
    PWM_Processor_UpdateOutputs();

    // Small delay to prevent overwhelming the system
    HAL_Delay(10);
  }
}
