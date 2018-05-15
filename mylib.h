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
uint8_t getKey_nonBlock(void);
void NVIC_CONFIG(uint8_t IRQChannel, uint8_t Priority);
void EXTI_CONFIG(void);
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
void SPI_CONFIG(void);
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
void duty_cycle(uint16_t pulse);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
void LCD1602_CONFIG(void);
void LCD_WriteString(uint8_t line, uint8_t x, char* c);
void LCD_WriteInteger(uint8_t line, uint8_t x, uint32_t c);
void LCD_WriteFloat(uint8_t line, uint8_t x, float f);
