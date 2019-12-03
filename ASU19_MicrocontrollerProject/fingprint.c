#include "tm4c123gh6pm.h"
//#include "fingerprint.h"
#define HEADER 0xEF01
#define ADDRESS 0xFFFFFFFF
#define COMMAND_PACKET_ID 0x01
#define BAUD 57600

#include <stdint.h>

//uart 2
#define UART_CTL UART2_CTL_R
#define UART_FBRD UART2_FBRD_R
#define UART_IBRD UART2_IBRD_R
#define UART_LCRH UART2_LCRH_R
#define UART_IFLS UART2_IFLS_R
#define UART_IM UART2_IM_R
#define UART_CTL UART2_CTL_R
#define UART_DATA UART2_DR_R
#define UART_FR UART2_FR_R

//pd6 7
#define GPIO_PORT_AFSEL GPIO_PORTD_AFSEL_R
#define GPIO_PORT_PCTL GPIO_PORTD_PCTL_R
#define GPIO_PCTL_PINS 0x11000000
#define GPIO_PCTL_IGNORED_PINS 0x00FFFFFF

#define UART_ENABLE 0x2 //uart2
#define PORT_ENABLE 0x4 //port D

#define FINGERPRINT_GENIMG 0x01
#define FINGERPRINT_IMAGE2TZ 0x02
#define FINGERPRINT_MATCH 0x03
#define FINGERPRINT_SEARCH 0x04
#define FINGERPRINT_REGMODEL 0x05
#define FINGERPRINT_STORE 0x06
#define FINGERPRINT_LOADCHAR 0x07
#define FINGERPRINT_DELETECHAR 0xC
#define FINGERPRINT_EMPTYLIBRARY 0xD
#define FINGERPRINT_HISPEEDSEARCH 0x1B

#define FINGERPRINT_ACKPACKET 0x07

void initUart()
{
  SYSCTL_RCGCUART_R |= UART_ENABLE; //Enable Uart2
  while ((SYSCTL_PRUART_R & UART_ENABLE) == 0)
  {
  };
  SYSCTL_RCGCGPIO_R |= PORT_ENABLE;  //Enable port clock
  //todo check afsel for pd6 pd7
  GPIO_PORT_AFSEL |= 0XC0; //alternate function for pD6 pD7 //todo check //done
  GPIO_PORT_PCTL = (GPIO_PORT_PCTL & GPIO_PCTL_IGNORED_PINS) | GPIO_PCTL_PINS; //todo define
  UART_CTL &= ~UART_CTL_UARTEN; //clear UART enable bit during config
  UART_FBRD = ((64 * ((16000000 / 16) % BAUD)) + BAUD / 2) / BAUD;
  UART_IBRD = 16000000 / 16 / BAUD;
  UART_LCRH = (UART_LCRH_WLEN_8 | UART_LCRH_FEN); //8 bit word length , 1 stop , no parity
  UART_IFLS &= ~0x3F;  //todo check                           // msh fahma //leh 3amlin clear ly bit 5 w hya reserved
  UART_IFLS += UART_IFLS_RX1_8;                   // 1/8 //todo typo
  UART_IM |= UART_IM_RXIM | UART_IM_RTIM;         //should we enable interrupt ?
  UART_CTL |= UART_CTL_UARTEN;                    //enable bit
}

//todo match not implemented
//todo implement replies

uint8_t generateImage(void) { 
  uint8_t packet[] = {FINGERPRINT_GENIMG};
  r307sendcommand(sizeof(packet)+2, packet);
  
  // uint8_t len = getReply(packet);
  // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;
  // return packet[1];
}

// generate charactar file from image buffer
// store it in char buffer with id # slot
uint8_t image2Tz(uint8_t slot) {
  uint8_t packet[] = {FINGERPRINT_IMAGE2TZ, slot};
  r307sendcommand(sizeof(packet)+2, packet);

  // uint8_t len = getReply(packet);  
  // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;
  // return packet[1];
}

// store char buffer 1 in library at #id
uint8_t storeModel(uint16_t id) {
  uint8_t packet[] = {FINGERPRINT_STORE, 0x01, id >> 8, id & 0xFF};
  r307sendcommand(sizeof(packet)+2, packet);
  
  // uint8_t len = getReply(packet);
  // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;
  // return packet[1];
}

// load char at library # id to char buffer #1
uint8_t loadChar(uint16_t id) {
    uint8_t packet[] = {FINGERPRINT_LOADCHAR, 0x01, id >> 8, id & 0xFF};
    r307sendcommand(sizeof(packet)+2, packet);
    
    // uint8_t len = getReply(packet);
    // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
        return -1;
    // return packet[1];
}

// combine char 1, 2 and put result in them both
uint8_t createModel(void) {
  uint8_t packet[] = {FINGERPRINT_REGMODEL};
  r307sendcommand(sizeof(packet)+2, packet);
  
  // uint8_t len = getReply(packet);
  // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;
  // return packet[1];
}

uint8_t deleteModel(uint16_t id) {
    uint8_t packet[] = {FINGERPRINT_DELETECHAR, id >> 8, id & 0xFF, 0x00, 0x01};
    r307sendcommand(sizeof(packet)+2, packet);

    // uint8_t len = getReply(packet);
    // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
        return -1;
    // return packet[1];
}

uint8_t emptyDatabase(void) {
  uint8_t packet[] = {FINGERPRINT_EMPTYLIBRARY};
  r307sendcommand(sizeof(packet)+2, packet);
  // uint8_t len = getReply(packet);
  
  // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;
  // return packet[1];
}

uint8_t fingerFastSearch(void) {
  uint16_t fingerID = 0xFFFF;
  uint16_t confidence = 0xFFFF;
  // high speed search of slot #1 starting at page 0x0000 and ending at page #0x00A3 
  uint8_t packet[] = {FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3};
  r307sendcommand(sizeof(packet)+2, packet);
  // uint8_t len = getReply(packet);
  
  // if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
   return -1;

  // fingerID = packet[2];
  // fingerID <<= 8;
  // fingerID |= packet[3];
  
  // confidence = packet[4];
  // confidence <<= 8;
  // confidence |= packet[5];
  
  // return packet[1];
}

void r307sendcommand(uint16_t len_bytes, uint8_t *packet_data)
{
  r307_printHex((uint8_t)HEADER >> 8);
  r307_printHex((uint8_t)HEADER);
  r307_printHex((uint8_t)ADDRESS >> 24);
  r307_printHex((uint8_t)ADDRESS >> 16);
  r307_printHex((uint8_t)ADDRESS >> 8);
  r307_printHex((uint8_t)ADDRESS);
  r307_printHex((uint8_t)COMMAND_PACKET_ID);

  //todo send len
  r307_printHex((uint8_t)len_bytes >> 8);
  r307_printHex((uint8_t)len_bytes);

  //todo send data
  for (uint8_t i=0; i< len_bytes - 2; i++) {
    r307_printHex(packet_data[i]);
  }
  //todo send checksum
  //todo check sum calculation
  uint16_t sum = (len_bytes>>8) + (len_bytes&0xFF) + COMMAND_PACKET_ID;
  r307_printHex((uint8_t)sum >> 8);
  r307_printHex((uint8_t)sum);
}

//todo ana bab3at hex msh char ?? :( i am not sad i am drawn this way
void r307_printHex(uint8_t input)
{
  while ((UART_FR & UART_FR_TXFF) != 0)
  {
  };
  UART_DATA = input;
  //  UART_OutChar(input); // echo debugging
}