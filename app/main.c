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
__IO ITStatus uartState = RESET;
__IO ITStatus status = RESET;
extern uint8_t RxBuffer[20];


int main( void )
{
    HAL_Init( );
    UART_Init();

    for (; ;)
    {
        if (status == SET)
        {
            status = RESET;
            if (isdigit(RxBuffer[0]))
            {
                HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_0);
                HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)"numero\r\n",sizeof("numero\r\n")-1);
            }
            else
            {
                HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1);  
                HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)"caracter\r\n",sizeof("caracter\r\n")-1); 
                
            }
        }
        
    } 
    return 0u;
}


