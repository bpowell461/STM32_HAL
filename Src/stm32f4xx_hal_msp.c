#include "stm32f4xx_hal.h"

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