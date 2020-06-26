#include "main.h"
#include "nfcm1833_module.h"
#include "ndef.h"
#include "bcdencode.h"
#include "i2c-lcd.h"
#include <string.h>

CRC_HandleTypeDef hcrc;
I2C_HandleTypeDef hi2c1;
#ifdef USE_TINZ
    SPI_HandleTypeDef spi_to_nfcm1833tinz;
#endif

UART_HandleTypeDef huart1;
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
volatile uint8_t customerID1[ID_TAG_SIZE + 1];
volatile uint8_t customerID2[ID_TAG_SIZE + 1];
volatile NDEF_t ndef_data_add_to_tag;
uint8_t idTag[ID_TAG_SIZE];
uint8_t idTagBCD[ID_TAG_SIZE * 2];
cc_file_data_t cc_data;
tag_format_t tag_data;
uint32_t error_count = 0;
HAL_StatusTypeDef status;
void SystemClock_Config (void);
static void MX_GPIO_Init (void);
static void MX_CRC_Init (void);

#ifdef USE_TINZ
    static void MX_SPI1_Init (void);
#endif 
void display(char* data);
void deleteBuffer(char* buf);
uint8_t dataReceive = 0;
char Sim_response[500] = {0};
char Sim_Rxdata[2] = {0};
int8_t Sim_sendCommand(char*command ,char*response,uint32_t timeout);
int8_t Sim_Response(char*response,uint32_t timeout);
uint8_t permissReadTag = 0;
uint8_t So_Tien_Pay[16];
uint8_t viTriTien = 0;
uint8_t So_Bill[16];
uint8_t viTriBill = 0;

void LCD_Display_Faild();
void LCD_Display_Success();
void LCD_Dislay_Clear();
void LCD_Display(char* str);
int main (void)
{
 uint8_t count = 0;
    HAL_Init();
    SystemClock_Config();
    
    MX_GPIO_Init();
    MX_CRC_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    HAL_Delay(5000);
    WakeUp_CR95HF();
    
    lcd_init();
    lcd_goto_XY(1,4);
    lcd_send_string("Welcome");    
    int a = 0;
    while(a < 1)
    {
      if(Sim_sendCommand("AT","OK",10000))
      {
        lcd_goto_XY(2,5);
        lcd_send_string(".");
        HAL_Delay(10);
        if(Sim_sendCommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"","OK",10000))
        {
          HAL_Delay(10);
          lcd_goto_XY(2,6);
          lcd_send_string(".");          
          if(Sim_sendCommand("AT+SAPBR=3,1,\"APN\",\"e-connect\"","OK",10000))		
          {
            HAL_Delay(10);
            lcd_goto_XY(2,7);
            lcd_send_string(".");            
            if(Sim_sendCommand("AT+SAPBR=1,1","OK",10000))
            {
              HAL_Delay(10);
              lcd_goto_XY(2,8);
              lcd_send_string(".");              
              if(Sim_sendCommand("AT+HTTPINIT","OK",10000))
              {
                HAL_Delay(10);
                lcd_goto_XY(2,9);
                lcd_send_string(".");                              
                a = 1;
              }
            }
          }
        }
      }  
    }    
    lcd_clear_display();
    HAL_Delay(500);
    lcd_goto_XY(1,5);
    lcd_send_string("Ready"); 
    HAL_Delay(1000);
    lcd_clear_display();
    HAL_Delay(1000);
    lcd_goto_XY(1,5);
    lcd_send_string("Conek");    
    HAL_Delay(1000);
    while (1)
    {  
          char url[100] = "AT+HTTPPARA=\"URL\",\"http://testcodeesp8266.000webhostapp.com/receiver.php?UID=";
          switch(permissReadTag)
          {
          case 0:
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
                __HAL_SPI_DISABLE(&spi_to_nfcm1833tinz);
                HAL_Delay (100);
                lcd_clear_display();
                HAL_Delay (100);
                lcd_goto_XY(1,5);
                lcd_send_string("Conek");    
                lcd_goto_XY(2,0);
                lcd_send_string((char *)idTagBCD); 
                HAL_Delay (100);
                lcd_clear_display();
                HAL_Delay (100);
                lcd_goto_XY(1,3);
                lcd_send_string("Nhap Bill:");        
                permissReadTag = 1;  
              }            
              break;
            case 1:
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==0)
              {
                HAL_Delay(100);
                viTriBill++;    
                So_Bill[viTriBill - 1] = 0x30;
                LCD_Display((char *)So_Bill);
                
              }
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x31; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x32; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x33; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x34; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x35; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x36; 
                LCD_Display((char *)So_Bill);
              }
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x37; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x38; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==0)
              {
                HAL_Delay(100);
                viTriBill++;
                So_Bill[viTriBill - 1] = 0x39; 
                LCD_Display((char *)So_Bill);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0)
              {
                permissReadTag = 2;
                HAL_Delay(200);
                lcd_clear_display();
                HAL_Delay (100);
                lcd_goto_XY(1,5);
                lcd_send_string("Conek");    
                HAL_Delay (100);
                lcd_goto_XY(1,3);
                lcd_send_string("Nhap Tien:");                                
              }                   
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==0)
              {
                So_Bill[viTriBill - 1] = 0;
                if(viTriBill > 0){
                  viTriBill--;
                }else{
                  viTriBill = 0;
                }
                LCD_Display((char *)So_Bill);
                HAL_Delay(300);
              }                       
              break;              
            case 2:
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==0)
              {
                HAL_Delay(100);
                viTriTien++;    
                So_Tien_Pay[viTriTien - 1] = 0x30;
                LCD_Display((char *)So_Tien_Pay);
                
              }
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x31; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x32; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x33; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x34; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x35; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x36; 
                LCD_Display((char *)So_Tien_Pay);
              }
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x37; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x38; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==0)
              {
                HAL_Delay(100);
                viTriTien++;
                So_Tien_Pay[viTriTien - 1] = 0x39; 
                LCD_Display((char *)So_Tien_Pay);
              }        
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==0)
              {
                permissReadTag = 3;
                HAL_Delay(200);
              }                   
              if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==0)
              {
                So_Tien_Pay[viTriTien - 1] = 0;
                if(viTriTien > 0){
                  viTriTien--;
                }else{
                  viTriTien = 0;
                }
                LCD_Display((char *)So_Tien_Pay);
                HAL_Delay(300);
              }                       
              break;
          default:
              LCD_Dislay_Clear();
              HAL_Delay(100);
              lcd_goto_XY(1,5);
              lcd_send_string("Conek");                    
              lcd_goto_XY(2,3);
              lcd_send_string("Sending...");     
              if(Sim_sendCommand("AT+SAPBR=1,1","ERROR",10000)){
                HAL_Delay(10);
              }else{
                if(Sim_sendCommand("AT+SAPBR=0,1","OK",10000)){
                  HAL_Delay(10);
                  if(Sim_sendCommand("AT+SAPBR=1,1","OK",10000)){
                    HAL_Delay(10);
                    if(Sim_sendCommand("AT+HTTPINIT","ERROR",10000))
                    {
                      HAL_Delay(10);
                    }
                  }
                }
              }               
              display((char *)url);
              for(uint8_t abc = 0; abc < 16; abc++){
                HAL_UART_Transmit(&huart1,&idTagBCD[abc],1,1000);
              }
              //display((char *)idTagBCD[1]);
              //display((char *)idTagBCD[2]);
              display("&bill=");
              display((char*)So_Bill);
              display("&money=");
              display((char *)So_Tien_Pay);
              permissReadTag = 0;
                if(Sim_sendCommand("\"","OK",10000))
                {
                  HAL_Delay(10);
                  if(Sim_sendCommand("AT+HTTPPARA=\"CID\",1","OK",10000))
                  {
                    HAL_Delay(10);
                    if(Sim_sendCommand("AT+HTTPACTION=0","OK",10000))
                    {
                      if(Sim_Response("200",10000))
                      {     
                        LCD_Display_Success();   
//                        if(Sim_sendCommand("AT+HTTPREAD","10",10000)){
//                          
//                        }
//                         
                      }
                      else
                      {
                        LCD_Display_Faild();                        
                      }
                    }
                    else
                    {
                      LCD_Display_Faild();
                    }
                  }
                }
              HAL_Delay(100);
              LCD_Dislay_Clear();
              HAL_Delay(100);
              for(uint8_t abc = 0; abc < 16; abc++){
                So_Tien_Pay[abc] = 0;
                So_Bill[abc] = 0;
                idTagBCD[abc] = 0;
              }
              viTriTien = 0;
              viTriBill = 0;
              break;
          }
    }
 
}
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==1)
//  {
//    permissReadTag = 2;
//  }
//}
void LCD_Dislay_Clear()
{
  lcd_clear_display();
  HAL_Delay (100);
  lcd_goto_XY(1,5);
  lcd_send_string("Conek");  
}
void LCD_Display(char* str)
{
  lcd_clear_display();
  HAL_Delay (100);
  lcd_goto_XY(1,3);
  lcd_send_string("Nhap Tien:");                     
  lcd_goto_XY(2,4);
  lcd_send_string(str);          
  HAL_Delay(300);  
}
void LCD_Display_Success()
{
  lcd_clear_display();
  HAL_Delay (100);
  lcd_goto_XY(1,5);
  lcd_send_string("Conek"); 
  lcd_goto_XY(2,2);
  lcd_send_string("Send Success");
  HAL_Delay(1000);  
}
void LCD_Display_Faild()
{
  lcd_clear_display();
  HAL_Delay (100);
  lcd_goto_XY(1,5);
  lcd_send_string("Conek"); 
  lcd_goto_XY(2,2);
  lcd_send_string("Send Failed");
  HAL_Delay(1000);
}
void display(char* data)																								
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
  HAL_UART_Transmit(&huart1, (uint8_t *)command, (uint16_t)strlen(command), 1000);
  HAL_UART_Transmit(&huart1,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
  do
  {
    while(HAL_UART_Receive(&huart1, (uint8_t *)Sim_Rxdata, 1, 1000) != HAL_OK)
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
      if(HAL_UART_Receive(&huart1, (uint8_t *)Sim_Rxdata, 1, 1000) == HAL_OK)
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
  //display(Sim_response);
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
    while(HAL_UART_Receive(&huart1, (uint8_t *)Sim_Rxdata, 1, 1000) != HAL_OK)
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
      if(HAL_UART_Receive(&huart1, (uint8_t *)Sim_Rxdata, 1, 1000) == HAL_OK)
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
  //display(Sim_response);
  return answer;
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
    RCC_OscInitStruct.PLL.PLLN = 10;
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
        | RCC_PERIPHCLK_USB | RCC_PERIPHCLK_I2C1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
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
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

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

static void MX_GPIO_Init (void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    
    /*Configure GPIO pin : PA0 */
//    GPIO_InitStruct.Pin = GPIO_PIN_0;
//    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /*Configure GPIO pins : PA3 PA8 PA11 PA12 */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12
                          ;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    
    
    /*Configure GPIO pins : PB0 PB1 PB3 PB4 
                             PB5 */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_4 
                            |GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    /* EXTI interrupt init*/
//    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
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
