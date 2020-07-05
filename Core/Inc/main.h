/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IGLA_CMD_Pin GPIO_PIN_13
#define IGLA_CMD_GPIO_Port GPIOC
#define DEADZONE_CMD_Pin GPIO_PIN_15
#define DEADZONE_CMD_GPIO_Port GPIOC
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOF
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOF
#define ADC_V_OP_Pin GPIO_PIN_0
#define ADC_V_OP_GPIO_Port GPIOA
#define ADC_V_Pin GPIO_PIN_1
#define ADC_V_GPIO_Port GPIOA
#define DAC_V_Pin GPIO_PIN_4
#define DAC_V_GPIO_Port GPIOA
#define PEREK_Pin GPIO_PIN_10
#define PEREK_GPIO_Port GPIOB
#define PEREK_EXTI_IRQn EXTI15_10_IRQn
#define IGLA_Pin GPIO_PIN_9
#define IGLA_GPIO_Port GPIOC
#define HZ50B_Pin GPIO_PIN_8
#define HZ50B_GPIO_Port GPIOA
#define HZ50A_Pin GPIO_PIN_9
#define HZ50A_GPIO_Port GPIOA
#define INB_Pin GPIO_PIN_10
#define INB_GPIO_Port GPIOA
#define INA_Pin GPIO_PIN_11
#define INA_GPIO_Port GPIOA
#define FAULT_Pin GPIO_PIN_12
#define FAULT_GPIO_Port GPIOA
#define DIO_Pin GPIO_PIN_13
#define DIO_GPIO_Port GPIOA
#define CLK_Pin GPIO_PIN_14
#define CLK_GPIO_Port GPIOA
#define FAULT2_Pin GPIO_PIN_15
#define FAULT2_GPIO_Port GPIOA
#define DEADZONE_EE1_Pin GPIO_PIN_12
#define DEADZONE_EE1_GPIO_Port GPIOC
#define EN_UCC_Pin GPIO_PIN_2
#define EN_UCC_GPIO_Port GPIOD
#define IGLA_EE3_Pin GPIO_PIN_7
#define IGLA_EE3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
