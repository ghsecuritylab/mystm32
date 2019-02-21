#include "stm32f1xx_hal.h"

/* Private typedef ----------------------------------------------------------- */
#define  PERIOD_VALUE       (uint32_t)(36 - 1)  /* Period Value  */
#define  PULSE1_VALUE       (uint32_t)(PERIOD_VALUE/2)        /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       (uint32_t)(PERIOD_VALUE*37.5/100) /* Capture Compare 2 Value  */
#define  PULSE3_VALUE       (uint32_t)(PERIOD_VALUE/4)        /* Capture Compare 3 Value  */
#define  PULSE4_VALUE       (uint32_t)(PERIOD_VALUE*12.5/100) /* Capture Compare 4 Value  */

/* Private define ------------------------------------------------------------ */
/* User can use this section to tailor TIMx instance used and associated
   resources */
/* Definition for TIMx clock resources */
#define TIMx                           TIM2
#define TIMx_CLK_ENABLE()              __HAL_RCC_TIM2_CLK_ENABLE()

/* Definition for TIMx Channel Pins */
#define TIMx_CHANNEL_GPIO_PORT()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define TIMx_GPIO_PORT_CHANNEL1        GPIOA
#define TIMx_GPIO_PORT_CHANNEL2        GPIOA
#define TIMx_GPIO_PORT_CHANNEL3        GPIOA
#define TIMx_GPIO_PORT_CHANNEL4        GPIOA
#define TIMx_GPIO_PIN_CHANNEL1         GPIO_PIN_0
#define TIMx_GPIO_PIN_CHANNEL2         GPIO_PIN_1
#define TIMx_GPIO_PIN_CHANNEL3         GPIO_PIN_2
#define TIMx_GPIO_PIN_CHANNEL4         GPIO_PIN_3
#define TIMx_GPIO_AF_CHANNEL1          /
#define TIMx_GPIO_AF_CHANNEL2          /
#define TIMx_GPIO_AF_CHANNEL3          /
#define TIMx_GPIO_AF_CHANNEL4          /

#define PULSE_CHANNEL1		CCR1
#define PULSE_CHANNEL2		CCR2
#define PULSE_CHANNEL3		CCR3
#define PULSE_CHANNEL4		CCR4

void duty_cycle(uint16_t pulse);
void PWM_BREATH_EXAMPLE(void);
void tone(uint16_t Hz);
void notone(void);
void PWM_MELODY_EXAMPLE(void);
