#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include <stdint.h>
#include "app_bsp.h"
#include "stm32f0xx_hal_conf.h"
#include "stm32f070xb.h"

void HAL_MspInit( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStructure); 

    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3; 
    GPIO_InitStructure.Pin += GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin  =  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStructure.Pin  +=  GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
    
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin  = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.Mode     = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull     = GPIO_NOPULL;
    GPIO_InitStructure.Speed    = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF1_USART2;

    HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
    HAL_NVIC_SetPriority(USART2_IRQn,1,0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}
