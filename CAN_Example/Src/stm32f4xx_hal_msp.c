#include "main.h"

void HAL_MspInit(void)
{
    // Set up priority grouping
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    // Enable required system exceptions
    // 0x7 = USGFAULTENA, BUSFAULTENA, MEMFAULTENA
    SCB->SHCSR |= 0x7 << 16;

    // Configure priority of exceptions
    HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
    HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
    HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
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

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htimer)
{
    // Enable Clock for TIM peripheral
    __HAL_RCC_TIM6_CLK_ENABLE();

    // Enable IRQ of TIM
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 15, 0);


}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_CAN1_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}