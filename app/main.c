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

const uint8_t *mensaje = {(uint8_t*)"Hola mundo en mi eeprom"};
const uint8_t* msgOk   =    (uint8_t*)"OK\n";
const uint8_t *comando = {(uint8_t*)"WRITE"};

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

HAL_StatusTypeDef correctComand(uint8_t * buffer, uint16_t * addr, uint8_t* byte);

uint32_t tickTimer = 0;
uint16_t addr = 0;
uint8_t byte = 0;
int main( void )
{
    HAL_Init( );
    UART_Init();
    SPI_Init();
    tickTimer = HAL_GetTick();
    for (; ;)
    {

        if (status == SET)
        {
            status = RESET;
            if(correctComand(RxBuffer,&addr,&byte) == HAL_OK)
            {
                write_byte(addr,byte);
                TxBufferSPI[0] = read_byte(addr);
                strcat((char*)TxBufferSPI,"\n");
                HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)TxBufferSPI,2);
            }
        }
        

        if (HAL_GetTick() - tickTimer > 100)
        {
            tickTimer = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
        }
        
    } 
    return 0u;
}


void UART_Init()
{
    UartHandle.Instance             = USART2;
    UartHandle.Init.BaudRate        = 115200;
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

HAL_StatusTypeDef correctComand(uint8_t * buffer, uint16_t * addr, uint8_t* byte)
{
    uint8_t * comando_check = NULL;
    uint8_t * addres = NULL;
    uint8_t * byteInput = NULL;
    HAL_StatusTypeDef flag = HAL_ERROR;

    comando_check = (uint8_t*)strtok((char*)buffer," ");
    addres = (uint8_t*)strtok(NULL, " ");
    byteInput = (uint8_t*)strtok(NULL, " ");

    if (strcmp((const char*)comando_check,(const char*)comando) == 0)
    {
        flag = HAL_OK;
    }

    flag = HAL_ERROR;
    if (addres != NULL)
    {
        uint8_t i = 0;
        while (addres[i] != '\0')
        {
            if (addres[i] >= '0' && addres[i] <= '9')
            {
                flag = HAL_OK;
            }
            else if (addres[i] == '\r')
            {
                flag = HAL_OK;
                break;
            }
            else
            {
                flag = HAL_ERROR;
                break;
            }
            i++;
        }
        if (flag != HAL_ERROR)
        {
            flag = HAL_ERROR;
            *addr = atoi((const char*)addres);
            if (*addr >= 0 && *addr <= 4096)
            {
                flag = HAL_OK;
            }   
        }  
    }

    if (byteInput != NULL && flag != HAL_ERROR)
    {
        if (byteInput[0] == '\r' || byteInput[0] == '\0')
        {
            flag = HAL_ERROR;    
        }
        else
        {
            *byte = byteInput[0];
            flag = HAL_OK;    
        }
    }
    
    if (strtok(NULL," ") != NULL)
    {
        flag  = HAL_ERROR;
    }
    return flag;
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