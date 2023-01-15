#include "main.h"

#include <string.h>
void SystemClockConfig(void);
void UART2_Init(void);
void Error_Handler(void);

UART_HandleTypeDef huart2;

static char *user_data = "Hello World\r\n";
uint8_t data_buffer[100];
uint8_t rcvd_data;
uint32_t count = 0;
uint8_t reception_complete = 0;

int main(void) {
    HAL_Init();
    SystemClockConfig();
    UART2_Init();

    HAL_UART_Transmit(&huart2, (uint8_t *) user_data, strlen(user_data), HAL_MAX_DELAY);

    while(reception_complete != 1)
    {
        HAL_UART_Receive_IT(&huart2, &rcvd_data, 1);
    }

    while(1);

    return 0;
}

void SystemClockConfig(void)
{

}

void UART2_Init(void)
{
    // Linking address
    huart2.Instance = USART2;

    //Initialization
    huart2.Init.BaudRate    = 115200;
    huart2.Init.WordLength  = UART_WORDLENGTH_8B;
    huart2.Init.StopBits    = UART_STOPBITS_1;
    huart2.Init.Parity      = UART_PARITY_NONE;
    huart2.Init.HwFlowCtl   = UART_HWCONTROL_NONE;
    huart2.Init.Mode        = UART_MODE_TX_RX;
    if( HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    // Do something here
    while (1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(rcvd_data == '\r')
    {
        reception_complete = 1;
        data_buffer[count++] = '\r';
        HAL_UART_Transmit(huart, data_buffer, count, HAL_MAX_DELAY);
    }
    else
    {
        data_buffer[count++] = rcvd_data;
    }
}