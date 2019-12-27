#include <stdint.h>

void initUart();
uint8_t verifyPassword(void);

uint32_t generateImage(void);
uint32_t image2Tz(uint8_t slot);
uint32_t storeModel(uint16_t id);
uint32_t loadChar(uint16_t id);
uint32_t createModel(void);
uint32_t deleteModel(uint16_t id);
uint32_t emptyDatabase(void);
uint32_t fingerFastSearch(void);
uint32_t genImg();
uint32_t match(uint16_t id);
uint32_t matching(void);
uint32_t enroll(uint16_t id);
uint32_t search();


#define HEADER 0xEF01
#define ADDRESS 0xFFFFFFFF
#define COMMAND_PACKET_ID 0x01
#define BAUD 57600
//uart 2
#define UART_CTL UART5_CTL_R
#define UART_FBRD UART5_FBRD_R
#define UART_IBRD UART5_IBRD_R
#define UART_LCRH UART5_LCRH_R
#define UART_IFLS UART5_IFLS_R
#define UART_IM UART5_IM_R
#define UART_CTL UART5_CTL_R
#define UART_DATA UART5_DR_R
#define UART_FR UART5_FR_R

//pd6 7
#define GPIO_PORT_AFSEL GPIO_PORTA_AFSEL_R
#define GPIO_PORT_PCTL GPIO_PORTA_PCTL_R
#define GPIO_PCTL_PINS 0x00000011
#define GPIO_PCTL_IGNORED_PINS 0xFFFFFF00

#define UART_ENABLE 0x1 //uart2
#define PORT_ENABLE 0x1 //port D
#define PORT_PINS 0x03

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

#define FINGERPRINT_STARTCODE 0xEF01

#define FINGERPRINT_COMMANDPACKET 0x1
#define FINGERPRINT_DATAPACKET 0x2
#define FINGERPRINT_ACKPACKET 0x7
#define FINGERPRINT_ENDDATAPACKET 0x8
#define FINGERPRINT_VERIFYPASSWORD 0x13
#define FINGERPRINT_OK 0x0
#define PASSWORD 0x0

#define FINGERPRINT_TIMEOUT 0xFF
#define FINGERPRINT_BADPACKET 0xFE

