#include "tm4c123gh6pm.h"

#include "esp8266.h"
#include "fingerprint.h"
#include "systicktimerutil.h"
#include <stdint.h>
#include "lcd.h"
#include "builtinswitchutil.h"
#include "periodicinterruptutil.h"

int enrolling = 0;
void enrollUser(void)
{
  enrolling = 1;
}

void nullOp()
{
}

void init()
{
  initWifi();
  systickTimerInit();
  initUart();
  lcdInit();
  builtin_switch_init();
  set_sw1_function(&enrollUser);
  set_sw2_function(&nullOp);

  lcd_clear();
  lcd_cursor_first_line();

  LCD_word("started", 7);
}

void loop()
{
  while (1)
  {
    do_builtin_switch_functions();
    if (enrolling)
    {
      lcd_clear();
      lcd_cursor_first_line();
      enrolling = 0;
      LCD_word("enrolling", 9);
      enroll(13);
      registerOnCloud(13);
      lcd_clear();
      lcd_cursor_first_line();
      LCD_word("enrolled", 8);
    }
  }
}

int main(void)
{
  init();
  loop();
}
