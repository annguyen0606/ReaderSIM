/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v2.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_vcp.h"
#include "ndef.h"
#include "main.h"
#include "bcdencode.h"
/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern CRC_HandleTypeDef hcrc;
extern uint32_t* run_mode;
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE  512
#define APP_TX_DATA_SIZE  512
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
uint8_t volatile waiting_more_data = 0x00;
uint8_t UserRxCustomBuffer[APP_TX_DATA_SIZE+1] = {0x00};

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef USBD_Device;
/* USB Device Core handle declaration. */
extern USBD_DescriptorsTypeDef FS_Desc;
/* USER CODE BEGIN EXPORTED_VARIABLES */
extern volatile uint8_t customerID1[ID_TAG_SIZE + 1];
extern volatile uint8_t customerID2[ID_TAG_SIZE + 1];
extern volatile NDEF_t ndef_data_add_to_tag;
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS (void);
static int8_t CDC_DeInit_FS (void);
static int8_t CDC_Control_FS (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS (uint8_t* pbuf, uint32_t* Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
    CDC_Init_FS,
    CDC_DeInit_FS,
    CDC_Control_FS,
    CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS (void)
{
    /* USER CODE BEGIN 3 */
    /* Set Application Buffers */
    USBD_CDC_SetTxBuffer (&USBD_Device, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer (&USBD_Device, UserRxBufferFS);
    return (USBD_OK);
    /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS (void)
{
    /* USER CODE BEGIN 4 */
    return (USBD_OK);
    /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
    /* USER CODE BEGIN 5 */
    switch (cmd)
        {
            case CDC_SEND_ENCAPSULATED_COMMAND:
                break;
                
            case CDC_GET_ENCAPSULATED_RESPONSE:
                break;
                
            case CDC_SET_COMM_FEATURE:
                break;
                
            case CDC_GET_COMM_FEATURE:
                break;
                
            case CDC_CLEAR_COMM_FEATURE:
                break;
                
            /*******************************************************************************/
            /* Line Coding Structure                                                       */
            /*-----------------------------------------------------------------------------*/
            /* Offset | Field       | Size | Value  | Description                          */
            /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
            /* 4      | bCharFormat |   1  | Number | Stop bits                            */
            /*                                        0 - 1 Stop bit                       */
            /*                                        1 - 1.5 Stop bits                    */
            /*                                        2 - 2 Stop bits                      */
            /* 5      | bParityType |  1   | Number | Parity                               */
            /*                                        0 - None                             */
            /*                                        1 - Odd                              */
            /*                                        2 - Even                             */
            /*                                        3 - Mark                             */
            /*                                        4 - Space                            */
            /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
            /*******************************************************************************/
            case CDC_SET_LINE_CODING:
                break;
                
            case CDC_GET_LINE_CODING:
                break;
                
            case CDC_SET_CONTROL_LINE_STATE:
                break;
                
            case CDC_SEND_BREAK:
                break;
                
            default:
                break;
        }
        
    return (USBD_OK);
    /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will block any OUT packet reception on USB endpoint
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result
  *         in receiving more data while previous ones are still not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS (uint8_t* Buf, uint32_t* Len)
{
    /* USER CODE BEGIN 6 */
    USBD_CDC_SetRxBuffer (&USBD_Device, &Buf[0]);
    USBD_CDC_ReceivePacket (&USBD_Device);
    if (*Len == 0x40) waiting_more_data = 1;
    if (waiting_more_data == 0)
    {
      VCP_Reply_Short (Buf, *Len);
    }
    else
    {
      if ((*Len + UserRxCustomBuffer[0]) < APP_TX_DATA_SIZE+1)
      {
        memcpy(UserRxCustomBuffer+UserRxCustomBuffer[0]+1,Buf,*Len);
        UserRxCustomBuffer[0]+= *Len;
        if (*Len < 0x40) 
        {
          waiting_more_data = 0;
          VCP_Reply_Long (UserRxCustomBuffer+1, UserRxCustomBuffer[0]);
          UserRxCustomBuffer[0] = 0x00;
        }
      }
    }
    return (USBD_OK);
    /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS (uint8_t* Buf, uint16_t Len)
{
    uint8_t result = USBD_OK;
    /* USER CODE BEGIN 8 */
    uint16_t i;
    
    for (i = 0; i < Len; i++)
        {
            UserTxBufferFS[i] = Buf[i];
        }
        
    USBD_CDC_SetTxBuffer (&USBD_Device, UserTxBufferFS, Len);
    result = USBD_CDC_TransmitPacket (&USBD_Device);
    /* USER CODE END 8 */
    return result;
}



/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

void USB_CDC_VCP_Init (void)
{
    /* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */
    /* USER CODE END USB_DEVICE_Init_PreTreatment */
    USBD_StatusTypeDef status = USBD_Init (&USBD_Device, &FS_Desc, 0);
    
    /* Init Device Library, add supported class and start the library. */
    if (status == USBD_OK)
        {
            status |= USBD_RegisterClass (&USBD_Device, &USBD_CDC);
            
            if (status == USBD_OK)
                {
                    status |= USBD_CDC_RegisterInterface (&USBD_Device, &USBD_Interface_fops_FS);
                    
                    if (status == USBD_OK)
                        {
                            status |= USBD_Start (&USBD_Device);
                        }
                }
        }
        
    /* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */
    /* USER CODE END USB_DEVICE_Init_PostTreatment */
}

void VCP_Reply_Short (uint8_t* message, uint8_t msg_len)
{
    uint32_t UID[3];
    uint8_t crc_bcd[4];
    uint32_t PAGEError = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct;
    HAL_StatusTypeDef status = HAL_ERROR;
    
    switch (message[0])
        {
            case 0x06: //ACK (ping)
                CDC_Transmit_FS (message, msg_len);
                break;
                
            case 0x8B : // Get mode
                message[0] = *run_mode & 0xFF;
                CDC_Transmit_FS (message, 1);
                break;
                
            case 0x8A: //Set mode
                if (msg_len > 1)
                    {
                        status = HAL_FLASH_Unlock();
                        __HAL_FLASH_CLEAR_FLAG (FLASH_FLAG_OPTVERR);
                        EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
                        EraseInitStruct.Banks       = FLASH_BANK_1;
                        EraseInitStruct.Page        = 127;
                        EraseInitStruct.NbPages     = 1;
                        status |= HAL_FLASHEx_Erase (&EraseInitStruct, &PAGEError);
                        status |= HAL_FLASH_Program (FLASH_TYPEPROGRAM_DOUBLEWORD, USER_FLASH_START, 0x0000000100000000 + message[1]);
                        status |= HAL_FLASH_Lock();
                    }
                    
                if (status == HAL_OK)
                    {
                        message[0] = 0x06; //ACK
                        CDC_Transmit_FS (message, 1);
                    }
                else
                    {
                        message[0] = 0x15; //NAK
                        CDC_Transmit_FS (message, 1);
                    }
                    
                break;
                
            case 0xE8:
                UID[0] = HAL_GetUIDw0();
                UID[1] = HAL_GetUIDw1();
                UID[2] = HAL_GetUIDw2();
                
                if (encode4byte (HAL_CRC_Calculate (&hcrc, UID, 12) % 10000, crc_bcd) == 0)
                    {
                        crc_bcd[0] += 0x30;
                        crc_bcd[1] += 0x30;
                        crc_bcd[2] += 0x30;
                        crc_bcd[3] += 0x30;
                        CDC_Transmit_FS ( (uint8_t*) &crc_bcd, 4);
                    }
                    
                break;
                
            case 'w':
                if (msg_len > ID_TAG_SIZE && (*run_mode & 0xFF) == 0x00)
                    {
                        memcpy ( (void*) customerID1, message, ID_TAG_SIZE + 1);
                        message[0] = 0x16; //SYNC
                        CDC_Transmit_FS (message, 1);
                    }
                else
                    {
                        message[0] = 0x15; //NAK
                        CDC_Transmit_FS (message, 1);
                    }
                    
                break;
                
            case 'W':
                if (msg_len > ID_TAG_SIZE && (*run_mode & 0xFF) == 0x00)
                    {
                        memcpy ( (void*) customerID2, message, ID_TAG_SIZE + 1);
                        message[0] = 0x16; //SYNC
                        CDC_Transmit_FS (message, 1);
                    }
                else
                    {
                        message[0] = 0x15; //NAK
                        CDC_Transmit_FS (message, 1);
                    }
                    
                break;
            case 0x04:
                HAL_NVIC_SystemReset();
                break;
                
            default:
                break;
        }
}


void VCP_Reply_Long (uint8_t* message, uint8_t msg_len)
{   
    switch (message[0])
        {
            case 'N':
                if (msg_len > 3 && (msg_len - 3) >= (message[2] + message[3]))
                    {
                        memcpy ( (void*) &ndef_data_add_to_tag, message + 1, 3 + message[2] + message [3]);
                        message[0] = 0x16; //SYNC
                        CDC_Transmit_FS (message, 1);
                    }
                else
                    {
                        message[0] = 0x15; //NAK
                        CDC_Transmit_FS (message, 1);
                    }
                    
                break; 
                
            default:
                break;
        }
}


/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
