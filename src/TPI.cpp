#include "TPI.h"
#include <SPI.h>

#include "tpi_access_layer.h"

#define IS_ODD(x)   (((x) & 0x01) == 0x01)

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

    tpi_sstpr(NVM_LOCK_BITS_START);

    uint8_t lockBits = tpi_sld();

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

    tpi_sstpr(NVM_LOCK_BITS_START);
    tpi_sout(WRITE_WORD, NVMCMD);
    tpi_sst_postinc(lockBits);
    tpi_sst(0xFF);

    uint8_t retriesRemaining = 100;

    do {
        if ((tpi_sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
}


bool TPIClass::externalResetDisable()
{
    tpi_sstpr(CONFIGURATION_BITS_START);

    return ((tpi_sld() & _BV(RSTDISBL)) == 0);
}

bool TPIClass::setExternalResetDisable(bool enable)
{
    return setConfigBit(RSTDISBL, enable);
}

bool TPIClass::watchdogTimerAlwaysOn()
{
    tpi_sstpr(CONFIGURATION_BITS_START);

    return ((tpi_sld() & _BV(WDTON)) == 0);
}

bool TPIClass::setWatchdogTimerAlwaysOn(bool enable)
{
    return setConfigBit(WDTON, enable);
}

bool TPIClass::systemClockOutput()
{
    tpi_sstpr(CONFIGURATION_BITS_START);

    return ((tpi_sld() & _BV(CKOUT)) == 0);
}

bool TPIClass::setSystemClockOutput(bool enable)
{
    return setConfigBit(CKOUT, enable);
}

uint8_t TPIClass::oscillatorCalibration()
{
    tpi_sstpr(CALIBRATION_BITS_START);
    return tpi_sld();
}

uint32_t TPIClass::deviceSignature()
{
    uint32_t signature;

    tpi_sstpr(DEVICE_ID_BITS_START);

    signature = tpi_sld_postinc();
    signature <<= 8;

    signature |= tpi_sld_postinc();
    signature <<= 8;

    signature |= tpi_sld();

    return signature;
}

void TPIClass::readMemory(uint16_t address, void *buffer, uint16_t count)
{
    uint8_t *pBuffer = (uint8_t *) buffer;

    tpi_sstpr(address);

    while (count--) {
        *(pBuffer++) = tpi_sld_postinc();
    }
}

bool TPIClass::writeMemory(uint16_t address, void *buffer, uint16_t count)
{
    uint8_t *pBuffer = (uint8_t *) buffer;

    if (IS_ODD(address)) {

        tpi_sstpr(address - 1);
        uint8_t lowByte = tpi_sld();

        tpi_sout(WRITE_WORD, NVMCMD);

        tpi_sst_postinc(lowByte);
        tpi_sst_postinc(*(pBuffer++));

        if (!whileNvmBusy()) {
            return false;
        }

        address++;
        count--;
    }

    tpi_sstpr(address);
    tpi_sout(WRITE_WORD, NVMCMD);

    while (count > 1) {

        tpi_sst_postinc(*(pBuffer++));
        tpi_sst_postinc(*(pBuffer++));

        if (!whileNvmBusy()) {
            return false;
        }

        address += 2;
        count -= 2;
    }

    if (count) {

        tpi_sld_postinc();
        uint8_t highByte = tpi_sld();

        tpi_sstpr(address);
        tpi_sout(WRITE_WORD, NVMCMD);

        tpi_sst_postinc(*(pBuffer++));
        tpi_sst_postinc(highByte);

        if (!whileNvmBusy()) {
            return false;
        }
    }

    return true;
}

bool TPIClass::eraseChip()
{
    tpi_sout(CHIP_ERASE, NVMCMD);

    tpi_sstpr(PROGRAM_FLASH_MEMORY_START);

    tpi_sst_postinc(0xFF);
    tpi_sst(0xFF);

    uint8_t retriesRemaining = 100;

    do {
        if ((tpi_sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
}

bool TPIClass::eraseConfigurationMemory()
{
    tpi_sout(SECTION_ERASE, NVMCMD);

    tpi_sstpr(CONFIGURATION_BITS_START);

    tpi_sst_postinc(0xFF);
    tpi_sst(0xFF);

    uint8_t retriesRemaining = 100;

    do {
        if ((tpi_sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
}

bool TPIClass::eraseProgramMemory()
{
    tpi_sout(SECTION_ERASE, NVMCMD);

    tpi_sstpr(PROGRAM_FLASH_MEMORY_START);

    tpi_sst_postinc(0xFF);
    tpi_sst(0xFF);

    uint8_t retriesRemaining = 100;

    do {
        if ((tpi_sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
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
    tpi_skey();

    uint8_t retriesRemaining = 100;

    do {
        if ((tpi_sldcs(TPISR) & _BV(NVMEN)) == _BV(NVMEN)) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
}

void TPIClass::exitNvmProgrammingMode()
{
    uint8_t status = tpi_sldcs(TPISR);
    status &= ~_BV(NVMEN);
    tpi_sstcs(status, TPISR);
}

bool TPIClass::whileNvmBusy()
{
    uint8_t retriesRemaining = 100;

    do {
        if ((tpi_sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    return (retriesRemaining > 0);
}

bool TPIClass::setConfigBit(uint8_t bit, bool enable)
{
    tpi_sstpr(CONFIGURATION_BITS_START);

    uint8_t config = tpi_sld_postinc();

    tpi_sout(SECTION_ERASE, NVMCMD);
    tpi_sst(0xFF);

    uint8_t retriesRemaining = 100;

    do {
        if ((tpi_sin(NVMCSR) & _BV(NVMBSY)) == 0) {
            break;
        }
    } while (--retriesRemaining);

    if (retriesRemaining) {

        if (enable) {
            config &= ~_BV(bit);
        } else {
            config |= _BV(bit);
        }

        tpi_sstpr(CONFIGURATION_BITS_START);
        tpi_sout(WRITE_WORD, NVMCMD);
        tpi_sst_postinc(config);
        tpi_sst(0xFF);

        retriesRemaining = 100;

        do {
            if ((tpi_sin(NVMCSR) & _BV(NVMBSY)) == 0) {
                break;
            }
        } while (--retriesRemaining);
    }

    return (retriesRemaining > 0);
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

