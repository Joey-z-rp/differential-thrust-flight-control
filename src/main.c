#include "stm32f1xx_hal.h"
#include "gpio_config.h"
#include "pwm_output_config.h"
#include "pwm_input_config.h"

// PWM Input Configuration
TIM_HandleTypeDef htim3;
uint32_t pwm_input_frequency = 0;
float pwm_input_duty_cycle = 0.0;
uint32_t pwm_input_period = 0;
uint32_t pwm_input_pulse_width = 0;

// Helper function to cycle PWM duty cycle
void cycle_pwm_duty(uint8_t *duty, uint8_t *direction, uint8_t min_val, uint8_t max_val)
{
  if (*direction == 0)
  {
    (*duty)++;
    if (*duty >= max_val)
    {
      *duty = max_val;
      *direction = 1;
    }
  }
  else
  {
    (*duty)--;
    if (*duty <= min_val)
    {
      *duty = min_val;
      *direction = 0;
    }
  }
}

// PWM Input Timer Initialization
void PWM_Input_Init(void)
{
  // Enable TIM3 clock
  __HAL_RCC_TIM3_CLK_ENABLE();

  // Configure TIM3 for PWM input mode
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 8; // 8MHz / 8 = 1MHz timer clock
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    // Error handling
    while (1)
      ;
  }

  // Configure TIM3 for PWM input mode
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  // Configure slave mode - TIM3 will reset on TI1 rising edge
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1; // TI1 as trigger
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
  sSlaveConfig.TriggerFilter = 0;

  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    // Error handling
    while (1)
      ;
  }

  // Configure Channel 1 (PA6) for period measurement
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;

  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    // Error handling
    while (1)
      ;
  }

  // Configure Channel 2 for duty cycle measurement
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;

  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    // Error handling
    while (1)
      ;
  }

  // Configure NVIC for TIM3 interrupt
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);

  // Start input capture on both channels
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1); // Start with interrupt for period
  HAL_TIM_IC_Start(&htim3, TIM_CHANNEL_2);    // Start without interrupt for duty
}

// PWM Input Capture Callback
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM3)
  {
    // Read the period (cycle time)
    pwm_input_period = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    if (pwm_input_period != 0)
    {
      // Read the pulse width (high time)
      pwm_input_pulse_width = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

      // Calculate duty cycle percentage
      pwm_input_duty_cycle = (float)pwm_input_pulse_width * 100.0f / (float)pwm_input_period;

      // Calculate frequency (1MHz timer clock / period)
      pwm_input_frequency = 1000000 / pwm_input_period;
    }
  }
}

int main(void)
{
  HAL_Init();

  GPIO_Init();
  PWM_Init();
  PWM_Input_Init(); // Initialize PWM input

  // Start PWM output on all three channels
  PWM_Start();

  // PWM configuration arrays
  uint8_t duty_cycles[3] = {0, 0, 0};  // Current duty cycles
  uint8_t directions[3] = {0, 0, 0};   // Direction flags (0=increasing, 1=decreasing)
  uint8_t update_rates[3] = {1, 2, 3}; // Update rates (every N iterations)
  uint8_t counters[3] = {0, 0, 0};     // Counters for update timing
  uint32_t pwm_channels[3] = {PWM_PA0_CHANNEL, PWM_PA1_CHANNEL, PWM_PA2_CHANNEL};

  while (1)
  {
    // Update PWM duty cycles for all channels
    for (int i = 0; i < 3; i++)
    {
      if (++counters[i] >= update_rates[i])
      {
        counters[i] = 0;
        cycle_pwm_duty(&duty_cycles[i], &directions[i], 0, 100);
        PWM_SetDutyCycle(pwm_channels[i], duty_cycles[i]);
      }
    }
    printf("PWM Input - Freq: %lu Hz, Duty: %.1f%%, Period: %lu us, Pulse: %lu us\r\n",
           pwm_input_frequency, pwm_input_duty_cycle, pwm_input_period, pwm_input_pulse_width);

    HAL_Delay(100);
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

// TIM3 Interrupt Handler
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
}
