#ifndef _MGPIO_466_H_INCLUDED_
#define _MGPIO_466_H_INCLUDED_

#include <stdint.h>

/*
** this is the address of that various registers in the GPIO expander.
** Note that the actual address depends on the setting on the BANK bit.
** read how to set the bank bit and note the changes it makes in use of
** device.
*/
// set pin direction
#define IODIRA   0x00
#define IODIRB   0x01
// set pin polarity
#define IPOLA    0x02
#define IPOLB    0x03
// enable interrupt on change
#define GPINTENA 0x04
#define GPINTENB 0x05
// default comparison for register
#define DEFVALA  0x06
#define DEFVALB  0x07
// compare settings for interrupts
#define INTCONA  0x08
#define INTCONB  0x09
// control bank bit and other settings
#define IOCONA   0x0a
#define IOCONB   0x0b
// enable pull up resistor
#define GPPUA    0x0c
#define GPPUB    0x0d
// which pin caused interrupt (read only)
#define INTFA    0x0e
#define INTFB    0x0f
// GPIOs at time of last interrupt (read only)
#define INTCAPA  0x10
#define INTCAPB  0x11
// chip GPIO address
#define GPIOA    0x12
#define GPIOB    0x13
// output latch register
#define OLATA    0x14
#define OLATB    0x15
// equivalent to 1000 0000 for binary operations
#define MASK     0x80

//#define SW1_PIN (uint8_t) 20
//#define SW2_PIN (uint8_t) 19
//const uint8_t SW1_PIN = 20;
//const uint8_t SW2_PIN = 19;

void    mGpioInit(void);
void    mGpioWriteByte(uint8_t address, uint8_t byte);
uint8_t mGpioReadByte(uint8_t address);

void setClk(bool state);
void setOut(uint8_t out);
uint8_t getIn();
uint8_t transfer(uint8_t out);


#endif // _MGPIO_466_H_INCLUDED_
