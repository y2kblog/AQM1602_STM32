/* Includes ------------------------------------------------------------------*/
#include "AQM1602.h"
#include "i2c.h"
#include "string.h"

/* Imported variables --------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private enum tag ----------------------------------------------------------*/
/* Private struct/union tag --------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void AQM1602_send(uint8_t, uint8_t);
static void AQM1602_send_IT(uint8_t *, uint8_t *, size_t);

/* Exported functions --------------------------------------------------------*/
void AQM1602_init(void)
{
    HAL_Delay(50);     // Wait > 40ms
    AQM1602_send(AQM1602_CTRL_CMD, 0x38);   // Function set = 0x38
    HAL_Delay(1);       // Wait > 26.3us
    AQM1602_send(AQM1602_CTRL_CMD, 0x39);   // Function set = 0x39
    HAL_Delay(1);       // Wait > 26.3us
    AQM1602_send(AQM1602_CTRL_CMD, 0x14);   // Internal OSC frequency = 0x14
    HAL_Delay(1);       // Wait > 26.3us
    AQM1602_send(AQM1602_CTRL_CMD, 0x70);   // Contrast set = 0x70
    HAL_Delay(1);       // Wait > 26.3us
    AQM1602_send(AQM1602_CTRL_CMD, 0x5E);   // Power/ICON/Contrast control = 0x56
    HAL_Delay(1);       // Wait > 26.3us
    AQM1602_send(AQM1602_CTRL_CMD, 0x6C);   // Follower control = 0x6C
    HAL_Delay(250);     // Wait > 200ms
    AQM1602_send(AQM1602_CTRL_CMD, 0x38);   // Function set = 0x38
    HAL_Delay(1);       // Wait > 26.3us
    AQM1602_send(AQM1602_CTRL_CMD, 0x0C);   // Display ON/OFF control = 0x0C
    HAL_Delay(1);       // Wait > 26.3us
    AQM1602_send(AQM1602_CTRL_CMD, 0x01);   // Clear Display = 0x01
    HAL_Delay(2);       // Wait > 1.08ms
}


void AQM1602_clear(void)
{
    AQM1602_send(AQM1602_CTRL_CMD, 0x01);   // Clear Display = 0x01
    HAL_Delay(2);       // Wait > 1.08ms
}


void AQM1602_locate(uint8_t x, uint8_t y)
{
    AQM1602_send(AQM1602_CTRL_CMD, 0x80 + y*0x40 + x);
}


void AQM1602_puts(const char* str)
{
    while (*str != '\0')
    {
        AQM1602_send(AQM1602_CTRL_DAT, *str);
        str++;
    }
}


void AQM1602_putsLine(uint8_t line, const char* str)
{
    if( (line == 0) || (line > AQM1602_LINES) )
    {
        printf("AQM1602_putsLine error : 1 <= line <= %d\r\n", AQM1602_LINES);
        return;
    }

    AQM1602_locate(line-1, 0);

    uint8_t DataBytes[1 + AQM1602_CHARS_PER_LINE];
    size_t len = strlen(str);
    if (len > AQM1602_CHARS_PER_LINE)
        len = AQM1602_CHARS_PER_LINE;

    for (size_t i = 0; i < len; ++i)
    {
        DataBytes[i] = *(str + i);
    }

    // fill space
    for (size_t i = len; i < AQM1602_CHARS_PER_LINE; ++i)
    {
        DataBytes[i] = ' ';
    }

    // Final byte is '\0'
    DataBytes[AQM1602_CHARS_PER_LINE] = '\0';

    AQM1602_puts(DataBytes);
}


void AQM1602_putsLine_IT(uint8_t line, const char* str)
{
    if( (line == 0) || (line > AQM1602_LINES) )
    {
        printf("AQM1602_putsLine_IT error : 1 <= line <= %d\r\n", AQM1602_LINES);
        return;
    }

    uint8_t CtrlBytes[1 + AQM1602_CHARS_PER_LINE];
    uint8_t DataBytes[1 + AQM1602_CHARS_PER_LINE];

    // Locate cursor
    CtrlBytes[0] = AQM1602_CTRL_CMD;
    DataBytes[0] = 0x80 + (line - 1);

    size_t len = strlen(str);
    if (len > AQM1602_CHARS_PER_LINE)
        len = AQM1602_CHARS_PER_LINE;

    for (size_t i = 0; i < len; ++i)
    {
        CtrlBytes[1 + i] = AQM1602_CTRL_DAT;
        DataBytes[1 + i] = *(str + i);
    }

    // fill space
    for (size_t i = len; i < AQM1602_CHARS_PER_LINE; ++i)
    {
        CtrlBytes[1 + i] = AQM1602_CTRL_DAT;
        DataBytes[1 + i] = ' ';
    }

    AQM1602_send_IT((uint8_t *) CtrlBytes, (uint8_t *) DataBytes, 1 + AQM1602_CHARS_PER_LINE);
}


// callback function after sending data of AQM1602_send_IT()
void I2C2_TxCpltCallback(void)
{

}


/* Private functions ---------------------------------------------------------*/
static void AQM1602_send(uint8_t CtrlByteWithoutCo, uint8_t DataByte)
{
    volatile uint8_t data[2];
    data[0] = CtrlByteWithoutCo;
    data[1] = DataByte;

    HAL_StatusTypeDef status;
    status = HAL_I2C_Master_Transmit(&hi2c2, AQM1602_DEV_ADDRESS, (uint8_t *) data, 2, 0xFFFFFFFF);
    if (status != HAL_OK)
    {
        printf("HAL_I2C_Master_Transmit error : %d\r\n", status);
        //Error_Handler();
    }
}

static void AQM1602_send_IT(uint8_t *CtrlBytesWithoutCo, uint8_t *DataBytes, size_t BytesLength)
{
    if (BytesLength > (1 + AQM1602_CHARS_PER_LINE))
    {
        printf("AQM1602_send_IT error : BytesLength must below %d\r\n", 1 + AQM1602_CHARS_PER_LINE);
        return;
    }

    static volatile uint8_t data[2 * (1 + AQM1602_CHARS_PER_LINE)];
    for (size_t i = 0; i < BytesLength; ++i)
    {
        data[2 * i] = *(CtrlBytesWithoutCo + i) | 0x80;  // Add "Co" bit
        data[2 * i + 1] = *(DataBytes + i);
    }
    data[2 * (BytesLength - 1)] &= 0x7F;    // Mask "Co" bit for last control byte

    HAL_StatusTypeDef status;
    status = HAL_I2C_Master_Transmit_IT(&hi2c2, AQM1602_DEV_ADDRESS, (uint8_t *) data, 2 * BytesLength);
    if (status != HAL_OK)
    {
        printf("HAL_I2C_Master_Transmit_IT error : %d\r\n", status);
        //Error_Handler();
    }
}

/***************************************************************END OF FILE****/
