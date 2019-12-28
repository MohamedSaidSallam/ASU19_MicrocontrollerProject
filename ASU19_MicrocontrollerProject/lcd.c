#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "systicktimerutil.h"
#include <math.h>

#define LCD_COMMAND_CLEAR 0x1                 // Clear display screen
#define LCD_COMMAND_RETURN_CURSOR 0x2         // Return cursor home
#define LCD_COMMAND_INC_CURSOR 0x6            // Increment cursor (shift cursor to right)
#define LCD_COMMAND_DISPLAY_ON 0xF            // Display on, cursor blinking
#define LCD_COMMAND_DISPLAY_ON_CURSOR_OFF 0xC // Display on, cursor off
#define LCD_COMMAND_CURSOR_FIRST_LINE 0x80    // Force cursor to beginning of 1st line
#define LCD_COMMAND_CURSOR_SECOND_LINE 0xC0   // Force cursor to beginning of 2nd line
#define LCD_COMMAND_8_BIT 0x38                // 2 lines and 5x7 character (8-bit data, D0 to D7)
#define LCD_COMMAND_4_BIT 0x28                // 2 lines and 5x7 character (4-bit data, D4 to D7)
#define LCD_COMMAND_WAKEUP 0x30               // wakeup

#define LCD_CTRL_ZERO 0
#define LCD_RS 0x1 << 4// 0000
#define LCD_RW 0x2 << 4// 0010
#define LCD_EN 0x4 << 4// 0100

#define LCD_CTRL_CLK SYSCTL_RCGCGPIO_R2
#define LCD_CTRL_DIR GPIO_PORTC_DIR_R
#define LCD_CTRL_DEN GPIO_PORTC_DEN_R
#define LCD_DATA_CLK SYSCTL_RCGCGPIO_R4
#define LCD_DATA_CLK2 SYSCTL_RCGCGPIO_R3
#define LCD_DATA_DIR GPIO_PORTE_DIR_R
#define LCD_DATA_DIR2 GPIO_PORTD_DIR_R
#define LCD_DATA_DEN GPIO_PORTE_DEN_R
#define LCD_DATA_DEN2 GPIO_PORTD_DEN_R
#define LCD_CTRL_DATA GPIO_PORTC_DATA_R
#define LCD_DATA_DATA GPIO_PORTE_DATA_R
#define LCD_DATA_DATA2 GPIO_PORTD_DATA_R

#define MASK_FIRST_FOUR_BITS 0x0F
#define MASK_LAST_FOUR_BITS 0xF0

#define DISPLAY_FLOAT_PRECISION 100
#define DISPLAY_FLOAT_SCALE 100

int inverseNum(int num);
void lcd_displayDigit(int digit);
void LCD_command(unsigned char command);

void LCD_data(unsigned char data)
{
    LCD_CTRL_DATA = LCD_RS;
    LCD_DATA_DATA = data & MASK_FIRST_FOUR_BITS ;
    LCD_DATA_DATA2 = (data & MASK_LAST_FOUR_BITS) >> 4;
    LCD_CTRL_DATA = LCD_EN | LCD_RS; /* pulse E */
    delayUs(1);
    LCD_CTRL_DATA = LCD_CTRL_ZERO;
    delayUs(40);
}

void LCD_word(char *word, uint8_t size)
{
    LCD_command(LCD_COMMAND_CLEAR);
    for (int i = 0; i < size; i++)
    {
        LCD_data(*word++);
    }
}

void initDataPort(void)
{
    SYSCTL_RCGCGPIO_R |= (LCD_DATA_CLK | LCD_DATA_CLK2);
    while (!(SYSCTL_PRGPIO_R & (LCD_DATA_CLK | LCD_DATA_CLK2)))
    {
    }
    LCD_DATA_DIR |= 0x0F;
    LCD_DATA_DIR2 |= 0x0F;
    LCD_DATA_DEN |= 0x0F;
    LCD_DATA_DEN2 |= 0x0F;
    LCD_DATA_DATA = 0;
    LCD_DATA_DATA2 = 0;
}

void initCtrlPort(void)
{
    SYSCTL_RCGCGPIO_R |= LCD_CTRL_CLK;
    while (!(SYSCTL_PRGPIO_R & LCD_CTRL_CLK))
    {
    }
    LCD_CTRL_DEN |= 0x70; // XX00 0111
    LCD_CTRL_DIR |= 0x70; // XX00 0111
    LCD_CTRL_DATA = 0;
}

void LCD_start(void)
{
    delayMs(20); // Wait >15 ms after power is applied
    /* initialization sequence */
    LCD_command(LCD_COMMAND_WAKEUP);
    delayMs(5); // must wait 5ms, busy flag not available
    LCD_command(LCD_COMMAND_WAKEUP);
    delayUs(100); // must wait 100us, busy flag not available
    LCD_command(LCD_COMMAND_WAKEUP);
    delayUs(100); // must wait 100us, busy flag not available
    /* Prepare LCD Operation and Function */
    LCD_command(LCD_COMMAND_8_BIT);
    LCD_command(LCD_COMMAND_INC_CURSOR);
    LCD_command(LCD_COMMAND_CLEAR);
    LCD_command(LCD_COMMAND_DISPLAY_ON);
}

void LCD_command(unsigned char command)
{
    LCD_CTRL_DATA = LCD_CTRL_ZERO;
    LCD_DATA_DATA = command & MASK_FIRST_FOUR_BITS ;
    LCD_DATA_DATA2 = (command & MASK_LAST_FOUR_BITS) >> 4;
    LCD_CTRL_DATA = LCD_EN; /* pulse E */
    delayUs(1);             /* Enable pulse Width */
    LCD_CTRL_DATA = LCD_CTRL_ZERO;
    if (command > LCD_COMMAND_RETURN_CURSOR)
        delayUs(50); /* all others 50 us */
    else
        delayMs(2); /* command 1 and 2 needs up to 1.64ms */
}

void lcd_clear(void)
{
    LCD_command(LCD_COMMAND_CLEAR);
}

void lcd_cursor_first_line(void)
{
    LCD_command(LCD_COMMAND_CURSOR_FIRST_LINE);
}

void lcd_display_on_cursor_off(void)
{
    LCD_command(LCD_COMMAND_DISPLAY_ON_CURSOR_OFF);
}

void lcdDisplayNum(uint32_t num)
{
    if (num == 0)
    {
        lcd_displayDigit(0);
        return;
    }

    num = inverseNum(num);
    uint8_t numSize = ceil(log10(num + 1));
    for (int i = 0; i < numSize; i++)
    {
        lcd_displayDigit(num % 10);
        num /= 10;
    }
}

void lcd_displayNumZeroPadded(int num, int numOfDigits)
{
    if (num < 10)
    {
        lcd_displayDigit(0);
        lcd_displayDigit(num);
    }
    else
    {
        num = inverseNum(num);
        for (int i = 0; i < numOfDigits; i++)
        {
            lcd_displayDigit(num % 10);
            num /= 10;
        }
    }
}

void lcd_displayIntAsFloat(int num)
{
    lcd_displayNumZeroPadded(num / DISPLAY_FLOAT_PRECISION, 2);
    LCD_data('.');
    lcd_displayNumZeroPadded(num % DISPLAY_FLOAT_SCALE, 2);
}

void lcd_displayDigit(int digit)
{
    char my_char = '0' + (char)(digit);
    LCD_data(my_char);
}

int inverseNum(int num)
{
    int temp = 0;
    while (num != 0)
    {
        temp = temp * 10 + num % 10;
        num /= 10;
    }
    return temp;
}
void lcdInit()
{
    initDataPort();
    initCtrlPort();
    systickTimerInit();
    // oneshotTimerInit(); //todo :( im not sad im just drawn this way :(((
    LCD_start();
}