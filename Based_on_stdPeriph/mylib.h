#include <stm32f10x.h>
#include <stdint.h>
void display(uint16_t number);
void displayEX(uint16_t number);
void displayStr(uint8_t c1,uint8_t c2,uint8_t c3,uint8_t c4);
void displayStrEx(uint8_t c1,uint8_t c2,uint8_t c3,uint8_t c4);
//void display_dot(uint8_t pos);
void displayEX_dot(uint8_t pos);
void display_real(double real);
void DISPLAY_CONFIG(void);
void KEYBOARD_CONFIG(void);
uint8_t getKey(void);
void waitKeyUp(uint8_t keyId);
void NVIC_CONFIG(uint8_t IRQChannel, uint8_t PreemptionPriority, uint8_t SubPriority);
void USART_INT_CONFIG(void);
void USART_CONFIG(void);
uint8_t I2C_EE_ByteWrite(uint8_t c, uint8_t addr);
uint8_t I2C_EE_CheckDevice(void);
uint8_t I2C_EE_BufferWrite(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
uint8_t I2C_EE_BufferRead(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint8_t i2c_ee_ByteWrite(uint8_t c, uint8_t addr);
uint8_t i2c_ee_CheckDevice(void);
uint8_t i2c_ee_ReadBytes(uint8_t *pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint8_t i2c_ee_WriteBytes(uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_Init(void);
void SPI_FLASH_WakeUp(void);
void SPI_FLASH_PowerDown(void);
uint32_t SPI_FLASH_ReadDeviceID(void);
uint32_t SPI_FLASH_ReadID(void);
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_SectorErase(uint32_t	SectorAddr);
void SPI_OLED_Init(void);
void OLED_Fill(uint8_t fill_Data);
void OLED_ShowStr(uint8_t x, uint8_t y, const char str[], uint8_t TextSize);
void ADC_SINGLE_CONFIG(void);
void DMA_ADC_Config(ADC_TypeDef *ADCX, uint32_t DMA_, DMA_Channel_TypeDef *DMAX_ChannelX, uint16_t dest[], uint32_t ArrayLength);
void ADC_MULTI_CONFIG(void);
void Temp_ADC1_Init(void);
float Temp_ADC1_Get(void);
void TIM_BASE_CONFIG(void);
void TIM_PWMOUT_CONFIG(void);
uint8_t TPAD_Init(void);
uint8_t TPAD_Scan(void);
void tone(uint16_t Hz);
void notone(void);
void CH1_DutyCycle(uint16_t pulse);
void CH2_DutyCycle(uint16_t pulse);
void CH3_DutyCycle(uint16_t pulse);
void CH4_DutyCycle(uint16_t pulse);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void LCD1602_CONFIG(void);
void LCD_WriteString(uint8_t line, uint8_t x, char* c);
void LCD_WriteInteger(uint8_t line, uint8_t x, uint32_t c);
void LCD_WriteFloat(uint8_t line, uint8_t x, float f);
