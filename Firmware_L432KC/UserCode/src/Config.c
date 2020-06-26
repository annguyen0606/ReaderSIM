

/* Includes ------------------------------------------------------------------*/
#include "LIB_Config.h"

static void driver_init(void);

/* Private functions ---------------------------------------------------------*/


/**
  * @brief  System initialization.
  * @param  None
  * @retval  None
  */
void OLED_init(void)
{
  driver_init();
}

static void driver_init(void)
{
  ssd1306_init();
}
