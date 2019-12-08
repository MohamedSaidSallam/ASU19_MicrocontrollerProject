#include "tm4c123gh6pm.h"
#include "fingerprint.h"
#define HEADER 0xEF01
#define ADDRESS 0xFFFFFFFF
#define COMMAND_PACKET_ID 0x01
#define BAUD 57600
#include <stdint.h>
#include <stdio.h>

void r307sendcommand(uint16_t len_bytes, uint8_t *packet_data);
void r307_printHex(char input);
uint32_t searching(uint16_t id, uint16_t pagenum);
uint32_t matching(void);
uint8_t getReply(uint8_t packet[]);

void initUart()
{
    SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0; // activate UART0
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // activate port A
  UART0_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
  UART0_IBRD_R = 16000000 / 16 / BAUD;
  UART0_FBRD_R = ((64 * ((16000000 / 16) % BAUD)) + BAUD / 2) / BAUD;

    //GPIO_PORTA_PCTL_R = 
      //      (GPIO_PORTA_PCTL_R&0xFFFFFF00)+0x00000011;
                                      // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART0_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
  UART0_CTL_R |= UART_CTL_UARTEN;       // enable UART
  GPIO_PORTA_AFSEL_R |= 0x03;           // enable alt funct on PA1-0
  GPIO_PORTA_DEN_R |= 0x03;             // enable digital I/O on PA1-0
}

uint32_t match(uint16_t id)
{
  if (loadChar(id) == -1)
    return -1;
  if (generateImage() == -1)
    return -1;
  if (image2Tz(2) == -1)
    return -1;
  return matching();
}

uint32_t search(uint16_t id, uint16_t pagenum, uint8_t slot)
{

  if (generateImage() == -1)
    return -1;
  if (image2Tz(slot) == -1)
    return -1;
  return searching(id, pagenum);
}

uint32_t enroll(uint16_t id)
{
  if (generateImage() == -1)
    return -1;
  if (image2Tz(1) == -1)
    return -1;
  if (generateImage() == -1)
    return -1;
  if (image2Tz(2) == -1)
    return -1;
  if (createModel() == -1)
    return -1;
  if (storeModel(id) == -1)
    return -1;
  return 0;
}

uint32_t searching(uint16_t id, uint16_t pagenum)
{
  uint8_t packet[] = {FINGERPRINT_SEARCH};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return (packet[1] << 16) + (packet[2] << 8) + packet[3];
}

uint32_t generateImage(void)
{
  uint8_t packet[] = {FINGERPRINT_GENIMG};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}
uint32_t matching(void)
{
  uint8_t packet[] = {FINGERPRINT_MATCH};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return (packet[1] << 16) + (packet[2] << 8) + packet[3];
}

// generate charactar file from image buffer
// store it in char buffer with id # slot (1->1, 2+ -> 2)
uint32_t image2Tz(uint8_t slot)
{
  uint8_t packet[] = {FINGERPRINT_IMAGE2TZ, slot};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

// store char buffer 1 in library at #id
uint32_t storeModel(uint16_t id)
{
  uint8_t packet[] = {FINGERPRINT_STORE, 0x01, id >> 8, id & 0xFF};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

// load char at library # id to char buffer #1
uint32_t loadChar(uint16_t id)
{
  uint8_t packet[] = {FINGERPRINT_LOADCHAR, 0x01, id >> 8, id & 0xFF};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

// combine char 1, 2 and put result in them both
uint32_t createModel(void)
{
  uint8_t packet[] = {FINGERPRINT_REGMODEL};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

uint32_t deleteModel(uint16_t id)
{
  uint8_t packet[] = {FINGERPRINT_DELETECHAR, id >> 8, id & 0xFF, 0x00, 0x01};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

uint32_t emptyDatabase(void)
{
  uint8_t packet[] = {FINGERPRINT_EMPTYLIBRARY};
  r307sendcommand(sizeof(packet) + 2, packet);
  uint8_t len = getReply(packet);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

uint32_t fingerFastSearch(void)
{
  uint16_t fingerID = 0xFFFF;
  uint16_t confidence = 0xFFFF;
  // high speed search of slot #1 starting at page 0x0000 and ending at page #0x00A3
  uint8_t packet[] = {FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3};
  r307sendcommand(sizeof(packet) + 2, packet);
  uint8_t len = getReply(packet);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;

  fingerID = packet[2];
  fingerID <<= 8;
  fingerID |= packet[3];

  confidence = packet[4];
  confidence <<= 8;
  confidence |= packet[5];

  return packet[1];
}

void r307sendcommand(uint16_t len_bytes, uint8_t *packet_data)
{
  r307_printHex((uint8_t)(HEADER >> 8));
  r307_printHex((uint8_t)(HEADER));
  r307_printHex((uint8_t)(ADDRESS >> 24));
  r307_printHex((uint8_t)(ADDRESS >> 16));
  r307_printHex((uint8_t)(ADDRESS >> 8));
  r307_printHex((uint8_t)(ADDRESS));
  r307_printHex((uint8_t)(COMMAND_PACKET_ID));

  //todo send len
  r307_printHex((uint8_t)(len_bytes >> 8));
  r307_printHex((uint8_t)(len_bytes));

  //todo send data
  for (uint8_t i = 0; i < len_bytes - 2; i++)
  {
    r307_printHex(packet_data[i]);
  }

  uint16_t sum = (len_bytes >> 8) + (len_bytes & 0xFF) + COMMAND_PACKET_ID;
  for (uint8_t i = 0; i < len_bytes - 2; i++)
  {
    sum += packet_data[i];
  }
  r307_printHex((uint8_t)(sum >> 8));
  r307_printHex((uint8_t)(sum));
}

//todo ana bab3at hex msh char ?? :( i am not sad i am drawn this way
void r307_printHex(char input)
{
  printf("%X ", input);
  while ((UART0_FR_R & UART_FR_TXFF) != 0)
  {
  };
  UART0_DR_R = input;
//   UART_OutChar(input); // echo debugging
}

//returns packet type then the packet
uint8_t getReply(uint8_t packet[])
{
  uint8_t reply[20], idx;
  idx = 0;
  uint16_t len = 0;
  while ((UART_FR & UART_FR_RXFE) == 0)
  {
    reply[idx] = UART_DATA; // retrieve char from FIFO
    //if not a valid packet start
    if ((idx == 0) && (reply[0] != (FINGERPRINT_STARTCODE >> 8)))
      continue;
    idx++;

    // check packet!
    if (idx >= 9)
    {
      if ((reply[0] != (FINGERPRINT_STARTCODE >> 8)) ||
          (reply[1] != (FINGERPRINT_STARTCODE & 0xFF)))
        return FINGERPRINT_BADPACKET;
      uint8_t packettype = reply[6];
      uint16_t len = reply[7];
      len <<= 8;
      len |= reply[8];
      len -= 2;
      //if not whole package delivered
      if (idx <= (len + 10))
        continue;
      packet[0] = packettype;
      for (uint8_t i = 0; i < len; i++)
      {
        packet[1 + i] = reply[9 + i];
      }
    }
  }

  return len;
}
