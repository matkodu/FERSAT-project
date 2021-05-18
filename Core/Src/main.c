/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RNG_HandleTypeDef hrng;

TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RNG_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// Globalne varijable

#define BUFF_L 64

uint32_t RandomNum;
uint32_t bin_test;
uint32_t p_index = 0;
int i,br=0, bin_buff[32]; 
int I_buff[BUFF_L];
int Q_buff[BUFF_L];   // Bit array - mijenjati njihovu duljinu po potrebi 
																	// PRBS(11) - 2048
																	// PRBS(9) - 512
																	// PRBS(7) - 128
																	// TrueRNG - nije bitna duljina


/*---------------------------------------------- Generator pravih slucajnih bitova --------------------------------------------------- */
// Koristena RNG konfiguracija
// poziv funkcije: TrueRNG();

void TrueRNG(void)
{
	
	HAL_RNG_GenerateRandomNumber(&hrng, &RandomNum);
	
	
	
	bin_test = RandomNum;
	for (i=31;i>=0;i--)
		{
			if(bin_test & 1)
				{
					bin_buff[i] = 1;
	        bin_test >>= 1;
	      }
	      else
	      {
					bin_buff[i] = 0;
	        bin_test >>= 1;
	      }
	  }
	for(i=0;i<32;i=i+2)
	 {
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 0;
	                br++;
	                
	            }
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 1;
	                br++;
	                
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 0;
	                br++;
	                
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 1;
	                br++;
	               
	            }
	            
	            
	  }
	
	
//  printf("Generiran = %08X\n", RandomNum);
}

/*------------------------------------------------------ PRBS 7 --------------------------------------------------------------- */
// x^7 + x^6 + 1
// poziv funkcije: PRBS_7();

uint32_t get_bit_7 (uint32_t pp)
{
	uint32_t p_v1;
	uint32_t p_v2;

	if (pp & (0x1<<6))
	{
		p_v1 = 1;
	}
	else
	{
		p_v1 = 0;
	}

	if (pp & (0x1<<5))
	{
		p_v2 = 1;
	}
	else
	{
		p_v2 = 0;
	}
	return (p_v1 ^ p_v2);
}

void PRBS_7 (void) 
{
    
	uint32_t seed = 0x7f;
	uint32_t move_time = 7;

	while (1)
	{
		if (get_bit_7(seed))
		{
			seed <<=1;
			seed += 1;
			move_time++;
		}
		else
		{
			seed <<=1;
			move_time++;
		}
		p_index++;

		if ((seed & 0x7f) == 0x7f)
		{
			break;
		}

		if (move_time == 32)
		{
		    bin_test = seed;
            for (i=31;i>=0;i--)
	        {
	            if(bin_test & 1)
	            {
	                bin_buff[i] = 1;
	                bin_test >>= 1;
	            }
	            else
	            {
	                bin_buff[i] = 0;
	                bin_test >>= 1;
	            }
	    
	        }
	        
	        for(i=0;i<32;i=i+2)
	        {
	            
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 0;
	                br++;
	                
	            }
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 1;
	                br++;
	                
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 0;
	                br++;
	                
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 1;
	                br++;
	               
	            }
	            
	        }
			//printf ("%08X\n", seed);
			move_time =0;
		}
	}

	//printf ("\nPeriod ponavljanja je %u\n", p_index);
	
	
	
}
/*----------------------------------------------------------- PRBS 9 ------------------------------------------------------------- */
// x^9 + x^5 + 1 
// poziv funkcije: PRBS_9();


uint32_t get_bit_9 (uint32_t pp)
{
	uint32_t p_v1;
	uint32_t p_v2;

	if (pp & (0x1<<8))
	{
		p_v1 = 1;
	}
	else
	{
		p_v1 = 0;
	}

	if (pp & (0x1<<4))
	{
		p_v2 = 1;
	}
	else
	{
		p_v2 = 0;
	}
	return (p_v1 ^ p_v2);
}

void PRBS_9 (void)
{
	uint32_t seed = 0x1ff;
	uint32_t move_time = 9;

	while (1)
	{
		if (get_bit_9(seed))
		{
			seed <<=1;
			seed += 1;
			move_time++;
		}
		else
		{
			seed <<=1;
			move_time++;
		}
		p_index ++;

		if ((seed & 0x1ff) == 0x1ff)
		{
			break;
		}

		if (move_time == 32)
		{
		    bin_test = seed;
            for (i=31;i>=0;i--)
	        {
	            if(bin_test & 1)
	            {
	                bin_buff[i] = 1;
	                bin_test >>= 1;
	            }
	            else
	            {
	                bin_buff[i] = 0;
	                bin_test >>= 1;
	            }
	    
	        }
	        
	        for(i=0;i<32;i=i+2)
	        {
	            
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 0;
	                br++;
	            }
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 1;
	                br++;
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 0;
	                br++;
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 1;
	                br++;
	            }
	            
	        }
			//printf ("%08X\n", seed);
			move_time =0;
		}
	}

	//printf ("\nPeriod ponavljanja je %u\n", p_index);
	

}

/*-------------------------------------------------- PRBS 11 ------------------------------------------------------*/
// x^11 + x^9 + 1
// poziv funkcije: PRBS11();

uint32_t get_bit_11 (uint32_t pp)
{
	uint32_t p_v1;
	uint32_t p_v2;

	if (pp & (0x1<<10))
	{
		p_v1 = 1;
	}
	else
	{
		p_v1 = 0;
	}

	if (pp & (0x1<<8))
	{
		p_v2 = 1;
	}
	else
	{
		p_v2 = 0;
	}
	return (p_v1 ^ p_v2);
}

void PRBS_11 (void)
{
	uint32_t  seed = 0x7ff;
	uint32_t  move_time = 11;

	while (1)
	{
		if (get_bit_11(seed))
		{
			seed <<=1;
			seed += 1;
			move_time++;
		}
		else
		{
			seed <<=1;
			move_time++;
		}
		p_index ++;

		if ((seed & 0x7ff) == 0x7ff)
		{
			break;
		}

		if (move_time == 32)
		{
		    bin_test = seed;
            for (i=31;i>=0;i--)
	        {
	            if(bin_test & 1)
	            {
	                bin_buff[i] = 1;
	                bin_test >>= 1;
	            }
	            else
	            {
	                bin_buff[i] = 0;
	                bin_test >>= 1;
	            }
	    
	        }
	        
	        for(i=0;i<32;i=i+2)
	        {
	            
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 0;
								  
	                br++; 
	            }
	            if((bin_buff[i] == 0) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 0;
	                Q_buff[br] = 1;
								  
	                br++;
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 0))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 0;
								  
	                br++;
	            }
	            if((bin_buff[i] == 1) && (bin_buff[i+1] == 1))
	            {
	                I_buff[br] = 1;
	                Q_buff[br] = 1;
								  
	                br++;
	            }
	            
	        }
			//printf ("%08X\n", seed);
			move_time =0;
		}

	}

	//printf ("\nPeriod ponavljanja je %u\n", p_index);
	
	
	
}





/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//unsigned int flag = 0;
	
	PRBS_7();  // poziv funkcije
	
	
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
  MX_RNG_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_TIM_Base_Start_IT(&htim4);
	//HAL_TIM_OnePulse_Start_IT(&htim4, TIM_CHANNEL_3);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV8;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_1);
}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 4;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA4 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
