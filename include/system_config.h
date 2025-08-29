#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "stm32f1xx_hal.h"

// System clock configuration
#define SYSTEM_CLOCK_FREQ_HZ 72000000 // 72MHz system clock
#define HSE_FREQ_HZ 8000000           // 8MHz external crystal
#define PLL_MULTIPLIER 9              // PLL multiplier for 72MHz

// Bus clock dividers
#define AHB_PRESCALER RCC_SYSCLK_DIV1 // AHB = SYSCLK
#define APB1_PRESCALER RCC_HCLK_DIV2  // APB1 = HCLK/2
#define APB2_PRESCALER RCC_HCLK_DIV1  // APB2 = HCLK

// Flash latency for 72MHz
#define FLASH_LATENCY FLASH_LATENCY_2

// Function prototypes
void SystemClock_Config(void);
void Error_Handler(void);

#endif // SYSTEM_CONFIG_H
