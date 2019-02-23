#include "f103_dac.h"

DAC_HandleTypeDef    DacHandle;
static DAC_ChannelConfTypeDef sConfig;

void Error_Handler(uint8_t id);
void DAC_CONFIG(void)
{
  /*##-1- Configure the DAC peripheral #######################################*/
  DacHandle.Instance = DACx;

  if (HAL_DAC_Init(&DacHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(0);
  }

  /*##-2- Configure DAC channel1 #############################################*/
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;

  if (HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DACx_CHANNEL) != HAL_OK)
  {
    /* Channel configuration Error */
    Error_Handler(1);
  }

  /*##-3- Set DAC Channel1 DHR register ######################################*/
  if (HAL_DAC_SetValue(&DacHandle, DACx_CHANNEL, DAC_ALIGN_8B_R, 0xFF) != HAL_OK)
  {
    /* Setting value Error */
    Error_Handler(2);
  }

  /*##-4- Enable DAC Channel1 ################################################*/
  if (HAL_DAC_Start(&DacHandle, DACx_CHANNEL) != HAL_OK)
  {
    /* Start Error */
    Error_Handler(3);
  }

}


/**
  * @brief DAC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hdac: DAC handle pointer
  * @retval None
  */
void HAL_DAC_MspInit(DAC_HandleTypeDef *hdac)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock ****************************************/
  DACx_CHANNEL_GPIO_CLK_ENABLE();
  /* DAC Periph clock enable */
  DACx_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* DAC Channel1 GPIO pin configuration */
  GPIO_InitStruct.Pin = DACx_CHANNEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DACx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
}

/**
  * @brief  DeInitializes the DAC MSP.
  * @param  hdac: pointer to a DAC_HandleTypeDef structure that contains
  *         the configuration information for the specified DAC.
  * @retval None
  */
void HAL_DAC_MspDeInit(DAC_HandleTypeDef *hdac)
{
  /* Enable DAC reset state */
  DACx_FORCE_RESET();

  /* Release DAC from reset state */
  DACx_RELEASE_RESET();
}
