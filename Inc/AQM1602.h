/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AQM1602_H
#define __AQM1602_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include system header files -----------------------------------------------*/
#include "stdint.h"
//#include "stdio.h"
//#include "stdarg.h"

/* Private macro -------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define AQM1602_CHARS_PER_LINE  16
#define AQM1602_LINES           2

#define AQM1602_DEV_ADDRESS     0x7C
#define AQM1602_CTRL_CMD        0x00
#define AQM1602_CTRL_DAT        0x40

/* Exported function macro ---------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported enum tag ---------------------------------------------------------*/
/* Exported struct/union tag -------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
void AQM1602_init(void);
void AQM1602_clear(void);
void AQM1602_locate(uint8_t, uint8_t);
void AQM1602_puts(const char*);
void AQM1602_putsLine(uint8_t, const char*);
void AQM1602_putsLine_IT(uint8_t, const char*);

void I2C2_TxCpltCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* __AQM1602_H */

/***************************************************************END OF FILE****/
