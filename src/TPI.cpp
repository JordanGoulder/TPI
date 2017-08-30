#include "TPI.h"
#include <SPI.h>

bool TPIClass::begin()
{
    enableTpiInterface();
    return enterProgrammingMode();
}

void TPIClass::end()
{
    exitProgrammingMode();
    disableTpiInterface();
}

uint32_t TPIClass::readDeviceSignature()
{
    return 0x12345678;
}

void TPIClass::enableTpiInterface()
{
    SPI.begin();
    SPI.setBitOrder(LSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.setClockDivider(SPI_CLOCK_DIV32);

    digitalWrite(SS, LOW);

    delay(1);

    SPI.transfer(0xff);
    SPI.transfer(0xff);
    SPI.transfer(0xff);
}

void TPIClass::disableTpiInterface()
{
    SPI.end();

    digitalWrite(SS, HIGH);

    delay(1);

    pinMode(SS, INPUT);
    pinMode(MOSI, INPUT);
    pinMode(MISO, INPUT);
    pinMode(SCK, INPUT);
}

bool TPIClass::enterProgrammingMode()
{
    tpiSKEY();

    int count = 0;
    while((tpiSLDCS(TPISR) & _BV(NVMEN)) == 0) {
        if (++count > 100) {
            break;
        }
    }

    return (count > 100);
}

bool TPIClass::exitProgrammingMode()
{
    tpiSSTCS(TPISR, _BV(NVMEN));

    int count = 0;
    while((tpiSLDCS(TPISR) & _BV(NVMEN)) == 0) {
        if (++count > 100) {
            break;
        }
    }

    return (count > 100);
}

void TPIClass::tpiSSTCS(uint8_t address, uint8_t data)
{
    tpiSend(SSTCS | address);
    tpiSend(data);
}

void TPIClass::tpiSKEY()
{
    tpiSend(SKEY);
    tpiSend(0xFF);
    tpiSend(0x88);
    tpiSend(0xD8);
    tpiSend(0xCD);
    tpiSend(0x45);
    tpiSend(0xAB);
    tpiSend(0x89);
    tpiSend(0x12);
}

