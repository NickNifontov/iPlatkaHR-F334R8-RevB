/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hrtim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define HZ50_AB 225
#define HZ50_DEADZONE 5

enum HZ_State{AZone1, AZone2, BZone1, BZone2, DeadZoneAB, DeadZoneBA};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
enum HZ_State HZ50_State=AZone1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void IGLAflagSET(void)
{
	GPIOC->BRR = GPIO_BRR_BR_13;
}

void IGLAflagRESET(void)
{
	GPIOC->BSRR = GPIO_BSRR_BS_13;
}


void DEADZONEflagRESET(void)
{
	GPIOC->BSRR = GPIO_BSRR_BS_15;
}

void DEADZONEflagSET(void)
{
	GPIOC->BRR = GPIO_BRR_BR_15;
}

void UCCdrvOFF(void)
{
	GPIOD->BRR = GPIO_BRR_BR_2;
}

void UCCdrvON(void)
{
	GPIOD->BSRR = GPIO_BSRR_BS_2;
}

void OutputOFF(void)
{
	HRTIM1->sCommonRegs.ODISR = HRTIM_ODISR_TB1ODIS  + HRTIM_ODISR_TB2ODIS;
	HRTIM1->sCommonRegs.OENR = HRTIM_OENR_TE2OEN;

}

void OutputON(void)
{
	HRTIM1->sCommonRegs.OENR = HRTIM_OENR_TB1OEN  + HRTIM_OENR_TB2OEN + HRTIM_OENR_TE2OEN;
}


void HZ50_OFF(void)
{
	 GPIOA->BRR = GPIO_BRR_BR_8;
	 GPIOA->BRR = GPIO_BRR_BR_9;
}

void HZ50_BON(void)
{
	 GPIOA->BRR = GPIO_BRR_BR_8;
	 GPIOA->BSRR = GPIO_BSRR_BS_9;
}

void HZ50_AON(void)
{
	 GPIOA->BRR = GPIO_BRR_BR_9;
	 GPIOA->BSRR = GPIO_BSRR_BS_8;
}

void HZ50_Stop(void) {
	HZ50_OFF();

	//
	UCCdrvOFF();

	DEADZONEflagSET();

	OutputOFF();
	HAL_HRTIM_WaveformCountStop_IT(&hhrtim1, HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_B
			| HRTIM_TIMERID_TIMER_E);

	IGLAflagRESET(); // off IGLA
}

void HZ50_Start(void)
{
	  HZ50_State=DeadZoneBA; // curr zone

	  HAL_HRTIM_RepetitionEventCallback(&hhrtim1,HRTIM_TIMERINDEX_MASTER);

	  /*HRTIM1->sMasterRegs.MREP=0;
	  HZ50_OFF(); // start from A 50Hz
	  OutputOFF();*/

	  HAL_HRTIM_WaveformCountStart_IT(&hhrtim1, HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_B
			  | HRTIM_TIMERID_TIMER_E);

	  UCCdrvON();
	  DEADZONEflagRESET();

	  IGLAflagRESET(); // off IGLA
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin== PEREK_Pin) {
		__HAL_GPIO_EXTI_CLEAR_IT(PEREK_Pin);
		if ((HAL_GPIO_ReadPin(PEREK_GPIO_Port,PEREK_Pin)==GPIO_PIN_RESET)) {
			HZ50_Start();
		} else {
			HZ50_Stop();
		}
	 }
}

void HAL_HRTIM_RepetitionEventCallback(HRTIM_HandleTypeDef *hhrtim,
                                              uint32_t TimerIdx)
{
		switch(HZ50_State) {
				case DeadZoneBA:
					HZ50_State=AZone1; // curr zone
					HRTIM1->sMasterRegs.MREP=HZ50_DEADZONE;
					HZ50_AON();
					OutputON();
					UCCdrvON();

					DEADZONEflagRESET(); // reset
					break;

				case BZone1:
					HZ50_State=BZone2; // curr zone
					HRTIM1->sMasterRegs.MREP=HZ50_AB;
					break;
				case BZone2:
					UCCdrvOFF();
					HZ50_OFF();
					OutputOFF();

					DEADZONEflagSET(); // set

					HZ50_State=DeadZoneBA; // curr zone
					HRTIM1->sMasterRegs.MREP=HZ50_AB;
					break;

				case DeadZoneAB:
					HZ50_State=BZone1; // curr zone
					HRTIM1->sMasterRegs.MREP=HZ50_DEADZONE;
					HZ50_BON();
					OutputON();
					UCCdrvON();

					DEADZONEflagRESET(); // reset
					break;

				case AZone1:
					HZ50_State=AZone2; // curr zone
					HRTIM1->sMasterRegs.MREP=HZ50_AB;
					break;
				case AZone2:
					UCCdrvOFF();
					HZ50_OFF();
					OutputOFF();

					DEADZONEflagSET(); // set

					HZ50_State=DeadZoneAB; // curr zone
					HRTIM1->sMasterRegs.MREP=HZ50_AB;
					break;
			}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_HRTIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_EXTI_Callback(PEREK_Pin);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_HRTIM1;
  PeriphClkInit.Hrtim1ClockSelection = RCC_HRTIM1CLK_PLLCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
