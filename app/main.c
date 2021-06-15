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

uint8_t RxByte;
uint8_t RxBuffer[20];
uint8_t RxBufferSPI[2], TxBufferSPI[4];


#define CS      GPIO_PIN_10
#define READ    3U
#define WRITE   2U
#define WRDI    4U
#define WREN    6U
#define RDSR    5U
#define WRSR    1U



void UART_Init(void); 
void SPI_Init(void);

void write_byte(uint16_t addr, uint8_t data);
uint8_t read_byte(uint16_t addr);

void write_data(uint16_t addr, uint8_t *data, uint8_t size);
void read_data(uint16_t addr, uint8_t *data, uint8_t size);

int main( void )
{
    HAL_Init( );
    UART_Init();
    SPI_Init();

    // write_byte(0,0x1F);
    
    // HAL_GPIO_WritePin(GPIOC,0xFF,RESET);
    // HAL_GPIO_WritePin(GPIOC,read_byte(0),SET);
    
    write_data(30,(uint8_t*)"Hola mundo y adios\n",strlen("Hola mundo y adios\n"));

    read_data(25,RxBuffer,19);

    HAL_UART_Transmit_IT(&UartHandle,RxBuffer,strlen((const char*)RxBuffer));
    for (; ;)
    {

    } 
    return 0u;
}


void UART_Init()
{
    UartHandle.Instance             = USART2;
    UartHandle.Init.BaudRate        = 9600;
    UartHandle.Init.WordLength      = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits        = UART_STOPBITS_1;
    UartHandle.Init.Parity          = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl       = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode            = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling    = UART_OVERSAMPLING_16;

    HAL_UART_Init(&UartHandle);
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void SPI_Init(void)
{
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
}

void write_byte(uint16_t addr, uint8_t data)
{
    uint8_t Tx_B_SPI[4] = {0};
    uint8_t Rx_SPI = 0;

    //Habilita la escritura
    HAL_GPIO_WritePin(GPIOB,CS,RESET);
    Tx_B_SPI[0] = WREN;
    HAL_SPI_Transmit_IT(&SpiHandle,Tx_B_SPI,1);
    HAL_GPIO_WritePin(GPIOB,CS,SET);    

    //Manda la intruccion, direccion y dato
    HAL_GPIO_WritePin(GPIOB,CS,RESET);
    Tx_B_SPI[0] = WRITE; 
    Tx_B_SPI[1] = (uint8_t)(addr>>8);
    Tx_B_SPI[2] = (uint8_t)(addr);
    Tx_B_SPI[3] = data;
    HAL_SPI_Transmit_IT(&SpiHandle,Tx_B_SPI,4);
    HAL_GPIO_WritePin(GPIOB,CS,SET);

    // Pregunta si ya termino de escribir en memoria
    do
    {
        HAL_GPIO_WritePin(GPIOB,CS,RESET);
        Tx_B_SPI[0] = RDSR;
        HAL_SPI_Transmit_IT(&SpiHandle,Tx_B_SPI,1);
        HAL_SPI_Receive(&SpiHandle,&Rx_SPI,1,1000);
        HAL_GPIO_WritePin(GPIOB,CS,SET); 
    }while(Rx_SPI == 3);

    // HAL_Delay(10);
}

uint8_t read_byte(uint16_t addr)
{
    uint8_t Rx_b_SPI[4] = {0};
    uint8_t Rx__SPI[2] = {0};
    HAL_GPIO_WritePin(GPIOB,CS,RESET);
    Rx_b_SPI[0] = 0x03; 
    Rx_b_SPI[1] = (uint8_t)(addr>>8);
    Rx_b_SPI[2] = (uint8_t)(addr);
    HAL_SPI_Transmit_IT(&SpiHandle,Rx_b_SPI,3);
    HAL_SPI_Receive(&SpiHandle,Rx__SPI,1,5000);
    HAL_GPIO_WritePin(GPIOB,CS,SET);

    return Rx__SPI[0];
}

void write_data(uint16_t addr, uint8_t *data, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        write_byte(addr+i,data[i]);
    }
    
}

void read_data(uint16_t addr, uint8_t *data, uint8_t size)
{
    for (uint8_t i = 0; i < size; i++)
    {
        data[i] = read_byte(addr+i);
    }
    
}



void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uartState = SET;
    memset(RxBuffer,0,sizeof(RxBuffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint32_t i = 0;
    RxBuffer[i] = RxByte;
    i++;
    if(RxBuffer[i-1] == '\r')
    {
        status = SET;
        i=0;
    }
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}