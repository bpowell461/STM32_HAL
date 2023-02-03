
#include <string.h>
#include <stdio.h>
#include "main.h"

#define TRUE 1
#define FALSE 0

#define INTERVAL_1HZ     1.0
#define INTERVAL_100HZ   0.1

void UART2_Init(void);
void Error_handler(void);
void Print_MCU_Startup_Banner(void);
void TIMER6_Init(float updateEventMS);

void SystemClockConfig();

UART_HandleTypeDef huart2;

TIM_HandleTypeDef hTimer6;

char msg[50];
uint32_t secondCount = 0;

int main(void)
{
	HAL_Init();
    SystemClockConfig();
	UART2_Init();
    TIMER6_Init(INTERVAL_1HZ);

    Print_MCU_Startup_Banner();

    //Start Timer
    HAL_TIM_Base_Start_IT(&hTimer6);

	while(1);


	return 0;
}

void UART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	if ( HAL_UART_Init(&huart2) != HAL_OK )
	{
		//There is a problem
		Error_handler();
	}


}

void TIMER6_Init(float updateEventMS)
{
    memset(&hTimer6, 0, sizeof(hTimer6));
    const uint32_t clockHZ = HAL_RCC_GetHCLKFreq();
    hTimer6.Instance = TIM6;
    hTimer6.Init.Prescaler = 124;

    uint32_t cntCLK = clockHZ / (hTimer6.Init.Prescaler + 1);
    double timePeriod = 1 / (double) cntCLK;
    double localPeriod = (double) updateEventMS / timePeriod;

    hTimer6.Init.Period = (uint32_t)localPeriod-1;
    hTimer6.Init.CounterMode = TIM_COUNTERMODE_UP;
    hTimer6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    hTimer6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if( HAL_TIM_Base_Init(&hTimer6) != HAL_OK )
    {
        Error_handler();
    }
    __HAL_TIM_CLEAR_FLAG(&hTimer6, TIM_SR_UIF);

}
void SystemClockConfig()
{
    RCC_OscInitTypeDef osc_init;
    RCC_ClkInitTypeDef clk_init;

    memset(&osc_init,0,sizeof(osc_init));
    osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_init.HSEState = RCC_HSE_BYPASS;
    if ( HAL_RCC_OscConfig(&osc_init) != HAL_OK)
    {
        Error_handler();
    }

    clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
    					RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    clk_init.AHBCLKDivider = RCC_SYSCLK_DIV2;
    clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
    clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

    if( HAL_RCC_ClockConfig(&clk_init, FLASH_ACR_LATENCY_0WS) != HAL_OK)
    {
        Error_handler();
    }

    __HAL_RCC_HSI_DISABLE(); //Saves some current

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void Error_handler(void)
{
	while(1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    sprintf(msg, "DEBUG: Time <%.2f>\n\r", (float) secondCount);
    HAL_UART_Transmit(&huart2, (uint8_t *)&msg, sizeof(msg), HAL_MAX_DELAY);
    secondCount++;
}

void Print_MCU_Startup_Banner(void)
{
    const char banner[175] = {
    "\n\r\n\r_______________\n\r|,----------.  |\\\n\r||           |=| |\n\r|| POWER ON  | | |\n\r||       . _o| | | __\n\r|`-----------' |/ /~/\n\r~~~~~~~~~~~~~~~ / /\n\r~~\n\r\n\r\n\r"
    };

    HAL_UART_Transmit(&huart2, (uint8_t*)&banner, sizeof(banner), HAL_MAX_DELAY);
}