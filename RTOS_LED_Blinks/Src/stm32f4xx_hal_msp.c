#include "main.h"

void HAL_MspInit(void)
{
    /* USER CODE BEGIN MspInit 0 */

    /* USER CODE END MspInit 0 */

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /* System interrupt init*/

    /* USER CODE BEGIN MspInit 1 */

    /* USER CODE END MspInit 1 */
}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // Enable peripheral clock
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    // Configure pin muxing
    GPIO_InitTypeDef gpio_uart;

    // Configuring TX Line
    gpio_uart.Pin       = GPIO_PIN_2;
    gpio_uart.Mode      = GPIO_MODE_AF_PP;
    gpio_uart.Pull      = GPIO_PULLDOWN;
    gpio_uart.Speed     = GPIO_SPEED_FREQ_LOW;
    gpio_uart.Alternate = GPIO_AF7_USART2;

    HAL_GPIO_Init(GPIOA, &gpio_uart);

    // Configuring RX Line
    gpio_uart.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &gpio_uart);

    // Enable IRQ and set up priority
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
}
