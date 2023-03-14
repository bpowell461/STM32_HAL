
#include <string.h>
#include <stdio.h>
#include "main.h"

#define TRUE 1
#define FALSE 0

#define SYS_CLOCK_FREQ_50_MHZ 50

#define INTERVAL_1HZ     1.0
#define INTERVAL_100HZ   0.1

void UART2_Init(void);
void CAN1_Init(void);
void CAN1_Tx(void);
void Error_handler(void);
void Print_MCU_Startup_Banner(void);
void TIMER6_Init(float updateEventMS);

void SystemClockConfig_HSE(uint8_t clock_freq);

UART_HandleTypeDef huart2;
CAN_HandleTypeDef hcan1;

TIM_HandleTypeDef hTimer6;

char msg[50];
uint32_t secondCount = 0;
uint8_t PrintAt1HZ = 0;

int main(void)
{
	HAL_Init();
    //SystemClockConfig_HSE(SYS_CLOCK_FREQ_50_MHZ);
	UART2_Init();
    TIMER6_Init(INTERVAL_1HZ);

    CAN1_Init();

    Print_MCU_Startup_Banner();

    //Start Timer
    HAL_TIM_Base_Start_IT(&hTimer6);

	while(1)
    {
        if(PrintAt1HZ)
        {
            sprintf(msg, "DEBUG: Time <%.2f>\n\r", (float) secondCount);
            CAN1_Tx();
            HAL_UART_Transmit(&huart2, (uint8_t *)&msg, sizeof(msg), HAL_MAX_DELAY);
            secondCount++;
            PrintAt1HZ = 0;
        }
    }


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

void CAN1_Init(void)
{
    hcan1.Instance = CAN1;
    hcan1.Init.Mode = CAN_MODE_LOOPBACK;
    hcan1.Init.AutoRetransmission = DISABLE;
    hcan1.Init.AutoWakeUp = DISABLE;
    hcan1.Init.ReceiveFifoLocked = DISABLE;
    hcan1.Init.TimeTriggeredMode = DISABLE;
    hcan1.Init.TransmitFifoPriority = DISABLE;

    // Bit Timing
    hcan1.Init.Prescaler = 5;
    hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
    hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;

    // Sleep Mode to Initialization Mode
    if (HAL_CAN_Init(&hcan1) != HAL_OK)
    {
        Error_handler();
    }

    // Move from Initialization mode to Normal mode
    if (HAL_CAN_Start(&hcan1) != HAL_OK)
    {
        Error_handler();
    }
}

void CAN1_Tx(void)
{
    CAN_TxHeaderTypeDef  TX_Header;
    uint32_t CAN_TX_Mailbox;

    char* message = "Hello!";

    TX_Header.DLC = 6;
    TX_Header.StdId = 0x65D;
    TX_Header.IDE = CAN_ID_STD;
    TX_Header.RTR = CAN_RTR_DATA;

    if (HAL_CAN_AddTxMessage(&hcan1, &TX_Header, (uint8_t *) &message, &CAN_TX_Mailbox) != HAL_OK)
    {
        Error_handler();
    }

    while(HAL_CAN_IsTxMessagePending(&hcan1, CAN_TX_Mailbox));

    sprintf(msg, "CAN Message Transmitted\r\n");
    HAL_UART_Transmit(&huart2, (uint8_t *)&msg, sizeof(msg), HAL_MAX_DELAY);
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
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClockConfig_HSE(uint8_t clock_freq)
{
    RCC_OscInitTypeDef Osc_Init;
    RCC_ClkInitTypeDef Clock_Init;
    uint8_t flash_latency=0;

    Osc_Init.OscillatorType = RCC_OSCILLATORTYPE_HSE ;
    Osc_Init.HSEState = RCC_HSE_ON;
    Osc_Init.PLL.PLLState = RCC_PLL_ON;
    Osc_Init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    switch(clock_freq)
    {
        case SYS_CLOCK_FREQ_50_MHZ:
            Osc_Init.PLL.PLLM = 4;
            Osc_Init.PLL.PLLN = 50;
            Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
            Osc_Init.PLL.PLLQ = 2;
            Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                   |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
            Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
            Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
            Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
            flash_latency = 1;
            break;

        case SYS_CLOCK_FREQ_84_MHZ:
            Osc_Init.PLL.PLLM = 4;
            Osc_Init.PLL.PLLN = 84;
            Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
            Osc_Init.PLL.PLLQ = 2;
            Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                   |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
            Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
            Clock_Init.APB1CLKDivider = RCC_HCLK_DIV2;
            Clock_Init.APB2CLKDivider = RCC_HCLK_DIV1;
            flash_latency = 2;
            break;

        case SYS_CLOCK_FREQ_120_MHZ:
            Osc_Init.PLL.PLLM = 4;
            Osc_Init.PLL.PLLN = 120;
            Osc_Init.PLL.PLLP = RCC_PLLP_DIV2;
            Osc_Init.PLL.PLLQ = 2;
            Clock_Init.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                   |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
            Clock_Init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
            Clock_Init.AHBCLKDivider = RCC_SYSCLK_DIV1;
            Clock_Init.APB1CLKDivider = RCC_HCLK_DIV4;
            Clock_Init.APB2CLKDivider = RCC_HCLK_DIV2;
            flash_latency = 3;
            break;

        default:
            return;
    }

    if (HAL_RCC_OscConfig(&Osc_Init) != HAL_OK)
    {
        Error_handler();
    }



    if (HAL_RCC_ClockConfig(&Clock_Init, flash_latency) != HAL_OK)
    {
        Error_handler();
    }


    /*Configure the systick timer interrupt frequency (for every 1 ms) */
    uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
    HAL_SYSTICK_Config(hclk_freq/1000);

    /**Configure the Systick
    */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);



}

void Error_handler(void)
{
	while(1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    PrintAt1HZ = 1;
}

void Print_MCU_Startup_Banner(void)
{
    const char banner[175] = {
    "\n\r\n\r_______________\n\r|,----------.  |\\\n\r||           |=| |\n\r|| POWER ON  | | |\n\r||       . _o| | | __\n\r|`-----------' |/ /~/\n\r~~~~~~~~~~~~~~~ / /\n\r~~\n\r\n\r\n\r"
    };

    HAL_UART_Transmit(&huart2, (uint8_t*)&banner, sizeof(banner), HAL_MAX_DELAY);
}