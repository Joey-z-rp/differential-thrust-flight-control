#include "gpio_config.h"

// GPIO System Configuration
static const GPIO_PinConfig_t system_pins[] = {
    {.port = GPIOC, .pin = GPIO_PIN_13, .mode = GPIO_MODE_OUTPUT_PP, .pull = GPIO_PULLUP, .speed = GPIO_SPEED_FREQ_HIGH}, // LED pin
    {.port = GPIOA, .pin = GPIO_PIN_8, .mode = GPIO_MODE_AF_INPUT, .pull = GPIO_NOPULL, .speed = GPIO_SPEED_FREQ_HIGH},   // PWM Input pin PA8 (TIM1_CH1)
    {.port = GPIOA, .pin = GPIO_PIN_10, .mode = GPIO_MODE_AF_INPUT, .pull = GPIO_NOPULL, .speed = GPIO_SPEED_FREQ_HIGH},  // PWM Input pin PA10 (TIM1_CH3)
    {.port = GPIOB, .pin = GPIO_PIN_6, .mode = GPIO_MODE_AF_INPUT, .pull = GPIO_NOPULL, .speed = GPIO_SPEED_FREQ_HIGH},   // PWM Input pin PB6 (TIM4_CH1)
                                                                                                                          // Add more system pins here as needed
};

void GPIO_Init(void)
{
  GPIO_InitMultiplePins(system_pins, sizeof(system_pins) / sizeof(system_pins[0]));
}

// Enable clock for a specific GPIO port
void GPIO_EnablePortClock(GPIO_TypeDef *port)
{
  if (port == GPIOA)
  {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  else if (port == GPIOB)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  else if (port == GPIOC)
  {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  }
  else if (port == GPIOD)
  {
    __HAL_RCC_GPIOD_CLK_ENABLE();
  }
  else if (port == GPIOE)
  {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  }
}

// Initialize a single GPIO pin
void GPIO_InitPin(const GPIO_PinConfig_t *config)
{
  // Enable port clock if not already enabled
  GPIO_EnablePortClock(config->port);

  // Configure the pin
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = config->pin;
  GPIO_InitStruct.Mode = config->mode;
  GPIO_InitStruct.Pull = config->pull;
  GPIO_InitStruct.Speed = config->speed;

  HAL_GPIO_Init(config->port, &GPIO_InitStruct);
}

// Initialize multiple GPIO pins
void GPIO_InitMultiplePins(const GPIO_PinConfig_t *configs, uint8_t count)
{
  for (uint8_t i = 0; i < count; i++)
  {
    GPIO_InitPin(&configs[i]);
  }
}
