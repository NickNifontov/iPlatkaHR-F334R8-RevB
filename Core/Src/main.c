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
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "hrtim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define PULSE_TOTAL_LENGTH 25345
#define PULSE_DEATHTIME 2304
#define PULSE_LENGTH 23041

#define SOFTSTART_STEP 1
#define SOFTSTART_STEP_MAX 100

#define VOLUME_MAX 100

#define HZ50_AB 225
#define HZ50_DEADZONE 5

#define SIN_ZER0 2
#define PULSE_IN_WAVE 450
#define PULSE_IN_DEATHZONE 5

#define DAC_PULSE_CNT 910//(uint16_t) (2*PULSE_IN_WAVE+2*PULSE_IN_DEATHZONE)

#define MAX_SIN_TABLE_VALUE 3500

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
uint8_t UseVolume=0;
uint8_t Volume_Coeff=0;

uint8_t UseSoftStart=0;
uint8_t SoftStart_Coeff=0;

uint32_t tmp_etalon=0;
uint32_t tmp_orig=0;
uint32_t tmp_etalon_amplitude=MAX_SIN_TABLE_VALUE;
uint32_t tmp_pulse=0;

enum HZ_State HZ50_State=AZone1;

uint16_t Sin_Step=0;

uint16_t Sin_Etalon[DAC_PULSE_CNT]={0, 12, 24, 37, 49, 61, 73, 86, 98, 110, 122, 134, 147, 159, 171, 183, 195,
		208, 220, 232, 244, 256, 269, 281, 293, 305, 317, 329, 342, 354, 366, 378, 390, 402, 414, 427, 439,
		451, 463, 475, 487, 499, 511, 523, 535, 548, 560, 572, 584, 596, 608, 620, 632, 644, 656, 668, 680,
		692, 704, 716, 728, 740, 752, 764, 775, 787, 799, 811, 823, 835, 847, 859, 870, 882, 894, 906, 918,
		929, 941, 953, 965, 976, 988, 1000, 1012, 1023, 1035, 1047, 1058, 1070, 1082, 1093, 1105, 1116, 1128,
		1139, 1151, 1163, 1174, 1186, 1197, 1209, 1220, 1231, 1243, 1254, 1266, 1277, 1288, 1300, 1311, 1322,
		1334, 1345, 1356, 1368, 1379, 1390, 1401, 1412, 1424, 1435, 1446, 1457, 1468, 1479, 1490, 1501, 1512,
		1523, 1534, 1545, 1556, 1567, 1578, 1589, 1600, 1611, 1622, 1632, 1643, 1654, 1665, 1675, 1686, 1697,
		1708, 1718, 1729, 1739, 1750, 1761, 1771, 1782, 1792, 1803, 1813, 1824, 1834, 1844, 1855, 1865, 1875,
		1886, 1896, 1906, 1916, 1927, 1937, 1947, 1957, 1967, 1977, 1987, 1997, 2008, 2018, 2027, 2037, 2047,
		2057, 2067, 2077, 2087, 2097, 2106, 2116, 2126, 2136, 2145, 2155, 2164, 2174, 2184, 2193, 2203, 2212,
		2222, 2231, 2240, 2250, 2259, 2268, 2278, 2287, 2296, 2305, 2315, 2324, 2333, 2342, 2351, 2360, 2369,
		2378, 2387, 2396, 2405, 2414, 2423, 2431, 2440, 2449, 2458, 2466, 2475, 2483, 2492, 2501, 2509, 2518,
		2526, 2535, 2543, 2551, 2560, 2568, 2576, 2585, 2593, 2601, 2609, 2617, 2625, 2633, 2641, 2649, 2657,
		2665, 2673, 2681, 2689, 2697, 2705, 2712, 2720, 2728, 2735, 2743, 2750, 2758, 2766, 2773, 2780, 2788,
		2795, 2803, 2810, 2817, 2824, 2832, 2839, 2846, 2853, 2860, 2867, 2874, 2881, 2888, 2895, 2902, 2908,
		2915, 2922, 2929, 2935, 2942, 2949, 2955, 2962, 2968, 2975, 2981, 2987, 2994, 3000, 3006, 3013, 3019,
		3025, 3031, 3037, 3043, 3049, 3055, 3061, 3067, 3073, 3079, 3085, 3090, 3096, 3102, 3107, 3113, 3119,
		3124, 3130, 3135, 3140, 3146, 3151, 3156, 3162, 3167, 3172, 3177, 3182, 3187, 3192, 3197, 3202, 3207,
		3212, 3217, 3222, 3227, 3231, 3236, 3241, 3245, 3250, 3254, 3259, 3263, 3268, 3272, 3276, 3280, 3285,
		3289, 3293, 3297, 3301, 3305, 3309, 3313, 3317, 3321, 3325, 3329, 3332, 3336, 3340, 3343, 3347, 3351,
		3354, 3358, 3361, 3364, 3368, 3371, 3374, 3378, 3381, 3384, 3387, 3390, 3393, 3396, 3399, 3402, 3405,
		3408, 3410, 3413, 3416, 3418, 3421, 3424, 3426, 3429, 3431, 3433, 3436, 3438, 3440, 3443, 3445, 3447,
		3449, 3451, 3453, 3455, 3457, 3459, 3461, 3462, 3464, 3466, 3468, 3469, 3471, 3472, 3474, 3475, 3477,
		3478, 3480, 3481, 3482, 3483, 3484, 3486, 3487, 3488, 3489, 3490, 3491, 3491, 3492, 3493, 3494, 3495,
		3495, 3496, 3496, 3497, 3497, 3498, 3498, 3499, 3499, 3499, 3499, 3500, 3500, 3500, 3500, 3500, 3500,
		3500, 3500, 3500, 3499, 3499, 3499, 3499, 3498, 3498, 3497, 3497, 3496, 3496, 3495, 3495, 3494, 3493,
		3492, 3491, 3491, 3490, 3489, 3488, 3487, 3486, 3484, 3483, 3482, 3481, 3480, 3478, 3477, 3475, 3474,
		3472, 3471, 3469, 3468, 3466, 3464, 3462, 3461, 3459, 3457, 3455, 3453, 3451, 3449, 3447, 3445, 3443,
		3440, 3438, 3436, 3433, 3431, 3429, 3426, 3424, 3421, 3418, 3416, 3413, 3410, 3408, 3405, 3402, 3399,
		3396, 3393, 3390, 3387, 3384, 3381, 3378, 3374, 3371, 3368, 3364, 3361, 3358, 3354, 3351, 3347, 3343,
		3340, 3336, 3332, 3329, 3325, 3321, 3317, 3313, 3309, 3305, 3301, 3297, 3293, 3289, 3285, 3280, 3276,
		3272, 3268, 3263, 3259, 3254, 3250, 3245, 3241, 3236, 3231, 3227, 3222, 3217, 3212, 3207, 3202, 3197,
		3192, 3187, 3182, 3177, 3172, 3167, 3162, 3156, 3151, 3146, 3140, 3135, 3130, 3124, 3119, 3113, 3107,
		3102, 3096, 3090, 3085, 3079, 3073, 3067, 3061, 3055, 3049, 3043, 3037, 3031, 3025, 3019, 3013, 3006,
		3000, 2994, 2987, 2981, 2975, 2968, 2962, 2955, 2949, 2942, 2935, 2929, 2922, 2915, 2908, 2902, 2895,
		2888, 2881, 2874, 2867, 2860, 2853, 2846, 2839, 2832, 2824, 2817, 2810, 2803, 2795, 2788, 2780, 2773,
		2766, 2758, 2750, 2743, 2735, 2728, 2720, 2712, 2705, 2697, 2689, 2681, 2673, 2665, 2657, 2649, 2641,
		2633, 2625, 2617, 2609, 2601, 2593, 2585, 2576, 2568, 2560, 2551, 2543, 2535, 2526, 2518, 2509, 2501,
		2492, 2483, 2475, 2466, 2458, 2449, 2440, 2431, 2423, 2414, 2405, 2396, 2387, 2378, 2369, 2360, 2351,
		2342, 2333, 2324, 2315, 2305, 2296, 2287, 2278, 2268, 2259, 2250, 2240, 2231, 2222, 2212, 2203, 2193,
		2184, 2174, 2164, 2155, 2145, 2136, 2126, 2116, 2106, 2097, 2087, 2077, 2067, 2057, 2047, 2037, 2027,
		2018, 2008, 1997, 1987, 1977, 1967, 1957, 1947, 1937, 1927, 1916, 1906, 1896, 1886, 1875, 1865, 1855,
		1844, 1834, 1824, 1813, 1803, 1792, 1782, 1771, 1761, 1750, 1739, 1729, 1718, 1708, 1697, 1686, 1675,
		1665, 1654, 1643, 1632, 1622, 1611, 1600, 1589, 1578, 1567, 1556, 1545, 1534, 1523, 1512, 1501, 1490,
		1479, 1468, 1457, 1446, 1435, 1424, 1412, 1401, 1390, 1379, 1368, 1356, 1345, 1334, 1322, 1311, 1300,
		1288, 1277, 1266, 1254, 1243, 1231, 1220, 1209, 1197, 1186, 1174, 1163, 1151, 1139, 1128, 1116, 1105,
		1093, 1082, 1070, 1058, 1047, 1035, 1023, 1012, 1000, 988, 976, 965, 953, 941, 929, 918, 906, 894, 882,
		870, 859, 847, 835, 823, 811, 799, 787, 775, 764, 752, 740, 728, 716, 704, 692, 680, 668, 656, 644, 632,
		620, 608, 596, 584, 572, 560, 548, 535, 523, 511, 499, 487, 475, 463, 451, 439, 427, 414, 402, 390, 378,
		366, 354, 342, 329, 317, 305, 293, 281, 269, 256, 244, 232, 220, 208, 195, 183, 171, 159, 147, 134, 122,
		110, 98, 86, 73, 61, 49, 37, 24, 12, 0, 0, 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0};

uint16_t ADC_V_op_data=0;
uint16_t ADC_V_data=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void SetMaxVolume(void)
{
	UseVolume=1;
	Volume_Coeff=VOLUME_MAX;
}

void PlusVolume(void)
{
	UseVolume=1;
	Volume_Coeff=Volume_Coeff+1;
	if (Volume_Coeff>VOLUME_MAX) {
		Volume_Coeff=VOLUME_MAX;
	}
}

void MinusVolume(void)
{
	if (UseVolume==1) {
		if (Volume_Coeff==0) {
			UseVolume=0;
		} else {
			Volume_Coeff--;
		}
	}
}

void CheckSoftStart(void)
{
	if (UseSoftStart==1) {
			SoftStart_Coeff=SoftStart_Coeff+SOFTSTART_STEP;
			if (SoftStart_Coeff>=SOFTSTART_STEP_MAX) {
				UseSoftStart=0;
				SoftStart_Coeff=0;
			}
	}
}

void DAC_V(void)
{
	if (Sin_Step>=DAC_PULSE_CNT) {
		Sin_Step=1;
	}

	tmp_orig=Sin_Etalon[Sin_Step-1];
	tmp_etalon=tmp_orig;
	tmp_etalon_amplitude=MAX_SIN_TABLE_VALUE;

	if (UseSoftStart==1) {
		tmp_etalon=(tmp_etalon*SoftStart_Coeff)/SOFTSTART_STEP_MAX;
		tmp_etalon_amplitude=(tmp_etalon_amplitude*SoftStart_Coeff)/SOFTSTART_STEP_MAX;
	}

	if (UseVolume==1) {
		tmp_etalon=(tmp_etalon*(Volume_Coeff+VOLUME_MAX))/VOLUME_MAX;
		if (tmp_etalon>=MAX_SIN_TABLE_VALUE) {
			tmp_etalon=MAX_SIN_TABLE_VALUE;
		}

		tmp_etalon_amplitude=(tmp_etalon_amplitude*(Volume_Coeff+VOLUME_MAX))/VOLUME_MAX;
	}

	if ((UseSoftStart==1) || (UseVolume==1)) {
		if (tmp_etalon_amplitude>=MAX_SIN_TABLE_VALUE) {
			tmp_etalon_amplitude=MAX_SIN_TABLE_VALUE;
		}
	}


	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_R,tmp_etalon);
}

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
			| HRTIM_TIMERID_TIMER_E | HRTIM_TIMERID_TIMER_C );

	IGLAflagRESET(); // off IGLA

	Sin_Step=0;
	DAC_V();

	UseSoftStart=0;
}

void HZ50_Start(void)
{
	  Sin_Step=0;
	  DAC_V();

	  HZ50_State=DeadZoneBA; // curr zone

	  HAL_HRTIM_RepetitionEventCallback(&hhrtim1,HRTIM_TIMERINDEX_MASTER);

	  HAL_HRTIM_WaveformCountStart_IT(&hhrtim1, HRTIM_TIMERID_MASTER | HRTIM_TIMERID_TIMER_B
			  | HRTIM_TIMERID_TIMER_E | HRTIM_TIMERID_TIMER_C);

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

void HAL_HRTIM_Compare1EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	//if (TimerIdx==HRTIM_TIMERINDEX_TIMER_C) {
		Sin_Step++;
		DAC_V();
	//}

	ADC_V_op_data=HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
	ADC_V_data=HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);

	tmp_pulse=(uint32_t)((tmp_orig*PULSE_LENGTH)/MAX_SIN_TABLE_VALUE); // max pulse

	if (UseSoftStart==1) {
		tmp_pulse=(uint32_t)((tmp_pulse*SoftStart_Coeff)/SOFTSTART_STEP_MAX);
	}

	if (UseVolume==1) {
		tmp_pulse=(uint32_t)((tmp_pulse*(Volume_Coeff+VOLUME_MAX))/VOLUME_MAX);
	}

	//ADC
	/*if (ADC_V_op_data>500) {
		cur_reg_step=cur_reg_step+reg_step;
	} else {
		if (cur_reg_step>=reg_step) {
			cur_reg_step=cur_reg_step-reg_step;
		}
	}*/

	if ((ADC_V_op_data>0) && (ADC_V_data>tmp_orig)) {
			tmp_pulse=(uint32_t)((tmp_pulse*ADC_V_op_data)/4096);
	}

	/*if ((ADC_V_op_data>=4095) && (ADC_V_data>tmp_orig) && (tmp_orig!=0)) {
		tmp_pulse=(uint32_t)((tmp_pulse*tmp_orig)/ADC_V_data);
	}*/

	if (tmp_pulse>=PULSE_LENGTH) {
		tmp_pulse=PULSE_LENGTH;
	}

	if ((tmp_pulse<=PULSE_DEATHTIME) || (tmp_orig==0)) {
		tmp_pulse=0;
	}



	/*if (ADC_V_op_data>=100) {
		//tmp_pulse=(uint32_t)((tmp_etalon*PULSE_LENGTH)/tmp_etalon_amplitude);
		tmp_pulse=PULSE_LENGTH;
		tmp_pulse=(tmp_pulse*ADC_V_data)/(1000);
	} else {
		tmp_pulse=PULSE_LENGTH;
	}*/

	HRTIM1->sTimerxRegs[1].CMP1xR=PULSE_DEATHTIME+tmp_pulse;
	HRTIM1->sTimerxRegs[1].CMP2xR=PULSE_TOTAL_LENGTH+HRTIM1->sTimerxRegs[1].CMP1xR;

	//HRTIM1->sTimerxRegs[1].CMP1xR=PULSE_DEATHTIME+(uint32_t)((tmp_etalon*PULSE_LENGTH)/tmp_etalon_amplitude);
	//HRTIM1->sTimerxRegs[1].CMP2xR=PULSE_TOTAL_LENGTH+HRTIM1->sTimerxRegs[1].CMP1xR;
}

void HAL_HRTIM_Compare2EventCallback(HRTIM_HandleTypeDef *hhrtim,
                                            uint32_t TimerIdx)
{
	//if (TimerIdx==HRTIM_TIMERINDEX_TIMER_C) {
		Sin_Step++;
		DAC_V();
	//}

	//ADC_V_op_data=HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
	//ADC_V_data=HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
}

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc)
{
	IGLAflagSET(); // off IGLA
}


void HAL_HRTIM_RepetitionEventCallback(HRTIM_HandleTypeDef *hhrtim,
                                              uint32_t TimerIdx)
{
	if (TimerIdx==HRTIM_TIMERINDEX_MASTER) {
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

					Sin_Step=0;
					CheckSoftStart();
					MinusVolume();
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

					Sin_Step=0;
					//CheckSoftStart();
					break;
			}
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
  MX_DMA_Init();
  MX_HRTIM1_Init();
  MX_DAC1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);
  UseSoftStart=1;
  //SetMaxVolume();

  HAL_ADCEx_InjectedStart(&hadc1);

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_HRTIM1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
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
