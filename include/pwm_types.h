#ifndef PWM_TYPES_H
#define PWM_TYPES_H

#include <stdint.h>

// PWM input data structure
typedef struct
{
  volatile uint32_t pulse_width; // Pulse width in microseconds
  volatile uint32_t period;      // Period in microseconds
  volatile uint8_t valid;        // Data validity flag
} PWM_Input_Data_t;

// PWM output data structure
typedef struct
{
  uint32_t compare_value; // Timer compare value
  uint32_t duty_cycle;    // Duty cycle percentage (0-100)
} PWM_Output_Data_t;

// PWM controller state
typedef enum
{
  PWM_STATE_INIT,  // Initialization state
  PWM_STATE_READY, // Ready to process
  PWM_STATE_ERROR  // Error state
} PWM_State_t;

// PWM channel enumeration
typedef enum
{
  PWM_CHANNEL_1 = 0,
  PWM_CHANNEL_2 = 1,
  PWM_CHANNEL_3 = 2,
  PWM_CHANNEL_COUNT = 3
} PWM_Channel_t;

#endif // PWM_TYPES_H
