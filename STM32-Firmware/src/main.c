/**
  ******************************************************************************
  * @file    FatFs/FatFs_USBDisk/Src/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-March-2014
  * @brief   Main program body
  *          This sample code shows how to use FatFs with USB disk drive.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


#define BUFFERSIZE 128 // ~0.428 MB/s = 52,6kb/s -> ~50Hz DMA interrupt
static uint8_t buf[2][BUFFERSIZE];
static uint8_t bufno = 0;
volatile uint8_t newbuf = 0;
static int bufoffset = 0;

DMA_HandleTypeDef     DmaHandle;

static void SystemClock_Config(void);

/* Private functions ---------------------------------------------------------*/

UART_HandleTypeDef UARTHandle;

int _write (int fd, char *pBuffer, int size)
{
	HAL_UART_Transmit(&UARTHandle, pBuffer, size, 10000);
	return size;
}

int _fstat (int fd, struct stat *pStat)
{
   pStat->st_mode = S_IFCHR;
   return 0;
}

int _close(int a)
{
   return -1;
}

int _isatty (int fd)
{
   return 1;
}

int _lseek(int a, int b, int c)
{
   return -1;
}

int _read (int fd, char *pBuffer, int size)
{
	//HAL_UART_Receive(&UARTHandle, pBuffer, size, 10000);
	return size;
}

void UART_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	/* Enable USARTx clock */
	__USART3_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* UART TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       =  GPIO_PIN_10;
	  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	  GPIO_InitStruct.Pull      = GPIO_PULLUP;
	  GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;

	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	  /* UART RX GPIO pin configuration  */
	  GPIO_InitStruct.Pin = GPIO_PIN_11;
	  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;

	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	//HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	UART_InitTypeDef UARTInit;
	UARTInit.BaudRate = 9600; // NIE DZIAŁA - DAJE 30018
	UARTInit.WordLength = UART_WORDLENGTH_8B;
	UARTInit.StopBits = UART_STOPBITS_1;
	UARTInit.Parity = UART_PARITY_NONE;
	UARTInit.OverSampling = UART_OVERSAMPLING_8;
	UARTInit.Mode = UART_MODE_TX_RX;
	UARTInit.HwFlowCtl = UART_HWCONTROL_NONE;

	UARTHandle.Init = UARTInit;
	UARTHandle.Instance = USART3;

	HAL_UART_Init(&UARTHandle);

	//USART3->BRR = ((0x187 << 4) + 0x00); // 60MHZ / 16*BAUD = [FRACTION << 4 + MANTISSA (x/16)]
	USART3->BRR = ((0x08 << 4) + 0x02); // 60MHZ / 16*BAUD = [FRACTION << 4 + MANTISSA (x/16)]

	__HAL_UART_ENABLE(&UARTHandle);


}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

void vSoundTask(void)
{
	//static int offset = 0;

	VS1003_GPIO_conf();
	VS1003_SPI_conf();
	VS1003_Start();
	VS1003_PluginLoad();
	VS1003_SetBass(2);
	VS1003_SetVolume(60);
	VS1003_SPI_SpeedUp();

	while(1)
	{
//		if(tCommand == TRACK_PLAY)
//		{
//			bufoffset += VS1003_SendMusicBytes(&buf[bufno][bufoffset], BUFFERSIZE-bufoffset);
//			if(bufoffset >= BUFFERSIZE)
//			{
//				bufoffset = 0;
//				if(bufno == 0) bufno = 1; else bufno = 0;
//				bufreq = 1;
//			}
//		}
		vTaskDelay(5);
	}
}

void vUARTTask(void)
{
	while(1)
	{
		vTaskDelay(10);
	}
}

void DMAFinish() // TODO: Optymalizacja - sprawdzić disassembly?
{
	if(bufno == 0) bufno = 1; else bufno = 0;
	newbuf = 1;
	HAL_DMA_Start_IT(&DmaHandle, (uint32_t)&USART3->DR, (uint32_t)&(buf[bufno][0]), BUFFERSIZE);
//	for(int i=0; i<BUFFERSIZE; i++)
//	{
//		while(!(USART3->SR & USART_SR_TXE));
//			  USART3->DR = buf[bufno][i];
//	}
    //
    //     Test if DMA Stream Transfer Complete interrupt
    //
//    if (DMA2->LISR & DMA_LISR_TCIF3)
//    {
//        //DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
//    	DMA2->LIFCR = DMA_LIFCR_CTCIF3;
//        //
//        //    The following is required to ensure that the chip select is not
//        //    changed while data is still being transmitted.
//        //
//    }
}

void dmaErrorCallback()
{
	while(1);
	//printf("*DMA error\r\n");
}

void DMA1_Stream1_IRQHandler(void)
{
	//printf("*DMA irq\r\n");
  /* Check the interrupt and clear flag */
  HAL_DMA_IRQHandler(&DmaHandle);
}


static void DMA_Config(void)
{
  /*## -1- Enable DMA2 clock #################################################*/
  __DMA1_CLK_ENABLE();

  /*#define DMA_STREAM               DMA1_Stream1
#define DMA_CHANNEL              DMA_CHANNEL_4
#define DMA_STREAM_IRQ           DMA1_Stream1_IRQn
#define DMA_STREAM_IRQHANDLER    DMA1_Stream1_IRQHandler*/


  /*##-2- Select the DMA functional Parameters ###############################*/
  DmaHandle.Init.Channel = DMA_CHANNEL_4;                     /* DMA_CHANNEL_0                    */
  DmaHandle.Init.Direction = DMA_PERIPH_TO_MEMORY;          /* M2M transfer mode                */
  DmaHandle.Init.PeriphInc = DMA_PINC_DISABLE;               /* Peripheral increment mode Enable */
  DmaHandle.Init.MemInc = DMA_MINC_ENABLE;                  /* Memory increment mode Enable     */
  DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; /* Peripheral data alignment : Word */
  DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;    /* memory data alignment : Word     */
  DmaHandle.Init.Mode = DMA_NORMAL;                         /* Normal DMA mode                  */
  DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;              /* priority level : high            */
  DmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled               */
  DmaHandle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
  DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */

  /*##-3- Select the DMA instance to be used for the transfer : DMA2_Stream0 #*/
  DmaHandle.Instance = DMA1_Stream1;

  /*##-4- Select Callbacks functions called after Transfer complete and Transfer error */
  DmaHandle.XferCpltCallback  = DMAFinish;
  DmaHandle.XferErrorCallback = dmaErrorCallback;

  /*##-5- Initialize the DMA stream ##########################################*/
  if (HAL_DMA_Init(&DmaHandle) != HAL_OK)
  {
    /* Initialization Error */
	  while(1);
    //Error_Handler();
  }

  /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
  /* Set Interrupt Group Priority */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);

  /* Enable the DMA STREAM global Interrupt */
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

  USART3->CR3 |= USART_CR3_DMAR;

  /*##-7- Start the DMA transfer using the interrupt mode ####################*/
  /* Configure the source, destination and buffer size DMA fields and Start DMA Stream transfer */
  /* Enable All the DMA interrupts */
  if (HAL_DMA_Start_IT(&DmaHandle, (uint32_t)&USART3->DR, (uint32_t)&(buf[0][0]), BUFFERSIZE) != HAL_OK)
  {
    /* Transfer Error */
	  while(1);
    //Error_Handler();
  }
}
//
//void DMAConfg(void)
//{
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
//
//	DMA_InitTypeDef DMA_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	DMA_DeInit(DMA2_Stream3);
//	DMA_StructInit(&DMA_InitStructure);
//
//	DMA_InitStructure.Channel            = DMA_Channel_3;
//	DMA_InitStructure.PeripheralBaseAddr = (uint32_t)&SPI1->DR;
//	DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t) dmabuffer1;
//	DMA_InitStructure.DMA_DIR                = DMA_DIR_MemoryToPeripheral;
//	DMA_InitStructure.DMA_BufferSize         = 3;
//	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable;
//	DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
//
//	DMA_Init(DMA2_Stream3, &DMA_InitStructure);
//
//	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE);
//
//	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
//}
int main(void)
{
  /* STM32F2xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
  HAL_Init();
  /* Configure the system clock to 120 MHz */
  SystemClock_Config();
  
  /* Configure LED1 and LED3 */
//  BSP_LED_Init(LED1);
//  BSP_LED_Init(LED3);

  UART_Init();

//  while(1)
//  {
//	  while(!(USART3->SR & USART_SR_RXNE));
//	  char t = USART3->DR;
//	  while(!(USART3->SR & USART_SR_TXE));
//	  USART3->DR = t;
//  }

	VS1003_GPIO_conf();
	VS1003_SPI_conf();
	VS1003_Start();
	VS1003_PluginLoad();
	VS1003_SetBass(2);
	VS1003_SetVolume(60);
	//VS1003_SineTest();
	VS1003_SPI_SpeedUp();

	printf("*VS1053 initialized\r\n");

  DMA_Config();

  unsigned char *test = "cli.start\r\n";
	for(int i=0; i<11; i++)
	{
	  while(!(USART3->SR & USART_SR_TXE));
	  USART3->DR = test[i];
	}

  printf("*DMA started\r\n");

  //HAL_UART_Transmit(&UARTHandle, test, 5, 10000);

 // printf("Test UART\n*Uruchamianie testu VS1053\n");



  //VS1003_SineTest();
//  	VS1003_GPIO_conf();
//	VS1003_SPI_conf();
//	VS1003_Start();
//	//VS1003_SineTest();
////	VS1003_PluginLoad();
////	VS1003_SetBass(2);
//	VS1003_SetVolume(50);
//	VS1003_SPI_SpeedUp();
//	soundena++;

  //printf("*Uruchomiono test VS1053\n");
//  xTaskHandle BackgorundTaskHandle, UARTTaskHandle, FilesystemTaskHandle, USBTaskHandle, SoundTaskHandle;
//
//  xTaskCreate( vUARTTask, ( signed portCHAR * ) "UART", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &UARTTaskHandle );
//  xTaskCreate( vSoundTask, ( signed portCHAR * ) "SOUND", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &SoundTaskHandle );
//
//  HAL_SYSTICK_Config(120000000 / 1000);
//  vTaskStartScheduler();

int i = 0;
  while(1)
  {
	  if(newbuf)
	  {
		  newbuf = 0;
		  uint8_t t = 0;
		  if(bufno == 0) t = 1;
//		  for(int i=0; i<BUFFERSIZE; i++)
//		  	{
//		  		while(!(USART3->SR & USART_SR_TXE));
//		  			  USART3->DR = buf[t][i];
//		  	}
		  //i++;
//		  newbuf = 0;
//		  uint8_t t = 0;
//		  if(!bufno) t = 1;
		  VS1003_SendMusicBytes(&(buf[t][0]), BUFFERSIZE);
		  //printf("%d\r", i);
	  }
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 240
  *            PLL_P                          = 2HAL UART Init
  *            PLL_Q                          = 5
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 240;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
}

//#ifdef USE_FULL_ASSERT
///**
//  * @brief  Reports the name of the source file and the source line number
//  *         where the assert_param error has occurred.
//  * @param  file: pointer to the source file name
//  * @param  line: assert_param error line source number
//  * @retval None
//  */
//void assert_failed(uint8_t* file, uint32_t line)
//{
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
//
//  /* Infinite loop */
//  while (1)
//  {}
//}
//
//#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
