#include "stm32f1xx_hal.h"
#include "gpio_config.h"

int main(void)
{
  HAL_Init();

  GPIO_Init();

  while (1)
  {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(100);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}
