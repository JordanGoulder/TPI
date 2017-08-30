#include "TPI.h"

bool TPIClass::begin()
{
    return true;
}

void TPIClass::end()
{
}

uint32_t TPIClass::readDeviceSignature()
{
    return 0x12345678;
}
