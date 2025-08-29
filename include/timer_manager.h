#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include "stm32f1xx_hal.h"
#include "pwm_config.h"

// Timer handles (extern declarations)
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

// Function prototypes
void TIM_Init(void);
void TIM_ConfigurePWMInputs(void);
void TIM_ConfigurePWMOutputs(void);
void TIM_StartPWMInputs(void);
void TIM_StartPWMOutputs(void);

// Error handler declaration
void Error_Handler(void);

#endif // TIMER_MANAGER_H
