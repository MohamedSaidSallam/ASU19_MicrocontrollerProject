#include "tm4c123gh6pm.h"
#include "fingerprint.h"
#include <stdio.h>
#include "systicktimerutil.h"

void init(void)
{
  systickTimerInit();
  initUart();

   genImg();
genImg();
genImg();
}

void loop(void)
{
  volatile int stupid = 0;
  volatile uint32_t x = search();
  while (1)
  {
    x = search();
    if(x==0)
      break;
  }
}

int main()
{
  init();
  loop();
  return 0;
}
