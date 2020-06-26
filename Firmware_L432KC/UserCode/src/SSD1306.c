

/* Includes ------------------------------------------------------------------*/
#include "LIB_Config.h"
#include "SSD1306.h"
#include "Fonts.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SSD1306_CMD    0
#define SSD1306_DAT    1

#define SSD1306_WIDTH    128
#define SSD1306_HEIGHT   64
extern I2C_HandleTypeDef hi2c1;

/* Private macro -------------------------------------------------------------*/




#define __SET_COL_START_ADDR() 	do { \
									ssd1306_write_byte(0x02, SSD1306_CMD); \
									ssd1306_write_byte(0x10, SSD1306_CMD); \
								} while(false)
							
/* Private variables ---------------------------------------------------------*/
static uint8_t s_chDispalyBuffer[128][8];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  Writes an byte to the display data ram or the command register
  *         
  * @param  chData: Data to be writen to the display data ram or the command register
  * @param chCmd:  
  *                           0: Writes to the command register
  *                           1: Writes to the display data ram
  * @retval None
**/
static void ssd1306_write_byte(uint8_t chData, uint8_t chCmd) 
{
        uint8_t data_t[2];
	if (chCmd) {
                data_t[0] = 0x40;  //en=1, rs=0
	} else {
		data_t[0] = 0x00;  //en=1, rs=0
	}
        data_t[1] = chData;  //en=0, rs=0
        HAL_I2C_Master_Transmit (&hi2c1, 0x78,(uint8_t *) data_t, 2, 100);
}   	  

/**
  * @brief  OLED turns on 
  *         
  * @param  None
  *         
  * @retval None
**/ 
void ssd1306_display_on(void)
{
	ssd1306_write_byte(0x8D, SSD1306_CMD);  
	ssd1306_write_byte(0x14, SSD1306_CMD);  
	ssd1306_write_byte(0xAF, SSD1306_CMD);  
}
   
/**
  * @brief  OLED turns off
  *         
  * @param  None
  *         
  * @retval  None
**/
void ssd1306_display_off(void)
{
	ssd1306_write_byte(0x8D, SSD1306_CMD);  
	ssd1306_write_byte(0x10, SSD1306_CMD); 
	ssd1306_write_byte(0xAE, SSD1306_CMD);  
}

/**
  * @brief  Refreshs the graphic ram
  *         
  * @param  None
  *         
  * @retval  None
**/

void ssd1306_refresh_gram(void)
{
	uint8_t i, j;
	
	for (i = 0; i < 8; i ++) {  
		ssd1306_write_byte(0xB0 + i, SSD1306_CMD);    
		__SET_COL_START_ADDR();      
		for (j = 0; j < 128; j ++) {
			ssd1306_write_byte(s_chDispalyBuffer[j][i], SSD1306_DAT); 
		}
	}   
}


/**
  * @brief   Clears the screen
  *         
  * @param  None
  *         
  * @retval  None
**/

void ssd1306_clear_screen(uint8_t chFill)  
{ 
	uint8_t i, j;
	
	for (i = 0; i < 8; i ++) {
		ssd1306_write_byte(0xB0 + i, SSD1306_CMD);
		__SET_COL_START_ADDR();
		for (j = 0; j < 128; j ++) {
			s_chDispalyBuffer[j][i] = chFill;
		}
	}
	
	ssd1306_refresh_gram();
}

/**
  * @brief  Draws a piont on the screen
  *         
  * @param  chXpos: Specifies the X position
  * @param  chYpos: Specifies the Y position
  * @param  chPoint: 0: the point turns off    1: the piont turns on 
  *         
  * @retval None
**/

void ssd1306_draw_point(uint8_t chXpos, uint8_t chYpos, uint8_t chPoint)
{
	uint8_t chPos, chBx, chTemp = 0;
	
	if (chXpos > 127 || chYpos > 63) {
		return;
	}
	chPos = 7 - chYpos / 8; // 
	chBx = chYpos % 8;
	chTemp = 1 << (7 - chBx);
	
	if (chPoint) {
		s_chDispalyBuffer[chXpos][chPos] |= chTemp;
		
	} else {
		s_chDispalyBuffer[chXpos][chPos] &= ~chTemp;
	}
}
	  
/**
  * @brief  Fills a rectangle
  *         
  * @param  chXpos1: Specifies the X position 1 (X top left position)
  * @param  chYpos1: Specifies the Y position 1 (Y top left position)
  * @param  chXpos2: Specifies the X position 2 (X bottom right position)
  * @param  chYpos3: Specifies the Y position 2 (Y bottom right position)
  *         
  * @retval 
**/

void ssd1306_fill_screen(uint8_t chXpos1, uint8_t chYpos1, uint8_t chXpos2, uint8_t chYpos2, uint8_t chDot)  
{  
	uint8_t chXpos, chYpos; 
	
	for (chXpos = chXpos1; chXpos <= chXpos2; chXpos ++) {
		for (chYpos = chYpos1; chYpos <= chYpos2; chYpos ++) {
			ssd1306_draw_point(chXpos, chYpos, chDot);
		}
	}	
	
	ssd1306_refresh_gram();
}


/**
  * @brief Displays one character at the specified position    
  *         
  * @param  chXpos: Specifies the X position
  * @param  chYpos: Specifies the Y position
  * @param  chSize: 
  * @param  chMode
  * @retval 
**/
void ssd1306_display_char(uint8_t chXpos, uint8_t chYpos, uint8_t chChr, uint8_t chSize, uint8_t chMode)
{      	
	uint8_t i, j;
	uint8_t chTemp, chYpos0 = chYpos;
	
	chChr = chChr - ' ';				   
    for (i = 0; i < chSize; i ++) {   
		if (chSize == 12) {
			if (chMode) {
				chTemp = c_chFont1206[chChr][i];
			} else {
				chTemp = ~c_chFont1206[chChr][i];
			}
		} else {
			if (chMode) {
				chTemp = c_chFont1608[chChr][i];
			} else {
				chTemp = ~c_chFont1608[chChr][i];
			}
		}
		
        for (j = 0; j < 8; j ++) {
			if (chTemp & 0x80) {
				ssd1306_draw_point(chXpos, chYpos, 1);
			} else {
				ssd1306_draw_point(chXpos, chYpos, 0);
			}
			chTemp <<= 1;
			chYpos ++;
			
			if ((chYpos - chYpos0) == chSize) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}  	 
    } 
}
static uint32_t pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;	 
	while(n --) result *= m;    
	return result;
}	


void ssd1306_display_num(uint8_t chXpos, uint8_t chYpos, uint32_t chNum, uint8_t chLen, uint8_t chSize)
{         	
	uint8_t i;
	uint8_t chTemp, chShow = 0;
	
	for(i = 0; i < chLen; i ++) {
		chTemp = (chNum / pow(10, chLen - i - 1)) % 10;
		if(chShow == 0 && i < (chLen - 1)) {
			if(chTemp == 0) {
				ssd1306_display_char(chXpos + (chSize / 2) * i, chYpos, ' ', chSize, 1);
				continue;
			} else {
				chShow = 1;
			}	 
		}
	 	ssd1306_display_char(chXpos + (chSize / 2) * i, chYpos, chTemp + '0', chSize, 1); 
	}
} 


/**
  * @brief  Displays a string on the screen
  *         
  * @param  chXpos: Specifies the X position
  * @param  chYpos: Specifies the Y position
  * @param  pchString: Pointer to a string to display on the screen 
  *         
  * @retval  None
**/
void ssd1306_display_string(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchString, uint8_t chSize, uint8_t chMode)
{
    while (*pchString != '\0') {       
        if (chXpos > (SSD1306_WIDTH - chSize / 2)) {
			chXpos = 0;
			chYpos += chSize;
			if (chYpos > (SSD1306_HEIGHT - chSize)) {
				chYpos = chXpos = 0;
				ssd1306_clear_screen(0x00);
			}
		}
		
        ssd1306_display_char(chXpos, chYpos, *pchString, chSize, chMode);
        chXpos += chSize / 2;
        pchString ++;
    }
}

void ssd1306_draw_1616char(uint8_t chXpos, uint8_t chYpos, uint8_t chChar)
{
	uint8_t i, j;
	uint8_t chTemp = 0, chYpos0 = chYpos, chMode = 0;

	for (i = 0; i < 32; i ++) {
		chTemp = c_chFont1612[chChar - 0x30][i];
		for (j = 0; j < 8; j ++) {
			chMode = chTemp & 0x80? 1 : 0; 
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chYpos ++;
			if ((chYpos - chYpos0) == 16) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

void ssd1306_draw_3216char(uint8_t chXpos, uint8_t chYpos, uint8_t chChar)
{
	uint8_t i, j;
	uint8_t chTemp = 0, chYpos0 = chYpos, chMode = 0;

	for (i = 0; i < 64; i ++) {
		chTemp = c_chFont3216[chChar - 0x30][i];
		for (j = 0; j < 8; j ++) {
			chMode = chTemp & 0x80? 1 : 0; 
			ssd1306_draw_point(chXpos, chYpos, chMode);
			chTemp <<= 1;
			chYpos ++;
			if ((chYpos - chYpos0) == 32) {
				chYpos = chYpos0;
				chXpos ++;
				break;
			}
		}
	}
}

void ssd1306_draw_bitmap(uint8_t chXpos, uint8_t chYpos, const uint8_t *pchBmp, uint8_t chWidth, uint8_t chHeight)
{
	uint16_t i, j, byteWidth = (chWidth + 7) / 8;
	
    for(j = 0; j < chHeight; j ++){
        for(i = 0; i < chWidth; i ++ ) {
            if(*(pchBmp + j * byteWidth + i / 8) & (128 >> (i & 7))) {
                ssd1306_draw_point(chXpos + i, chYpos + j, 1);
            }
        }
    }
}



/**
  * @brief  SSd1306 initialization
  *         
  * @param  None
  *         
  * @retval None
**/
void ssd1306_init(void)
{


	ssd1306_write_byte(0xAE, SSD1306_CMD);//--turn off oled panel
        HAL_Delay(1);
	ssd1306_write_byte(0x00, SSD1306_CMD);//---set low column address
        HAL_Delay(1);
	ssd1306_write_byte(0x10, SSD1306_CMD);//---set high column address
        HAL_Delay(1);
	ssd1306_write_byte(0x40, SSD1306_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
        HAL_Delay(1);
	ssd1306_write_byte(0x81, SSD1306_CMD);//--set contrast control register
        HAL_Delay(1);
	ssd1306_write_byte(0xCF, SSD1306_CMD);// Set SEG Output Current Brightness
        HAL_Delay(1);
	ssd1306_write_byte(0xA1, SSD1306_CMD);//--Set SEG/Column Mapping     
        HAL_Delay(1);
	ssd1306_write_byte(0xC0, SSD1306_CMD);//Set COM/Row Scan Direction   
        HAL_Delay(1);
	ssd1306_write_byte(0xA6, SSD1306_CMD);//--set normal display
        HAL_Delay(1);
	ssd1306_write_byte(0xA8, SSD1306_CMD);//--set multiplex ratio(1 to 64)
        HAL_Delay(1);
	ssd1306_write_byte(0x3f, SSD1306_CMD);//--1/64 duty
        HAL_Delay(1);
	ssd1306_write_byte(0xD3, SSD1306_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
        HAL_Delay(1);
	ssd1306_write_byte(0x00, SSD1306_CMD);//-not offset
        HAL_Delay(1);
	ssd1306_write_byte(0xd5, SSD1306_CMD);//--set display clock divide ratio/oscillator frequency
        HAL_Delay(1);
	ssd1306_write_byte(0x80, SSD1306_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
        HAL_Delay(1);
	ssd1306_write_byte(0xD9, SSD1306_CMD);//--set pre-charge period
        HAL_Delay(1);
	ssd1306_write_byte(0xF1, SSD1306_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
        HAL_Delay(1);
	ssd1306_write_byte(0xDA, SSD1306_CMD);//--set com pins hardware configuration
        HAL_Delay(1);
	ssd1306_write_byte(0x12, SSD1306_CMD);
        HAL_Delay(1);
	ssd1306_write_byte(0xDB, SSD1306_CMD);//--set vcomh
        HAL_Delay(1);
	ssd1306_write_byte(0x40, SSD1306_CMD);//Set VCOM Deselect Level
        HAL_Delay(1);
	ssd1306_write_byte(0x20, SSD1306_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
        HAL_Delay(1);
	ssd1306_write_byte(0x02, SSD1306_CMD);//
        HAL_Delay(1);
	ssd1306_write_byte(0x8D, SSD1306_CMD);//--set Charge Pump enable/disable
        HAL_Delay(1);
	ssd1306_write_byte(0x14, SSD1306_CMD);//--set(0x10) disable
        HAL_Delay(1);
	ssd1306_write_byte(0xA4, SSD1306_CMD);// Disable Entire Display On (0xa4/0xa5)
        HAL_Delay(1);
	ssd1306_write_byte(0xA6, SSD1306_CMD);// Disable Inverse Display On (0xa6/a7) 
        HAL_Delay(1);
	ssd1306_write_byte(0xAF, SSD1306_CMD);//--turn on oled panel
	HAL_Delay(1);
	ssd1306_clear_screen(0x00);
}

/*-------------------------------END OF FILE-------------------------------*/

