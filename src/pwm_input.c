#include "pwm_input_config.h"

// PWM Input Timer Handles
TIM_HandleTypeDef htim1; // For PA8 and PA10 (TIM1_CH1 and TIM1_CH3)
TIM_HandleTypeDef htim4; // For PB6 (TIM4_CH1)

// Array to store PWM input data for 3 channels
pwm_input_data_t pwm_inputs[PWM_INPUT_CHANNEL_COUNT] = {0};

// Timer configuration structure
typedef struct
{
  TIM_HandleTypeDef *htim;
  uint32_t rcc_clk;
  IRQn_Type irqn;
  uint8_t channel_count;
  struct
  {
    uint8_t channel;
    uint32_t period_channel;
    uint32_t duty_channel;
  } channels[2]; // Max 2 channels per timer
} pwm_timer_config_t;

// Timer configuration lookup table
static const pwm_timer_config_t timer_configs[] = {
    {.htim = &htim1,
     .rcc_clk = RCC_APB2ENR_TIM1EN,
     .irqn = TIM1_CC_IRQn,
     .channel_count = 2,
     .channels = {
         {PWM_INPUT_CHANNEL_PA8, TIM_CHANNEL_1, TIM_CHANNEL_2},
         {PWM_INPUT_CHANNEL_PA10, TIM_CHANNEL_3, TIM_CHANNEL_4}}},
    {.htim = &htim4, .rcc_clk = RCC_APB1ENR_TIM4EN, .irqn = TIM4_IRQn, .channel_count = 1, .channels = {{PWM_INPUT_CHANNEL_PB6, TIM_CHANNEL_1, TIM_CHANNEL_2}}}};

static void PWM_Input_Timer_Init(const pwm_timer_config_t *config)
{
  // Enable timer clock
  if (config->htim == &htim1)
  {
    __HAL_RCC_TIM1_CLK_ENABLE();
    config->htim->Instance = TIM1;
  }
  else if (config->htim == &htim4)
  {
    __HAL_RCC_TIM4_CLK_ENABLE();
    config->htim->Instance = TIM4;
  }

  // Configure timer for PWM input mode
  config->htim->Init.Prescaler = 8; // 8MHz / 8 = 1MHz timer clock
  config->htim->Init.CounterMode = TIM_COUNTERMODE_UP;
  config->htim->Init.Period = 65535;
  config->htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  config->htim->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  if (HAL_TIM_Base_Init(config->htim) != HAL_OK)
  {
    // Error handling
    while (1)
      ;
  }

  // Configure slave mode - timer will reset on TI1 rising edge
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1; // TI1 as trigger
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sSlaveConfig.TriggerPrescaler = TIM_ICPSC_DIV1;
  sSlaveConfig.TriggerFilter = 0;

  if (HAL_TIM_SlaveConfigSynchro(config->htim, &sSlaveConfig) != HAL_OK)
  {
    // Error handling
    while (1)
      ;
  }

  // Configure input capture channels for each PWM input
  TIM_IC_InitTypeDef sConfigIC = {0};

  for (uint8_t i = 0; i < config->channel_count; i++)
  {
    // Configure period measurement channel (rising edge, direct)
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;

    if (HAL_TIM_IC_ConfigChannel(config->htim, &sConfigIC, config->channels[i].period_channel) != HAL_OK)
    {
      // Error handling
      while (1)
        ;
    }

    // Configure duty cycle measurement channel (falling edge, indirect)
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;

    if (HAL_TIM_IC_ConfigChannel(config->htim, &sConfigIC, config->channels[i].duty_channel) != HAL_OK)
    {
      // Error handling
      while (1)
        ;
    }
  }

  // Configure NVIC for timer interrupt
  HAL_NVIC_SetPriority(config->irqn, 0, 0);
  HAL_NVIC_EnableIRQ(config->irqn);

  // Start input capture on all channels
  for (uint8_t i = 0; i < config->channel_count; i++)
  {
    HAL_TIM_IC_Start_IT(config->htim, config->channels[i].period_channel); // Period measurement
    HAL_TIM_IC_Start(config->htim, config->channels[i].duty_channel);      // Duty cycle measurement
  }
}

// Main PWM Input Initialization Function
void PWM_Input_Init(void)
{
  // Initialize all PWM input timers using configuration table
  for (uint8_t i = 0; i < sizeof(timer_configs) / sizeof(timer_configs[0]); i++)
  {
    PWM_Input_Timer_Init(&timer_configs[i]);
  }
}

// PWM Input Capture Callback
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  // Find the timer configuration
  const pwm_timer_config_t *config = NULL;
  for (uint8_t i = 0; i < sizeof(timer_configs) / sizeof(timer_configs[0]); i++)
  {
    if (timer_configs[i].htim == htim)
    {
      config = &timer_configs[i];
      break;
    }
  }

  if (config == NULL)
  {
    return; // Unknown timer
  }

  // Find which channel triggered
  uint8_t channel = 0;
  uint32_t period_channel = 0, duty_channel = 0;

  for (uint8_t i = 0; i < config->channel_count; i++)
  {
    if (htim->Channel == (config->channels[i].period_channel == TIM_CHANNEL_1 ? HAL_TIM_ACTIVE_CHANNEL_1 : config->channels[i].period_channel == TIM_CHANNEL_3 ? HAL_TIM_ACTIVE_CHANNEL_3
                                                                                                                                                               : 0))
    {
      channel = config->channels[i].channel;
      period_channel = config->channels[i].period_channel;
      duty_channel = config->channels[i].duty_channel;
      break;
    }
  }

  // Read the period (cycle time)
  pwm_inputs[channel].period = HAL_TIM_ReadCapturedValue(htim, period_channel);

  if (pwm_inputs[channel].period != 0)
  {
    // Read the pulse width (high time)
    pwm_inputs[channel].pulse_width = HAL_TIM_ReadCapturedValue(htim, duty_channel);

    // Calculate duty cycle percentage
    pwm_inputs[channel].duty_cycle = (float)pwm_inputs[channel].pulse_width * 100.0f / (float)pwm_inputs[channel].period;

    // Calculate frequency (1MHz timer clock / period)
    pwm_inputs[channel].frequency = 1000000 / pwm_inputs[channel].period;

    // Mark as valid
    pwm_inputs[channel].valid = 1;
  }
}

// Get PWM input data for a specific channel
pwm_input_data_t *PWM_Input_GetData(uint8_t channel)
{
  if (channel >= PWM_INPUT_CHANNEL_COUNT)
    return NULL;
  return &pwm_inputs[channel];
}

// Get PWM input frequency for a specific channel
uint32_t PWM_Input_GetFrequency(uint8_t channel)
{
  if (channel >= PWM_INPUT_CHANNEL_COUNT || !pwm_inputs[channel].valid)
    return 0;
  return pwm_inputs[channel].frequency;
}

// Get PWM input duty cycle for a specific channel
float PWM_Input_GetDutyCycle(uint8_t channel)
{
  if (channel >= PWM_INPUT_CHANNEL_COUNT || !pwm_inputs[channel].valid)
    return 0.0f;
  return pwm_inputs[channel].duty_cycle;
}

// Get PWM input period for a specific channel
uint32_t PWM_Input_GetPeriod(uint8_t channel)
{
  if (channel >= PWM_INPUT_CHANNEL_COUNT || !pwm_inputs[channel].valid)
    return 0;
  return pwm_inputs[channel].period;
}

// Get PWM input pulse width for a specific channel
uint32_t PWM_Input_GetPulseWidth(uint8_t channel)
{
  if (channel >= PWM_INPUT_CHANNEL_COUNT || !pwm_inputs[channel].valid)
    return 0;
  return pwm_inputs[channel].pulse_width;
}

// Check if PWM input data is valid for a specific channel
uint8_t PWM_Input_IsValid(uint8_t channel)
{
  if (channel >= PWM_INPUT_CHANNEL_COUNT)
    return 0;
  return pwm_inputs[channel].valid;
}
