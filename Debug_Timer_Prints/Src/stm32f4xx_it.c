
#include "main.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef hTimer6;

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}

void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&hTimer6);
}