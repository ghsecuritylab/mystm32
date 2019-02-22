#include "f103_pwm_out.h"
void duty_cycle(uint16_t pulse)
{
	// 0 <= pulse <= TIMX_PERIOD-1
	TIMx->PULSE_CHANNEL1 = pulse;	// TIM_SetCompareX(TIMX, pulse)
}
void PWM_BREATH_EXAMPLE(void)
{
	PWM_CONFIG();
	while (1)
	{
		for(int32_t i = 0; i < PERIOD_VALUE; i++) {
			duty_cycle(i);
			HAL_Delay(20);
		}
		for(int32_t i = PERIOD_VALUE-1; i >= 0; i--) {
			duty_cycle(i);
			HAL_Delay(20);
		}
	}
}

const uint16_t notes_c4[] = {264,292,330,352,396,440,495,523};
const uint16_t notes_c5[] = {523,587,659,698,784,880,988,1046};
const uint16_t notes_c6[] = {1046,1175,1319,1397,1568,1760,1976,2093};
const uint16_t notes_c7[] = {2093,2349,2637,2794,3136,3520,3951,4186};
void tone(uint16_t Hz)
{
	// Hz = 72000000 / PERIOD / PSC
	TIMx->PULSE_CHANNEL1 = PERIOD_VALUE/2;	// 比较寄存器 TIM_SetCompareX(TIMX, pulse)
	TIMx->ARR = PERIOD_VALUE-1;		// 自动重装值 TIM_SetAutoreload(TIMX, TIMX_PERIOD-1);
	TIMx->PSC = (SystemCoreClock / PERIOD_VALUE) / Hz - 1;	// 预分频
}
void notone(void)
{
	// 将PWM作为输入引脚时通电逻辑相反，置为全高即可
	TIMx->PULSE_CHANNEL1 = PERIOD_VALUE;
}
void PWM_MELODY_EXAMPLE(void)
{
	PWM_CONFIG();
	while (1)
	{
		for(int8_t i = 0; i < sizeof(notes_c6)/sizeof(uint16_t); i++) {
			tone(notes_c7[i]);
			HAL_Delay(400);
			notone();
			HAL_Delay(200);
		}
	}
}


/* Timer handler declaration */
TIM_HandleTypeDef    TimHandle;

/* Timer Output Compare Configuration Structure declaration */
TIM_OC_InitTypeDef sConfig;

/* Counter Prescaler value */
uint32_t uhPrescalerValue = 0;

void Error_Handler(uint8_t id);
void PWM_CONFIG(void)
{

  /* Compute the prescaler value to have TIM2 counter clock equal to 2000000 Hz */
  uhPrescalerValue = (uint32_t)(SystemCoreClock / 2000000) - 1;

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
  TIM2 Configuration: generate 4 PWM signals with 4 different duty cycles.

    In this example TIM2 input clock (TIM2CLK) is set to APB1 clock (PCLK1) x2,
    since APB1 prescaler is set to 4 (0x100).
       TIM2CLK = PCLK1*2
       PCLK1   = HCLK/2
    => TIM2CLK = PCLK1*2 = (HCLK/2)*2 = HCLK = SystemCoreClock

    To get TIM2 counter clock at 2.1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM2CLK / TIM2 counter clock) - 1
       Prescaler = ((SystemCoreClock) /2.1 MHz) - 1

    To get TIM2 output clock at 3 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM2 counter clock / TIM2 output clock) - 1
           = 699

    TIM2 Channel1 duty cycle = (TIM2_CCR1/ TIM2_ARR + 1)* 100 = 50%
    TIM2 Channel2 duty cycle = (TIM2_CCR2/ TIM2_ARR + 1)* 100 = 37.5%
    TIM2 Channel3 duty cycle = (TIM2_CCR3/ TIM2_ARR + 1)* 100 = 25%
    TIM2 Channel4 duty cycle = (TIM2_CCR4/ TIM2_ARR + 1)* 100 = 12.5%

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f1xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Initialize TIMx peripheral as follows:
       + Prescaler = (SystemCoreClock / 2000000) - 1
       + Period = (700 - 1)
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Instance = TIMx;

  TimHandle.Init.Prescaler         = uhPrescalerValue;
  TimHandle.Init.Period            = PERIOD_VALUE;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(1);
  }

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for all channels */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse = PULSE1_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler(2);
  }
#if 0
  /* Set the pulse value for channel 2 */
  sConfig.Pulse = PULSE2_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler(3);
  }

  /* Set the pulse value for channel 3 */
  sConfig.Pulse = PULSE3_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler(4);
  }

  /* Set the pulse value for channel 4 */
  sConfig.Pulse = PULSE4_VALUE;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler(5);
  }
#endif
  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler(6);
  }
#if 0
  /* Start channel 2 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2) != HAL_OK)
  {
    /* PWM Generation Error */
    Error_Handler(7);
  }
  /* Start channel 3 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3) != HAL_OK)
  {
    /* PWM generation Error */
    Error_Handler(8);
  }
  /* Start channel 4 */
  if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4) != HAL_OK)
  {
    /* PWM generation Error */
    Error_Handler(9);
  }
#endif
}

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */
  TIMx_CLK_ENABLE();

  /* Enable all GPIO Channels Clock requested */
  TIMx_CHANNEL_GPIO_PORT();

  /* Configure PA.0  (On Eval Board, pin 31 on CN1  for example) (TIM2_Channel1),
	 PA.1  (On Eval Board, pin 33 on CN1  for example) (TIM2_Channel2),
     PA.2  (On Eval Board, pin 34 on CN1  for example) (TIM2_Channel3),
     PA.3  (On Eval Board, pin 58 on CN11 for example) (TIM2_Channel4) in output,
     push-pull, alternate function mode
  */
  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL1;
  HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL1, &GPIO_InitStruct);

//  GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL2;
//  HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL2, &GPIO_InitStruct);

//  GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL3;
//  HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL3, &GPIO_InitStruct);

//  GPIO_InitStruct.Pin = TIMx_GPIO_PIN_CHANNEL4;
//  HAL_GPIO_Init(TIMx_GPIO_PORT_CHANNEL4, &GPIO_InitStruct);
}

