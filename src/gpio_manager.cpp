#include "gpio_manager.h"
#include "system_config.h"

void GPIO_Init(void)
{
  GPIO_ConfigurePWMInputs();
  GPIO_ConfigurePWMOutputs();
}

void GPIO_ConfigurePWMInputs(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Enable GPIOA clock
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // Configure PWM input pins (PA0, PA1, PA2)
  GPIO_InitStruct.Pin = PWM_INPUT_1_PIN | PWM_INPUT_2_PIN | PWM_INPUT_3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void GPIO_ConfigurePWMOutputs(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Configure PWM output pins (PA6, PA7, PA8)
  GPIO_InitStruct.Pin = PWM_OUTPUT_1_PIN | PWM_OUTPUT_2_PIN | PWM_OUTPUT_3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
