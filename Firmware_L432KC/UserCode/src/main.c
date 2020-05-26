#include "main.h"
#include "nfcm1833_module.h"
#include "ndef.h"
#include "bcdencode.h"

CRC_HandleTypeDef hcrc;

#ifdef USE_TINZ
    SPI_HandleTypeDef spi_to_nfcm1833tinz;
#endif
#ifdef USE_TINY
    //UART_HandleTypeDef huart2;
#endif
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart1;
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init (void);
volatile uint8_t customerID1[ID_TAG_SIZE + 1];
volatile uint8_t customerID2[ID_TAG_SIZE + 1];
volatile NDEF_t ndef_data_add_to_tag;
uint8_t idTag[ID_TAG_SIZE];
uint8_t idTagBCD[ID_TAG_SIZE * 2];
cc_file_data_t cc_data;
tag_format_t tag_data;
uint32_t error_count = 0;
HAL_StatusTypeDef status;
void uart_printf(uint8_t *message);
void SystemClock_Config (void);
static void MX_GPIO_Init (void);
static void MX_CRC_Init (void);

#ifdef USE_TINY
    static void MX_USART2_UART_Init (void);
#endif

#ifdef USE_TINZ
    static void MX_SPI1_Init (void);
#endif /* USE_TINZ */
void display(void* data);
void deleteBuffer(char* buf);
uint8_t dataReceive = 0;
char Sim_response[500] = {0};
char Sim_Rxdata[2] = {0};
int8_t Sim_sendCommand(char*command ,char*response,uint32_t timeout);
int8_t Sim_Response(char*response,uint32_t timeout);
int main (void)
{
    uint8_t count = 0;
    uint8_t temp;

    HAL_Init();
    SystemClock_Config();
    
    MX_GPIO_Init();
    MX_CRC_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    WakeUp_CR95HF();
    display("Ready");
    int a = 0;
    while(a < 1)
    {
      if(Sim_sendCommand("AT","OK",5000))
      {
        HAL_Delay(10);
        if(Sim_sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",5000))
        {
          HAL_Delay(10);
          if(Sim_sendCommand("AT+SAPBR=3,1,\"APN\",\"e-connect\"","OK",5000))		
          {
            HAL_Delay(10);
            if(Sim_sendCommand("AT+SAPBR=1,1","OK",5000))
            {
              if(Sim_sendCommand("AT+HTTPINIT","OK",5000))
              {
                HAL_Delay(10);
                a = 1;
              }
            }
          }
        }
      }  
    }    
    while (1)
        {
          char url[110] = "AT+HTTPPARA=\"URL\",\"http://testcodeesp8266.000webhostapp.com/receiver.php?UID=";
          char url2[10] = "&time=66\"";
          if ( (ping_module() == PING_OK) && (select_tag_type (TYPE_5) == PROTOCOL_OK) && (getDeviceID (idTag) == SEND_RECV_OK))
          {
            idTag[7] = 0x00;
            if (encode8byte_little_edian (idTag, idTagBCD) == 0)
            {
              for (count = 0; count < ID_TAG_SIZE * 2; count++)
              {
                idTagBCD[count] += 0x30;
              }
            }
            memcpy(url+77,idTagBCD,16);
            memcpy(url+93,url2,10);
            display(url); 
            if(Sim_sendCommand(url,"OK",5000))
            {
              HAL_Delay(10);
              if(Sim_sendCommand("AT+HTTPACTION=0","OK",5000))
              {
                if(Sim_Response("200",10000))
                {
                  HAL_Delay(10);
                  if(Sim_sendCommand("AT+HTTPREAD","OK",5000))
                  {
                  
                  }
                }
              }
            }      
                  HAL_Delay(10);
                  if(Sim_sendCommand("AT+HTTPREAD","OK",5000))
                  {
                  
                  }            
            while (getDeviceID (idTag) != NO_TAG)
            {
              HAL_Delay (50);
            }
          }
        }
 
}

void display(void* data)																								
{
	HAL_UART_Transmit(&huart1,(uint8_t *)data,(uint16_t)strlen(data),1000);
}
void deleteBuffer(char* buf)
{
	int len = strlen(buf);
	for(int i = 0; i < len; i++)
	{
		buf[i] = 0;
	}
}
int8_t Sim_sendCommand(char*command ,char*response,uint32_t timeout)
{
  uint8_t answer = 0, count  = 0;
  deleteBuffer(Sim_response);
  uint32_t time = HAL_GetTick();
  uint32_t time1 = HAL_GetTick();
  HAL_UART_Transmit(&huart2, (uint8_t *)command, (uint16_t)strlen(command), 1000);
  HAL_UART_Transmit(&huart2,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
  do
  {
    while(HAL_UART_Receive(&huart2, (uint8_t *)Sim_Rxdata, 1, 1000) != HAL_OK)
    {
      if(HAL_GetTick() - time > timeout) 
      {
        return 0;
      }
    }
    time = HAL_GetTick();
    Sim_response[count++] = Sim_Rxdata[0];
    while((HAL_GetTick() - time < timeout))
    {
      if(HAL_UART_Receive(&huart2, (uint8_t *)Sim_Rxdata, 1, 1000) == HAL_OK)
      {
        Sim_response[count++] = Sim_Rxdata[0];
        time1 = HAL_GetTick();
      }
      else
      {
        if(HAL_GetTick() - time1 > 100)
        {
          if(strstr(Sim_response,response) != NULL) 
          {
            answer = 1;
          }
          else
          {
            answer = 0;
          }
          break;
        }
      }
    }
  }
  while(answer == 0);
  display(Sim_response);
  return answer;
}

int8_t Sim_Response(char*response,uint32_t timeout)
{
  uint8_t answer = 0, count  = 0;
  deleteBuffer(Sim_response);
  uint32_t time = HAL_GetTick();
  uint32_t time1 = HAL_GetTick();
  do
  {
    while(HAL_UART_Receive(&huart2, (uint8_t *)Sim_Rxdata, 1, 1000) != HAL_OK)
    {
      if(HAL_GetTick() - time > timeout) 
      {
        return 0;
      }
    }
    time = HAL_GetTick();
    Sim_response[count++] = Sim_Rxdata[0];
    while((HAL_GetTick() - time < timeout))
    {
      if(HAL_UART_Receive(&huart2, (uint8_t *)Sim_Rxdata, 1, 1000) == HAL_OK)
      {
        Sim_response[count++] = Sim_Rxdata[0];
        time1 = HAL_GetTick();
      }
      else
      {
        if(HAL_GetTick() - time1 > 100)
        {
          if(strstr(Sim_response,response) != NULL) 
          {
            answer = 1;
          }
          else
          {
            answer = 0;
          }
          break;
        }
      }
    }
  }
  while(answer == 0);
  display(Sim_response);
  return answer;
}
void uart_printf(uint8_t *message)
{
        for(int i = 0; i < 16; i++)
	//while(*message != '\0')
	{
		HAL_UART_Transmit(&huart2,(uint8_t *)message,1,10);
		message++;
	}
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config (void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 8;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    
    if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
        {
            Error_Handler();
        }
        
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    
    if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
        {
            Error_Handler();
        }
        
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2
        | RCC_PERIPHCLK_USB;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    
    if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInit) != HAL_OK)
        {
            Error_Handler();
        }
        
    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling (PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
        {
            Error_Handler();
        }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init (void)
{
    /* USER CODE BEGIN CRC_Init 0 */
    /* USER CODE END CRC_Init 0 */
    /* USER CODE BEGIN CRC_Init 1 */
    /* USER CODE END CRC_Init 1 */
    hcrc.Instance = CRC;
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
    hcrc.Init.GeneratingPolynomial    = 0x1021;
    hcrc.Init.CRCLength               = CRC_POLYLENGTH_16B;
    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
    hcrc.Init.InitValue           = 0xFFFF;
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE;
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE;
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
    
    if (HAL_CRC_Init (&hcrc) != HAL_OK)
        {
            Error_Handler();
        }
        
    /* USER CODE BEGIN CRC_Init 2 */
    /* USER CODE END CRC_Init 2 */
}

#ifdef USE_TINZ
/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init (void)
{
    /* USER CODE BEGIN SPI1_Init 0 */
    /* USER CODE END SPI1_Init 0 */
    /* USER CODE BEGIN SPI1_Init 1 */
    /* USER CODE END SPI1_Init 1 */
    /* SPI1 parameter configuration*/
    spi_to_nfcm1833tinz.Instance = SPI1;
    spi_to_nfcm1833tinz.Init.Mode = SPI_MODE_MASTER;
    spi_to_nfcm1833tinz.Init.Direction = SPI_DIRECTION_2LINES;
    spi_to_nfcm1833tinz.Init.DataSize = SPI_DATASIZE_8BIT;
    spi_to_nfcm1833tinz.Init.CLKPolarity = SPI_POLARITY_LOW;
    spi_to_nfcm1833tinz.Init.CLKPhase = SPI_PHASE_1EDGE;
    spi_to_nfcm1833tinz.Init.NSS = SPI_NSS_SOFT;
    spi_to_nfcm1833tinz.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    spi_to_nfcm1833tinz.Init.FirstBit = SPI_FIRSTBIT_MSB;
    spi_to_nfcm1833tinz.Init.TIMode = SPI_TIMODE_DISABLE;
    spi_to_nfcm1833tinz.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    spi_to_nfcm1833tinz.Init.CRCPolynomial = 7;
    spi_to_nfcm1833tinz.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    spi_to_nfcm1833tinz.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    
    if (HAL_SPI_Init (&spi_to_nfcm1833tinz) != HAL_OK)
        {
            Error_Handler();
        }
        
    /* USER CODE BEGIN SPI1_Init 2 */
    /* USER CODE END SPI1_Init 2 */
}
#endif /* USE_TINZ */

#ifdef USE_TINY
/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init (void)
{
    /* USER CODE BEGIN USART2_Init 0 */
    /* USER CODE END USART2_Init 0 */
    /* USER CODE BEGIN USART2_Init 1 */
    /* USER CODE END USART2_Init 1 */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    
    if (HAL_UART_Init (&huart2) != HAL_OK)
        {
            Error_Handler();
        }
}

#endif /* USE_TINY */
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init (void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    
}
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void Error_Handler (void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed (char* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
