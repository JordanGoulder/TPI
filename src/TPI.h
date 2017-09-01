#ifndef TPI_H
#define TPI_H

#include <Arduino.h>

class TPIClass
{

public:
    static bool             begin();
    static void             end();
    static uint32_t         readDeviceSignature();

private:
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
    static void             skey    ();

    static void             write   (uint8_t value);
    static uint8_t          read    ();

    static const bool       NO_POST_INC = false;
    static const bool       POST_INC    = true;

    static const uint8_t    TPISR   = 0x00;
    static const uint8_t    TPIPCR  = 0x02;
    static const uint8_t    TPIIR   = 0x0F;
    static const uint8_t    NVMEN   = 1;

    static const uint8_t    SLD         = 0x20;
    static const uint8_t    SLD_POSTINC = 0x24;
    static const uint8_t    SST         = 0x60;
    static const uint8_t    SST_POSTINC = 0x64;
    static const uint8_t    SSTPR_LO    = 0x68;
    static const uint8_t    SSTPR_HI    = 0x69;
    static const uint8_t    SIN         = 0x10;
    static const uint8_t    SOUT        = 0x90;
    static const uint8_t    SLDCS       = 0x80;
    static const uint8_t    SSTCS       = 0xC0;
    static const uint8_t    SKEY        = 0xE0;

};

extern TPIClass TPI;

#endif // TPI_H
