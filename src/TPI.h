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
    static void             sstcs   (uint8_t address);
    static void             skey    ();

    static const bool       NO_POST_INC = false;
    static const bool       POST_INC    = true;

    static const uint8_t    TPISR   = 0x01;
    static const uint8_t    TPIPCR  = 0x02;
    static const uint8_t    TPIIR   = 0x0F;
    static const uint8_t    NVMEN   = 1;
};

extern TPIClass TPI;

#endif // TPI_H
