#ifndef TPI_H
#define TPI_H

#include <Arduino.h>

#ifndef LOW_BYTE
#define LOW_BYTE(x)     ( (uint8_t) ( (x) & 0xFF) )
#endif

#ifndef HIGH_BYTE
#define HIGH_BYTE(x)    ( (uint8_t) ( ( (x) >> 8) & 0xFF) )
#endif

class SPISettings;

class TPIClass
{

public:
    static bool             begin(int resetPin);
    static void             end();

    static bool             externalResetDisable();
    static bool             watchdogTimerAlwaysOn();
    static bool             systemClockOutput();

    static uint8_t          oscillatorCalibration();

    static uint32_t         deviceSignature();

private:
    static void             enableSpiInterface();
    static void             disableSpiInterface();

    static void             enableTpiInterface();
    static void             disableTpiInterface();

    static bool             enterNvmProgrammingMode();
    static void             exitNvmProgrammingMode();

    static uint8_t          sld     (bool postIncrement = NO_POST_INC);
    static void             sst     (uint8_t data, bool postIncrement = NO_POST_INC);
    static void             sstpr   (uint16_t data);
    static uint8_t          sin     (uint8_t address);
    static void             sout    (uint8_t address, uint8_t data);
    static uint8_t          sldcs   (uint8_t address);
    static void             sstcs   (uint8_t address, uint8_t data);
    static void             skey    (uint64_t key);

    static void             write   (uint8_t value);
    static uint8_t          read    ();

    static const bool       NO_POST_INC     = false;
    static const bool       POST_INC        = true;

    static const uint8_t    TPISR           = 0x00;
    static const uint8_t    TPIPCR          = 0x02;
    static const uint8_t    TPIIR           = 0x0F;
    static const uint8_t    NVMEN           = 1;

    static const uint8_t    SLD             = 0x20;
    static const uint8_t    SLD_POST_INC    = 0x24;
    static const uint8_t    SST             = 0x60;
    static const uint8_t    SST_POST_INC    = 0x64;
    static const uint8_t    SSTPR_LO        = 0x68;
    static const uint8_t    SSTPR_HI        = 0x69;
    static const uint8_t    SIN             = 0x10;
    static const uint8_t    SOUT            = 0x90;
    static const uint8_t    SLDCS           = 0x80;
    static const uint8_t    SSTCS           = 0xC0;
    static const uint8_t    SKEY            = 0xE0;

    static const uint8_t    CS_ADDR_MASK    = 0x0F;

    static const uint8_t    T_RST           = 1;

    static const uint64_t   NVM_PROG_ENABLE = 0x1289AB45CDD888FFULL;

    static const uint16_t   IO_SPACE_START              = 0x0000;
    static const uint16_t   SRAM_DATA_MEMORY_START      = 0x0040;
    static const uint16_t   NVM_LOCK_BITS_START         = 0x3F00;
    static const uint16_t   CONFIGURATION_BITS_START    = 0x3F40;
    static const uint16_t   CALIBRATION_BITS_START      = 0x3F80;
    static const uint16_t   DEVICE_ID_BITS_START        = 0x3FC0;
    static const uint16_t   PROGRAM_FLASH_MEMORY_START  = 0x4000;

    static const uint8_t    RSTDISBL        = 0;
    static const uint8_t    WDTON           = 1;
    static const uint8_t    CKOUT           = 2;

    static SPISettings      spiSettings;
    static int              resetPin;
};

extern TPIClass TPI;

#endif // TPI_H
