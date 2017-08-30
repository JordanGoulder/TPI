#ifndef TPI_H
#define TPI_H

#include <Arduino.h>

class TPIClass
{

public:
    static bool begin();
    static void end();
    static uint32_t readDeviceSignature();

};

extern TPIClass TPI;

#endif // TPI_H
