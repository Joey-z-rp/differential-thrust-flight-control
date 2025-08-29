#include "pwm_processor.h"
#include "timer_manager.h"
#include "system_config.h"

// PWM input and output data arrays
PWM_Input_Data_t pwm_inputs[PWM_CHANNEL_COUNT] = {0};
PWM_Output_Data_t pwm_outputs[PWM_CHANNEL_COUNT] = {0};

// Helper function to process PWM input capture for a specific channel
static void ProcessPWMInputCapture(PWM_Channel_t channel, uint32_t captured_value)
{
  static uint32_t rising_edge_times[PWM_CHANNEL_COUNT] = {0};
  static uint32_t falling_edge_times[PWM_CHANNEL_COUNT] = {0};

  if (captured_value > 0)
  {
    // Rising edge detected
    rising_edge_times[channel] = captured_value;
  }
  else
  {
    // Falling edge detected
    falling_edge_times[channel] = captured_value;

    // Calculate pulse width
    uint32_t rising_time = rising_edge_times[channel];
    uint32_t falling_time = falling_edge_times[channel];

    if (falling_time > rising_time)
    {
      pwm_inputs[channel].pulse_width = falling_time - rising_time;
    }
    else
    {
      // Handle timer overflow case
      pwm_inputs[channel].pulse_width = (0xFFFF - rising_time) + falling_time;
    }

    // Mark data as valid
    pwm_inputs[channel].valid = 1;
  }
}

// Helper function to get channel from timer instance and channel
static PWM_Channel_t GetPWMChannelFromTimer(TIM_HandleTypeDef *htim, uint32_t active_channel)
{
  if (htim->Instance == PWM_INPUT_1_TIMER)
  {
    if (active_channel == HAL_TIM_ACTIVE_CHANNEL_1)
      return PWM_CHANNEL_1;
  }
  else if (htim->Instance == PWM_INPUT_2_TIMER)
  {
    if (active_channel == HAL_TIM_ACTIVE_CHANNEL_2)
      return PWM_CHANNEL_2;
  }
  else if (htim->Instance == PWM_INPUT_3_TIMER)
  {
    if (active_channel == HAL_TIM_ACTIVE_CHANNEL_3)
      return PWM_CHANNEL_3;
  }

  return PWM_CHANNEL_COUNT; // Invalid channel
}

void PWM_Processor_Init(void)
{
  // Initialize PWM input data
  for (int i = 0; i < PWM_CHANNEL_COUNT; i++)
  {
    pwm_inputs[i].pulse_width = 0;
    pwm_inputs[i].period = 0;
    pwm_inputs[i].valid = 0;
  }

  // Initialize PWM output data
  for (int i = 0; i < PWM_CHANNEL_COUNT; i++)
  {
    pwm_outputs[i].compare_value = 0;
    pwm_outputs[i].duty_cycle = 0;
  }
}

void PWM_Processor_UpdateOutputs(void)
{
  // Update all channels
  for (int i = 0; i < PWM_CHANNEL_COUNT; i++)
  {
    PWM_Processor_MapInputToOutput((PWM_Channel_t)i);
  }
}

void PWM_Processor_MapInputToOutput(PWM_Channel_t channel)
{
  if (channel >= PWM_CHANNEL_COUNT)
    return;

  // Calculate compare value from input pulse width
  uint32_t compare_value = PWM_Processor_CalculateCompareValue(
      pwm_inputs[channel].pulse_width);

  // Update output data
  pwm_outputs[channel].compare_value = compare_value;
  pwm_outputs[channel].duty_cycle =
      (compare_value * 100) / PWM_OUTPUT_PERIOD;

  // Update timer compare values based on channel
  switch (channel)
  {
  case PWM_CHANNEL_1:
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, compare_value);
    break;
  case PWM_CHANNEL_2:
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, compare_value);
    break;
  case PWM_CHANNEL_3:
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, compare_value);
    break;
  default:
    break;
  }
}

uint32_t PWM_Processor_CalculateCompareValue(uint32_t pulse_width)
{
  // Map input pulse width (1000-2000 us) to timer compare value (0-1000)
  uint32_t compare_value = (pulse_width * PWM_OUTPUT_PERIOD) /
                           (PWM_INPUT_MAX_PULSE_WIDTH - PWM_INPUT_MIN_PULSE_WIDTH);

  // Clamp values to valid range
  if (compare_value > PWM_OUTPUT_MAX_COMPARE)
    compare_value = PWM_OUTPUT_MAX_COMPARE;

  return compare_value;
}

// PWM input capture callback - now much cleaner!
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  // Get the active channel
  uint32_t active_channel = htim->Channel;

  // Get the captured value
  uint32_t captured_value = 0;
  switch (active_channel)
  {
  case HAL_TIM_ACTIVE_CHANNEL_1:
    captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
    break;
  case HAL_TIM_ACTIVE_CHANNEL_2:
    captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
    break;
  case HAL_TIM_ACTIVE_CHANNEL_3:
    captured_value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
    break;
  default:
    return; // Unknown channel
  }

  // Get the PWM channel from timer instance and active channel
  PWM_Channel_t pwm_channel = GetPWMChannelFromTimer(htim, active_channel);

  // Process the capture if it's a valid channel
  if (pwm_channel < PWM_CHANNEL_COUNT)
  {
    ProcessPWMInputCapture(pwm_channel, captured_value);
  }
}
