#ifndef TINYSPI_H_INCLUDED
#define TINYSPI_H_INCLUDED

#include <util/atomic.h>

// USI ports and pins
#define SPI_DDR_PORT DDRA
#define USCK_DD_PIN DDA4
#define DO_DD_PIN DDA5
#define DI_DD_PIN DDA6

// SPI data modes
#define SPI_MODE0 0x00
#define SPI_MODE1 0x04

class tinySPI
{
    public:
        static void begin();
        static void setDataMode(uint8_t spiDataMode);
        static uint8_t transfer(uint8_t spiData);
        static void end();
};

extern tinySPI SPI;

#endif
