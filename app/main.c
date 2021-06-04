#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "app_bsp.h"
#include "stm32f070xb.h"
#include "stm32f0xx_hal_conf.h"
#include "string.h"
#include "stm32f0xx_hal.h"
#include "funciones.h"
#include <ctype.h>
/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

UART_HandleTypeDef UartHandle;
SPI_HandleTypeDef SpiHandle;

__IO ITStatus uartState = RESET;
__IO ITStatus status = RESET;

extern uint8_t RxBuffer[20];
uint8_t RxBufferSPI[2], TxBufferSPI[4];
#define CS  GPIO_PIN_10

int main( void )
{
    HAL_Init( );
    UART_Init();

    SpiHandle.Instance                  = SPI1;
    SpiHandle.Init.Mode                 = SPI_MODE_MASTER;
    SpiHandle.Init.BaudRatePrescaler    = SPI_BAUDRATEPRESCALER_4;
    SpiHandle.Init.Direction            = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase             = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity          = SPI_POLARITY_HIGH;
    SpiHandle.Init.CRCCalculation       = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial        = 7;
    SpiHandle.Init.DataSize             = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit             = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS                  = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode               = SPI_TIMODE_DISABLE;

    HAL_GPIO_WritePin(GPIOB,CS,SET);
    HAL_SPI_Init(&SpiHandle);
    
    HAL_GPIO_WritePin(GPIOB,CS,RESET);
    TxBufferSPI[0] = 0x06;
    HAL_SPI_Transmit(&SpiHandle,TxBufferSPI,1,5000);
    HAL_GPIO_WritePin(GPIOB,CS,SET);

    HAL_GPIO_WritePin(GPIOB,CS,RESET);
    TxBufferSPI[0] = 0x02; 
    TxBufferSPI[1] = 0x00;
    TxBufferSPI[2] = 0x00;
    TxBufferSPI[3] = 0xf0;
    HAL_SPI_Transmit(&SpiHandle,TxBufferSPI,4,5000);
    HAL_GPIO_WritePin(GPIOB,CS,SET);

    HAL_Delay(10);

    HAL_GPIO_WritePin(GPIOB,CS,RESET);
    TxBufferSPI[0] = 0x03; 
    TxBufferSPI[1] = 0x00;
    TxBufferSPI[2] = 0x00;
    HAL_SPI_Transmit(&SpiHandle,TxBufferSPI,3,5000);
    HAL_SPI_Receive(&SpiHandle,RxBufferSPI,1,5000);
    HAL_GPIO_WritePin(GPIOB,CS,SET);

    HAL_GPIO_WritePin(GPIOC,0xFF,RESET);
    HAL_GPIO_WritePin(GPIOC,RxBufferSPI[0],SET);

    for (; ;)
    {

    } 
    return 0u;
}


