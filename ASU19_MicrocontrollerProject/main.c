#include "tm4c123gh6pm.h"
#include "fingerprint.h"
#include <stdio.h>
#include "systicktimerutil.h"

void init(void)
{
  systickTimerInit();
  initUart();

  verifyPassword();
}

void loop(void)
{
  volatile int stupid = 0;
  while (1)
  {
    // printf("%d\n", generateImage());
    // generateImage();
    delayMs(100);
    if (generateImage() == 0)
    {
      stupid++;
      if (stupid > 2)
      {
        break;
      }
    }
  }
}

int main()
{
  init();
  loop();
  return 0;
}
