#include "tm4c123gh6pm.h"
#include "fingerprint.h"
int main()
{
  initUart();
  while(1)
    generateImage();
  return 0;
}
