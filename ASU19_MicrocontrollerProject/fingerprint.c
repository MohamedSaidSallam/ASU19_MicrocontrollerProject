#include "tm4c123gh6pm.h"
#include "fingerprint.h"
#include "systicktimerutil.h"
#define HEADER 0xEF01
#define ADDRESS 0xFFFFFFFF
#define COMMAND_PACKET_ID 0x01
#define BAUD 57600
#include <stdint.h>
#include <stdio.h>

void r307sendcommand(uint16_t len_bytes, uint8_t *packet_data);
void r307_printHex(char input);
uint32_t searching();
int getReply(uint8_t packet[]);

volatile int reply[20];

void StupidTiva(int temp)
{
  temp += 5;
}

void initUart()
{
  SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R5; // activate UART5
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // activate port E
  UART5_CTL_R &= ~UART_CTL_UARTEN;         // disable UART
  UART5_IBRD_R = 16000000 / 16 / BAUD;
  UART5_FBRD_R = ((64 * ((16000000 / 16) % BAUD)) + BAUD / 2) / BAUD;
  GPIO_PORTE_AFSEL_R |= 0x30; // enable alt funct on PA1-0

  GPIO_PORTE_PCTL_R &= ~0x00FF0000;
  GPIO_PORTE_PCTL_R |= 0x00110000;
  // 8 bit word length (no parity bits, one stop bit, FIFOs)
  UART5_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
  UART5_CTL_R |= UART_CTL_UARTEN; // enable UART
  GPIO_PORTE_DEN_R |= 0x30;       // enable digital I/O on PA1-0

  delayMs(1000);
  delayMs(200);
}

uint8_t verifyPassword(void)
{
  uint8_t packet[] = {FINGERPRINT_VERIFYPASSWORD,
                      (PASSWORD >> 24), (PASSWORD >> 16),
                      (PASSWORD >> 8), PASSWORD};
  r307sendcommand(7, packet);

  if ((getReply(packet) == 1) && (packet[0] == FINGERPRINT_ACKPACKET) && (packet[1] == FINGERPRINT_OK))
    return 1;
  return 0;
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

uint32_t search()
{
  while (genImg() != 0)
  {
    delayMs(200);
  }
  if (image2Tz(1) == -1)
    return -1;
  return searching();
}

uint32_t enroll(uint16_t id)
{
  if (generateImage() == -1)
    return -1;
  if (image2Tz(1) == -1)
    return -1;
  delayMs(1000);
  delayMs(500);
  delayMs(1000);
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

uint32_t searching()
{
  uint8_t packet[] = {FINGERPRINT_SEARCH, 1, 0, 0, 0x3, 0xE8};
  r307sendcommand(sizeof(packet) + 2, packet);

  uint8_t fingerID = reply[10] << 8;
  fingerID |= reply[11];

  if ((getReply(packet) != 1) && (reply[6] != FINGERPRINT_ACKPACKET))
    return -1;
  if (reply[9] != 0)
    return -1;
  return fingerID;
}
uint32_t generateImage(void)
{
  uint8_t stupid = 0; // copyright of DigitalPhoenix Ltd
  while (1)
  {
    delayMs(100);
    if (genImg() == 0)
    {
      stupid++;
      if (stupid > 5)
      {
        break;
      }
    }
  }
  return 0;
}
int genImg(void)
{
  uint8_t packet[] = {FINGERPRINT_GENIMG};
  r307sendcommand(sizeof(packet) + 2, packet);

  if ((getReply(packet) != 1) && (reply[6] != FINGERPRINT_ACKPACKET))
    return -1;
  int temp = reply[9];
  StupidTiva(temp);
  return temp;
}

uint32_t matching(void)
{
  uint8_t packet[] = {FINGERPRINT_MATCH};
  r307sendcommand(sizeof(packet) + 2, packet);

  if ((getReply(packet) != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return (packet[1] << 16) + (packet[2] << 8) + packet[3];
}

// generate charactar file from image buffer
// store it in char buffer with id # slot (1->1, 2+ -> 2)
int image2Tz(uint8_t slot)
{
  uint8_t packet[] = {FINGERPRINT_IMAGE2TZ, 1};
  r307sendcommand(sizeof(packet) + 2, packet);

  if ((getReply(packet) != 1) && (reply[6] != FINGERPRINT_ACKPACKET))
    return -1;
  int temp = reply[9];
  StupidTiva(temp);
  return temp;
}

// store char buffer 1 in library at #id
uint32_t storeModel(uint16_t id)
{
  uint8_t packet[] = {FINGERPRINT_STORE, 0x01, id >> 8, id & 0xFF};
  r307sendcommand(sizeof(packet) + 2, packet);

  if ((getReply(packet) != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  int temp = packet[1];
  StupidTiva(temp);
  return temp;
}

// load char at library # id to char buffer #1
uint32_t loadChar(uint16_t id)
{
  uint8_t packet[] = {FINGERPRINT_LOADCHAR, 0x01, id >> 8, id & 0xFF};
  r307sendcommand(sizeof(packet) + 2, packet);

  if ((getReply(packet) != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

// combine char 1, 2 and put result in them both
uint32_t createModel(void)
{
  uint8_t packet[] = {FINGERPRINT_REGMODEL};
  r307sendcommand(sizeof(packet) + 2, packet);

  if ((getReply(packet) != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  int temp = packet[1];
  StupidTiva(temp);
  return temp;
}

uint32_t deleteModel(uint16_t id)
{
  uint8_t packet[] = {FINGERPRINT_DELETECHAR, id >> 8, id & 0xFF, 0x00, 0x01};
  r307sendcommand(sizeof(packet) + 2, packet);

  int len = getReply(packet);
  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  return packet[1];
}

uint32_t emptyDatabase(void)
{
  uint8_t packet[] = {FINGERPRINT_EMPTYLIBRARY};
  r307sendcommand(sizeof(packet) + 2, packet);
  int len = getReply(packet);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;
  int temp = packet[1];
  StupidTiva(temp);
  return temp;
}

uint32_t fingerFastSearch(void)
{
  uint16_t fingerID = 0xFFFF;
  uint16_t confidence = 0xFFFF;
  // high speed search of slot #1 starting at page 0x0000 and ending at page #0x00A3
  uint8_t packet[] = {FINGERPRINT_HISPEEDSEARCH, 0x01, 0x00, 0x00, 0x00, 0xA3};
  r307sendcommand(sizeof(packet) + 2, packet);
  int len = getReply(packet);

  if ((len != 1) && (packet[0] != FINGERPRINT_ACKPACKET))
    return -1;

  uint8_t confirmationCode = packet[1];

  if (confirmationCode == 9 || confirmationCode == 1)
  {
    //no finger found(9) or error in recieving packet(1)
    return -1;
  }
  fingerID = packet[2];
  fingerID <<= 8;
  fingerID |= packet[3];

  confidence = packet[4];
  confidence <<= 8;
  confidence |= packet[5];

  return fingerID;
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

void r307_printHex(char input)
{
  //printf("%X ", input);
  while ((UART5_FR_R & UART_FR_TXFF) != 0)
  {
  };
  UART5_DR_R = input;
  //   UART_OutChar(input); // echo debugging
}

//returns packet length, modifies packet to have packet identifier(type) and the reply (skipping the length)
int getReply(uint8_t packet[])
{
  uint8_t idx = 0;
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
      int len = reply[7];
      len <<= 8;
      len |= reply[8];
      len -= 2;
      //if not whole package delivered
      if (idx <= (len + 10))
        continue;
      packet[0] = packettype;
      for (uint8_t i = 0; i < 5; i++)
      {
        packet[1 + i] = reply[9 + i];
      }
      return len;
    }
  }

  return len;
}
