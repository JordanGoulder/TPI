#ifndef TPI_H
#define TPI_H

#include <Arduino.h>

class SPISettings;

class TPIClass
{

public:
    static bool                 begin(int resetPin);
    static void                 end();

    enum NvmProtectionMode
    {
        NO_PROTECTION               = 0,
        WRITE_PROTECTION            = 1,
        UNDEFINED_PROTECTION        = 2,
        WRITE_AND_VERIFY_PROTECTION = 3
    };

    static NvmProtectionMode    nvmProtectionMode();
    static bool                 setNvmProtectionMode(NvmProtectionMode mode);

    static bool                 externalResetDisable();
    static bool                 setExternalResetDisable(bool enable);

    static bool                 watchdogTimerAlwaysOn();
    static bool                 setWatchdogTimerAlwaysOn(bool enable);

    static bool                 systemClockOutput();
    static bool                 setSystemClockOutput(bool enable);

    static uint8_t              oscillatorCalibration();

    static uint32_t             deviceSignature();

    static void                 readMemory(uint16_t address, void *buffer, uint16_t count);
    static bool                 writeMemory(uint16_t address, void *buffer, uint16_t count);

    static bool                 eraseChip();
    static bool                 eraseConfigurationMemory();
    static bool                 eraseProgramMemory();

private:
    static void                 enableSpiInterface();
    static void                 disableSpiInterface();

    static void                 enableTpiInterface();
    static void                 disableTpiInterface();

    static bool                 enterNvmProgrammingMode();
    static void                 exitNvmProgrammingMode();
    static bool                 whileNvmBusy();

    static bool                 setConfigBit(uint8_t bit, bool enable);

    static void                 write   (uint8_t value);
    static uint8_t              read    ();

    static const uint8_t        TPISR           = 0x00;
    static const uint8_t        TPIPCR          = 0x02;
    static const uint8_t        TPIIR           = 0x0F;
    static const uint8_t        NVMEN           = 1;

    static const uint8_t        NVMCSR          = 0x32;
    static const uint8_t        NVMBSY          = 7;
    static const uint8_t        NVMCMD          = 0x33;
    static const uint8_t        NO_OPERATION    = 0x00;
    static const uint8_t        CHIP_ERASE      = 0x10;
    static const uint8_t        SECTION_ERASE   = 0x14;
    static const uint8_t        WRITE_WORD      = 0x1D;


    static const uint8_t        T_RST           = 1;

    static const uint16_t       IO_SPACE_START              = 0x0000;
    static const uint16_t       SRAM_DATA_MEMORY_START      = 0x0040;
    static const uint16_t       NVM_LOCK_BITS_START         = 0x3F00;
    static const uint16_t       CONFIGURATION_BITS_START    = 0x3F40;
    static const uint16_t       CALIBRATION_BITS_START      = 0x3F80;
    static const uint16_t       DEVICE_ID_BITS_START        = 0x3FC0;
    static const uint16_t       PROGRAM_FLASH_MEMORY_START  = 0x4000;

    static const uint8_t        RSTDISBL        = 0;
    static const uint8_t        WDTON           = 1;
    static const uint8_t        CKOUT           = 2;

    static const uint8_t        NVLB1           = 0;
    static const uint8_t        NVLB2           = 1;

    static SPISettings          spiSettings;
    static int                  resetPin;
};

extern TPIClass TPI;

#endif // TPI_H
