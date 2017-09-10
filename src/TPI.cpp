#include "TPI.h"
#include <SPI.h>

#define IO_ADDR_HI_MASK (0x30)
#define IO_ADDR_LO_MASK (0x0F)
#define IO_ADDR_HI_OFFSET (1)
#define IO_ADDR_LO_OFFSET (0)

#define IO_ADDR(x) (        (((x) & IO_ADDR_HI_MASK) << IO_ADDR_HI_OFFSET) \
                        |   (((x) & IO_ADDR_LO_MASK) << IO_ADDR_LO_OFFSET) )

SPISettings TPIClass::spiSettings(1000000, LSBFIRST, SPI_MODE0);
int TPIClass::resetPin = SS;

bool TPIClass::begin(int resetPin)
{
    TPIClass::resetPin = resetPin;

    enableSpiInterface();

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
    disableSpiInterface();
}

TPIClass::NvmProtectionMode TPIClass::nvmProtectionMode()
{
    NvmProtectionMode mode;

    sstpr(NVM_LOCK_BITS_START);

    uint8_t lockBits = sld();

    if ((lockBits & _BV(NVLB1)) == _BV(NVLB1) && (lockBits & _BV(NVLB2)) == _BV(NVLB2)) {
        mode = NO_PROTECTION;
    } else if ((lockBits & _BV(NVLB1)) == 0 && (lockBits & _BV(NVLB2)) == 0) {
        mode = WRITE_AND_VERIFY_PROTECTION;
    } else if ((lockBits & _BV(NVLB1)) == 0) {
        mode = WRITE_PROTECTION;
    } else {
        mode = UNDEFINED_PROTECTION;
    }

    return mode;
}

bool TPIClass::setNvmProtectionMode(NvmProtectionMode mode)
{
    NvmProtectionMode currentMode = nvmProtectionMode();

    uint8_t lockBits;

    switch (mode) {
        case NO_PROTECTION:
            if (currentMode == NO_PROTECTION) {
                return true;
            } else {
                return false;
            }
            break;

        case WRITE_PROTECTION:
            if ((currentMode != NO_PROTECTION) && (currentMode != WRITE_PROTECTION)) {
                return false;
            } else {
                lockBits = ~_BV(NVLB1);
            }
            break;

        case WRITE_AND_VERIFY_PROTECTION:
            if ((currentMode != NO_PROTECTION) && (currentMode != WRITE_PROTECTION)) {
                return false;
            } else {
                lockBits = ~(_BV(NVLB2) | _BV(NVLB1));
            }
            break;


        default:
            return false;
            break;
    }

    sstpr(NVM_LOCK_BITS_START);
    sout(NVMCMD, WRITE_WORD);
    sst(lockBits, true);
    sst(0xFF);

    uint8_t retriesRemaining = 100;

    do {
        if ((sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
}


bool TPIClass::externalResetDisable()
{
    sstpr(CONFIGURATION_BITS_START);

    return ((sld() & _BV(RSTDISBL)) == 0);
}

bool TPIClass::setExternalResetDisable(bool enable)
{
    return setConfigBit(RSTDISBL, enable);
}

bool TPIClass::watchdogTimerAlwaysOn()
{
    sstpr(CONFIGURATION_BITS_START);

    return ((sld() & _BV(WDTON)) == 0);
}

bool TPIClass::setWatchdogTimerAlwaysOn(bool enable)
{
    return setConfigBit(WDTON, enable);
}

bool TPIClass::systemClockOutput()
{
    sstpr(CONFIGURATION_BITS_START);

    return ((sld() & _BV(CKOUT)) == 0);
}

bool TPIClass::setSystemClockOutput(bool enable)
{
    return setConfigBit(CKOUT, enable);
}

uint8_t TPIClass::oscillatorCalibration()
{
    sstpr(CALIBRATION_BITS_START);
    return sld();
}

uint32_t TPIClass::deviceSignature()
{
    uint32_t signature;

    sstpr(DEVICE_ID_BITS_START);

    signature = sld(POST_INC);
    signature <<= 8;

    signature |= sld(POST_INC);
    signature <<= 8;

    signature |= sld();

    return signature;
}

void TPIClass::enableSpiInterface()
{
    SPI.begin();

    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
}

void TPIClass::disableSpiInterface()
{
    SPI.end();

    pinMode(resetPin, INPUT);
    pinMode(MOSI, INPUT);
    pinMode(MISO, INPUT);
    pinMode(SCK, INPUT);
}

void TPIClass::enableTpiInterface()
{
    digitalWrite(resetPin, LOW);
    delay(T_RST);

    SPI.beginTransaction(spiSettings);
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);
    SPI.transfer(0xFF);
    SPI.endTransaction();
}

void TPIClass::disableTpiInterface()
{
    digitalWrite(resetPin, HIGH);
    delay(T_RST);
}

bool TPIClass::enterNvmProgrammingMode()
{
    skey(NVM_PROG_ENABLE);

    uint8_t retriesRemaining = 100;

    do {
        if ((sldcs(TPISR) & _BV(NVMEN)) == _BV(NVMEN)) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
}

void TPIClass::exitNvmProgrammingMode()
{
    uint8_t status = sldcs(TPISR);
    status &= ~_BV(NVMEN);
    sstcs(TPISR, status);
}

bool TPIClass::setConfigBit(uint8_t bit, bool enable)
{
    sstpr(CONFIGURATION_BITS_START);

    uint8_t config = sld(true);

    sout(NVMCMD, SECTION_ERASE);
    sst(0xFF);

    uint8_t retriesRemaining = 100;

    do {
        if ((sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    if (retriesRemaining) {

        if (enable) {
            config &= ~_BV(bit);
        } else {
            config |= _BV(bit);
        }

        sstpr(CONFIGURATION_BITS_START);
        sout(NVMCMD, WRITE_WORD);
        sst(config, true);
        sst(0xFF);

        retriesRemaining = 100;

        do {
            if ((sin(NVMCSR) & _BV(NVMBSY)) == 0) {
                break;
            }
        } while (--retriesRemaining);
    }

    return (retriesRemaining > 0);
}

uint8_t TPIClass::sld(bool postIncrement)
{
    if (postIncrement) {
        write(SLD_POST_INC);
    } else {
        write(SLD);
    }

    return read();
}

void TPIClass::sst(uint8_t data, bool postIncrement)
{
    if (postIncrement) {
        write(SST_POST_INC);
    } else {
        write(SST);
    }

    return write(data);
}

void TPIClass::sstpr(uint16_t data)
{
    write(SSTPR_LO);
    write(LOW_BYTE(data));
    write(SSTPR_HI);
    write(HIGH_BYTE(data));
}

uint8_t TPIClass::sin(uint8_t address)
{
    write(SIN | IO_ADDR(address));
    return read();
}

void TPIClass::sout(uint8_t address, uint8_t data)
{
    write(SOUT | IO_ADDR(address));
    write(data);
}

uint8_t TPIClass::sldcs(uint8_t address)
{
    write(SLDCS | (address & CS_ADDR_MASK));
    return read();
}

void TPIClass::sstcs(uint8_t address, uint8_t data)
{
    write(SSTCS | (address & CS_ADDR_MASK));
    write(data);
}

void TPIClass::skey(uint64_t key)
{
    write(SKEY);

    while (key) {
        write(LOW_BYTE(key));
        key >>= 8;
    }
}

void TPIClass::write(uint8_t value)
{
    uint8_t par = value;
    par ^= (par >> 4); // b[7:4] (+) b[3:0]
    par ^= (par >> 2); // b[3:2] (+) b[1:0]
    par ^= (par >> 1); // b[1] (+) b[0]

    SPI.beginTransaction(spiSettings);

    // REMEMBER: this is in LSBfirst mode and idle is high
    // (2 idle) + (1 start bit) + (data[4:0])
    SPI.transfer(0x03 | (value << 3));
    // (data[7:5]) + (1 parity) + (2 stop bits) + (2 idle)
    SPI.transfer(0xf0 | (par << 3) | (value >> 5));

    SPI.endTransaction();
}

uint8_t TPIClass::read()
{
    uint8_t b1, b2, b3;

    SPI.beginTransaction(spiSettings);

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

    SPI.endTransaction();

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

