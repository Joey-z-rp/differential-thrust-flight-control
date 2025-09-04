#include "flight_control.h"

// Flight control configuration variables
static float control_sensitivity = CONTROL_SENSITIVITY;
static float max_throttle = MAX_THROTTLE_PERCENT;

// Current flight control state
static flight_control_inputs_t current_inputs = {0};
static motor_outputs_t current_outputs = {0};

// Forward declarations for static functions
static void FlightControl_CalculateMotorOutputs(const flight_control_inputs_t *inputs, motor_outputs_t *outputs);
static void FlightControl_ApplyMotorOutputs(const motor_outputs_t *outputs);
static float FlightControl_Clamp(float value, float min_val, float max_val);
static float FlightControl_ApplyDeadband(float value, float deadband);

// Clamp value between min and max
static float FlightControl_Clamp(float value, float min_val, float max_val)
{
  if (value < min_val)
    return min_val;
  if (value > max_val)
    return max_val;
  return value;
}

// Apply deadband to prevent motor stuttering
static float FlightControl_ApplyDeadband(float value, float deadband)
{
  if (value < deadband && value > 0.0f)
  {
    return 0.0f; // Below deadband, set to zero
  }
  return value; // Above deadband, keep original value
}

// Apply motor outputs to PWM channels
static void FlightControl_ApplyMotorOutputs(const motor_outputs_t *outputs)
{
  if (outputs == NULL)
    return;

  // Set PWM outputs for each motor
  PWM_SetPulseWidthPercentage(MOTOR_LEFT_OUTER, (uint32_t)outputs->left_outer);
  PWM_SetPulseWidthPercentage(MOTOR_LEFT_INNER, (uint32_t)outputs->left_inner);
  PWM_SetPulseWidthPercentage(MOTOR_RIGHT_INNER, (uint32_t)outputs->right_inner);
  PWM_SetPulseWidthPercentage(MOTOR_RIGHT_OUTER, (uint32_t)outputs->right_outer);
}

// Calculate motor outputs based on flight control inputs
static void FlightControl_CalculateMotorOutputs(const flight_control_inputs_t *inputs, motor_outputs_t *outputs)
{
  if (inputs == NULL || outputs == NULL)
    return;

  // Apply control sensitivity to roll and pitch
  float roll_control = inputs->roll * control_sensitivity;
  float pitch_control = inputs->pitch * control_sensitivity;

  // Calculate base motor outputs with differential thrust
  // For left outer, left inner, right inner, right outer layout:
  // Left side motors: throttle + roll control
  // Right side motors: throttle - roll control
  // Outer motors: throttle + pitch control (more thrust for forward pitch)
  // Inner motors: throttle - pitch control (less thrust for forward pitch)

  float left_outer_base = inputs->throttle + roll_control + pitch_control;
  float left_inner_base = inputs->throttle + roll_control - pitch_control;
  float right_inner_base = inputs->throttle - roll_control - pitch_control;
  float right_outer_base = inputs->throttle - roll_control + pitch_control;

  // Clamp motor outputs to valid range
  outputs->left_outer = FlightControl_Clamp(left_outer_base, 0.0f, 100.0f);
  outputs->left_inner = FlightControl_Clamp(left_inner_base, 0.0f, 100.0f);
  outputs->right_inner = FlightControl_Clamp(right_inner_base, 0.0f, 100.0f);
  outputs->right_outer = FlightControl_Clamp(right_outer_base, 0.0f, 100.0f);

  // Apply deadband to prevent motor stuttering at low outputs
  outputs->left_outer = FlightControl_ApplyDeadband(outputs->left_outer, MOTOR_DEADBAND);
  outputs->left_inner = FlightControl_ApplyDeadband(outputs->left_inner, MOTOR_DEADBAND);
  outputs->right_inner = FlightControl_ApplyDeadband(outputs->right_inner, MOTOR_DEADBAND);
  outputs->right_outer = FlightControl_ApplyDeadband(outputs->right_outer, MOTOR_DEADBAND);
}

// Initialize flight control module
void FlightControl_Init(void)
{
  // Initialize all values to zero
  current_inputs.throttle = 0.0f;
  current_inputs.roll = 0.0f;
  current_inputs.pitch = 0.0f;
  current_inputs.yaw = 0.0f;

  current_outputs.left_outer = 0.0f;
  current_outputs.left_inner = 0.0f;
  current_outputs.right_inner = 0.0f;
  current_outputs.right_outer = 0.0f;

  // Set initial motor outputs to minimum
  FlightControl_ApplyMotorOutputs(&current_outputs);
}

// Update flight control - call this in main loop
void FlightControl_Update(void)
{
  // Read PWM inputs and convert to flight control inputs
  FlightControl_GetInputs(&current_inputs);

  // Calculate motor outputs based on inputs
  FlightControl_CalculateMotorOutputs(&current_inputs, &current_outputs);

  // Apply motor outputs to PWM channels
  FlightControl_ApplyMotorOutputs(&current_outputs);
}

// Get current flight control inputs from PWM channels
void FlightControl_GetInputs(flight_control_inputs_t *inputs)
{
  if (inputs == NULL)
    return;

  // Read throttle (0-100%)
  inputs->throttle = PWM_Input_GetPercentage(THROTTLE_CHANNEL);
  inputs->throttle = FlightControl_Clamp(inputs->throttle, MIN_THROTTLE_PERCENT, max_throttle);

  // Read roll (-100% to +100%, centered at 50%)
  float roll_raw = PWM_Input_GetPercentage(ROLL_CHANNEL);
  inputs->roll = (roll_raw - 50.0f) * 2.0f; // Convert 0-100% to -100% to +100%
  inputs->roll = FlightControl_Clamp(inputs->roll, -100.0f, 100.0f);

  // Read pitch (-100% to +100%, centered at 50%)
  float pitch_raw = PWM_Input_GetPercentage(PITCH_CHANNEL);
  inputs->pitch = (pitch_raw - 50.0f) * 2.0f; // Convert 0-100% to -100% to +100%
  inputs->pitch = FlightControl_Clamp(inputs->pitch, -100.0f, 100.0f);

  // Yaw is not currently implemented (no 4th input channel)
  inputs->yaw = 0.0f;
}