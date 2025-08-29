#ifndef GPIO_MANAGER_H
#define GPIO_MANAGER_H

#include "stm32f1xx_hal.h"
#include "pwm_config.h"

// Function prototypes
void GPIO_Init(void);
void GPIO_ConfigurePWMInputs(void);
void GPIO_ConfigurePWMOutputs(void);

// Error handler declaration
void Error_Handler(void);

#endif // GPIO_MANAGER_H
