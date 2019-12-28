//credits: http://users.ece.utexas.edu/~valvano/arm/

//***********************  main.c  ***********************
// Program written by:
// - Steven Prickett  steven.prickett@gmail.com
//
// Brief desicription of program:
// - Initializes an ESP8266 module to act as a WiFi client
//   and fetch weather data from openweathermap.org
//
//*********************************************************
/* Modified by Jonathan Valvano
 Sept 14, 2016
 Out of the box: to make this work you must
 Step 1) Set parameters of your AP in lines 59-60 of esp8266.c
 Step 2) Change line 39 with directions in lines 40-42
 Step 3) Run a terminal emulator like Putty or TExasDisplay at
         115200 bits/sec, 8 bit, 1 stop, no flow control
 Step 4) Set line 50 to match baud rate of your ESP8266 (9600 or 115200)
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "tm4c123gh6pm.h"

#include "esp8266.h"
#include "interrupts.h"

char Fetch[] = "GET /registerAttendance?id=5 HTTP/1.1\r\nHost: us-central1-microcontrollerproject-69ca4.cloudfunctions.net\r\n\r\n";

int main(void)
{
  // DisableInterrupts();
  // //Output_Init(); // UART0 only used for debugging
  // ESP8266_Init(115200); // connect to access point, set up as client
  // ESP8266_GetVersionNumber();
  // // while (1)
  // // {
  //   ESP8266_GetStatus();
  //   if (ESP8266_MakeTCPConnection("us-central1-microcontrollerproject-69ca4.cloudfunctions.net"))
  //   { // open socket in server
  //     ESP8266_SendTCP(Fetch);
  //   }
  //   ESP8266_CloseTCPConnection();
  // // }

  miniInit();
  theOneCommandToRuleThemAll();
}
