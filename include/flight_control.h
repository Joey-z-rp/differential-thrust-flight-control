#ifndef FLIGHT_CONTROL_H
#define FLIGHT_CONTROL_H

#include "stm32f1xx_hal.h"
#include "pwm_input_config.h"
#include "pwm_output_config.h"

// Flight control input channels
#define THROTTLE_CHANNEL PWM_INPUT_CHANNEL_PA8 // PA8 - Throttle input
#define ROLL_CHANNEL PWM_INPUT_CHANNEL_PA10    // PA10 - Roll input
#define PITCH_CHANNEL PWM_INPUT_CHANNEL_PB6    // PB6 - Pitch input

// Motor output channels
#define MOTOR_LEFT_OUTER PWM_PA0_CHANNEL  // PA0 - Left Outer Motor
#define MOTOR_LEFT_INNER PWM_PA1_CHANNEL  // PA1 - Left Inner Motor
#define MOTOR_RIGHT_INNER PWM_PA2_CHANNEL // PA2 - Right Inner Motor
#define MOTOR_RIGHT_OUTER PWM_PA3_CHANNEL // PA3 - Right Outer Motor

// Flight control configuration
#define MAX_THROTTLE_PERCENT 80.0f // Maximum throttle percentage (safety limit)
#define MIN_THROTTLE_PERCENT 0.0f  // Minimum throttle percentage
#define CONTROL_SENSITIVITY 0.2f   // Control sensitivity multiplier (0.0-1.0)
#define MOTOR_DEADBAND 3.0f        // Minimum motor output percentage

// Flight control data structure
typedef struct
{
  float throttle; // Throttle input (0-100%)
  float roll;     // Roll input (-100 to +100%)
  float pitch;    // Pitch input (-100 to +100%)
  float yaw;      // Yaw input (-100 to +100%) - currently not used
} flight_control_inputs_t;

// Motor output structure
typedef struct
{
  float left_outer;  // Left outer motor output (0-100%)
  float left_inner;  // Left inner motor output (0-100%)
  float right_inner; // Right inner motor output (0-100%)
  float right_outer; // Right outer motor output (0-100%)
} motor_outputs_t;

// Function declarations
void FlightControl_Init(void);
void FlightControl_Update(void);
void FlightControl_GetInputs(flight_control_inputs_t *inputs);

#endif // FLIGHT_CONTROL_H
