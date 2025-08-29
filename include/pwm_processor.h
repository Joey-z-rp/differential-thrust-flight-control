#ifndef PWM_PROCESSOR_H
#define PWM_PROCESSOR_H

#include "pwm_types.h"
#include "pwm_config.h"

// PWM input data (extern declarations)
extern PWM_Input_Data_t pwm_inputs[PWM_CHANNEL_COUNT];
extern PWM_Output_Data_t pwm_outputs[PWM_CHANNEL_COUNT];

// Function prototypes
void PWM_Processor_Init(void);
void PWM_Processor_UpdateOutputs(void);
void PWM_Processor_MapInputToOutput(PWM_Channel_t channel);
uint32_t PWM_Processor_CalculateCompareValue(uint32_t pulse_width);

// PWM input capture callback
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

// Error handler declaration
void Error_Handler(void);

#endif // PWM_PROCESSOR_H
