
#include "main.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef hTimer6;

void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();
}
