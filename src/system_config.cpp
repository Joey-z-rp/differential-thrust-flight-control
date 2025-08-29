#include "system_config.h"

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configure the main internal regulator output voltage
  __HAL_RCC_PWR_CLK_ENABLE();
  // Note: STM32F1xx doesn't use voltage scaling like STM32F4xx

  // Initializes the RCC Oscillators according to the specified parameters
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  // Initializes the CPU, AHB and APB buses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = AHB_PRESCALER;
  RCC_ClkInitStruct.APB1CLKDivider = APB1_PRESCALER;
  RCC_ClkInitStruct.APB2CLKDivider = APB2_PRESCALER;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY) != HAL_OK)
  {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  // User can add his own implementation to report the HAL error return state
  __disable_irq();
  while (1)
  {
    // Error occurred - you can add LED blinking or other error indication here
  }
}
