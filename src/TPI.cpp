#include "TPI.h"
#include <SPI.h>

bool TPIClass::begin()
{
    enableTpiInterface();
    return enterNvmProgrammingMode();
}

void TPIClass::end()
{
    exitNvmProgrammingMode();
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

bool TPIClass::enterNvmProgrammingMode()
{
    skey();

    int count = 0;
    while((sldcs(TPISR) & _BV(NVMEN)) == 0) {
        if (++count > 100) {
            break;
        }
    }

    return (count > 100);
}

void TPIClass::exitNvmProgrammingMode()
{
    sstcs(TPISR, _BV(NVMEN));

    int count = 0;
    while((sldcs(TPISR) & _BV(NVMEN)) == 0) {
        if (++count > 100) {
            break;
        }
    }

    return (count > 100);
}

void TPIClass::sstcs(uint8_t address, uint8_t data)
{
    write(SSTCS | address);
    write(data);
}

void TPIClass::skey()
{
    write(SKEY);
    write(0xFF);
    write(0x88);
    write(0xD8);
    write(0xCD);
    write(0x45);
    write(0xAB);
    write(0x89);
    write(0x12);
}

