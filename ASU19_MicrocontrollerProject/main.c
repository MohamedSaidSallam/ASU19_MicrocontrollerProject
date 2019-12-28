#include "tm4c123gh6pm.h"
#include "lcd.h"
#include "systicktimerutil.h"
#include "builtinswitchutil.h"
#include "fingerprint.h"


void init();
void loop();

void clearLCD();

void addFingerprint();
void searchForUser();

int main()
{
  init();
  LCD_word("S1 enroll S2 search",19);
  loop();
}
void init(){
  lcdInit();
  systickTimerInit();
  builtin_switch_init();
  set_sw1_function(&addFingerprint);
  set_sw2_function(&searchForUser);
}
void loop(){
  while(1){
    do_builtin_switch_functions();
  }
}
void clearLCD(){
  lcd_clear();
  lcd_cursor_first_line();
}
void addFingerprint(){
  clearLCD();
  LCD_word("put your finger,remove then put",32);
}
void searchForUser(){
  clearLCD();
  int id =0;
  if(id == -1){
    LCD_word("User not found.",15);
  }
  else{
    LCD_word("User found.",11);
  }
}