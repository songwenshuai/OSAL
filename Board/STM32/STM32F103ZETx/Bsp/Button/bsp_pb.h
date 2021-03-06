/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_PB_H
#define __BSP_PB_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/**
  * @}
  */

/** @defgroup BSP BSP Nucleo 144
  * @{
  */

typedef enum
{
  BUTTON_A = 0,
  BUTTON_B = 1,
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;

/**
  * @}
  */

/** @defgroup BUTTON
  * @{
  */
#define BUTTONn                                 2

/**
 * @brief Key push-button
 */
#define USER_BUTTON_A_PIN                          GPIO_PIN_0
#define USER_BUTTON_A_GPIO_PORT                    GPIOA
#define USER_BUTTON_A_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
#define USER_BUTTON_A_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()
#define USER_BUTTON_A_EXTI_IRQn                    EXTI0_IRQn

#define USER_BUTTON_B_PIN                          GPIO_PIN_13
#define USER_BUTTON_B_GPIO_PORT                    GPIOC
#define USER_BUTTON_B_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define USER_BUTTON_B_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()
#define USER_BUTTON_B_EXTI_IRQn                    EXTI15_10_IRQn

/**
  * @}
  */

/** @defgroup Exported Functions
  * @{
  */
void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_PB_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
