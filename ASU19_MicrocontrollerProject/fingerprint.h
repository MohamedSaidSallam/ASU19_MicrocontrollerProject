#include <stdint.h>

void initUart();
uint8_t generateImage(void); 
uint8_t image2Tz(uint8_t slot);
uint8_t storeModel(uint16_t id);
uint8_t loadChar(uint16_t id);
uint8_t createModel(void);
uint8_t deleteModel(uint16_t id);
uint8_t emptyDatabase(void);
uint8_t fingerFastSearch(void);

#define HEADER 0xEF01
#define ADDRESS 0xFFFFFFFF
#define COMMAND_PACKET_ID 0x01
#define BAUD 57600
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
#define PORT_PINS 0XD0

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

#define FINGERPRINT_TIMEOUT 0xFF
#define FINGERPRINT_BADPACKET 0xFE

