#include "TPI.h"
#include <SPI.h>

bool TPIClass::begin()
{
    enableTpiInterface();
    bool success = enterNvmProgrammingMode();

    if (!success) {
        end();
    }

    return success;
}

void TPIClass::end()
{
    exitNvmProgrammingMode();
    disableTpiInterface();
}

uint32_t TPIClass::readDeviceSignature()
{
    uint32_t signature = 0;
    sstpr(0x3FC0);
    signature |= ((uint32_t) sld(POST_INC)) << 16;
    signature |= ((uint32_t) sld(POST_INC)) << 8;
    signature |= sld();

    return signature;
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
    digitalWrite(SS, HIGH);

    delay(1);

    SPI.end();

    pinMode(SS, INPUT);
    pinMode(MOSI, INPUT);
    pinMode(MISO, INPUT);
    pinMode(SCK, INPUT);
}

bool TPIClass::enterNvmProgrammingMode()
{
    skey();

    uint8_t retries = 100;

    do {
        if ((sldcs(TPISR) & _BV(NVMEN)) == _BV(NVMEN)) {
            break;
        }
    } while (--retries);

    return (retries > 0);
}

void TPIClass::exitNvmProgrammingMode()
{
    uint8_t status = sldcs(TPISR);
    status &= ~_BV(NVMEN);
    sstcs(TPISR, status);
}

uint8_t TPIClass::sld(bool postIncrement)
{
    if (postIncrement) {
        write(SLD_POSTINC);
    } else {
        write(SLD);
    }

    return read();
}

void TPIClass::sstpr(uint16_t data)
{
    write(SSTPR_LO);
    write(data & 0xFF);
    write(SSTPR_HI);
    write(data >> 8);
}

uint8_t TPIClass::sldcs(uint8_t address)
{
    write(SLDCS | address);
    return read();
}

void TPIClass::sstcs(uint8_t address, uint8_t data)
{
    write(SSTCS | address);
    write(data);
}

void TPIClass::skey()
{
    static uint64_t nvm_program_enable = 0x1289AB45CDD888FFULL;
    write(SKEY);
    while (nvm_program_enable) {
        write(nvm_program_enable & 0xFF);
        nvm_program_enable >>= 8;
    }
}

void TPIClass::write(uint8_t value)
{
    uint8_t par = value;
    par ^= (par >> 4); // b[7:4] (+) b[3:0]
    par ^= (par >> 2); // b[3:2] (+) b[1:0]
    par ^= (par >> 1); // b[1] (+) b[0]

    // REMEMBER: this is in LSBfirst mode and idle is high
    // (2 idle) + (1 start bit) + (data[4:0])
    SPI.transfer(0x03 | (value << 3));
    // (data[7:5]) + (1 parity) + (2 stop bits) + (2 idle)
    SPI.transfer(0xf0 | (par << 3) | (value >> 5));
}

uint8_t TPIClass::read()
{
    uint8_t b1, b2, b3;

    // keep transmitting high(idle) while waiting for a start bit
    do {
        b1 = SPI.transfer(0xff);
    } while (0xff == b1);

    // get (partial) data bits
    b2 = SPI.transfer(0xff);

    // if the first byte(b1) contains less than 4 data bits
    // we need to get a third byte to get the parity and stop bits
    if (0x0f == (0x0f & b1)) {
        b3 = SPI.transfer(0xff);
    }

    // now shift the bits into the right positions
    // b1 should hold only idle and start bits = 0b01111111
    while (0x7f != b1) { // data not aligned

        b2 <<= 1; // shift left data bits

        if (0x80 & b1) { // carry from 1st byte
          b2 |= 1; // set bit
        }

        b1 <<= 1;
        b1 |= 0x01; // fill with idle bit (1)
    }

    // now the data byte is stored in b2
    return( b2 );
}

