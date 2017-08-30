#include <TPI.h>

void setup()
{
    Serial.begin(9600);

    TPI.begin();
    uint32_t deviceSignature = TPI.readDeviceSignature();
    TPI.end();

    Serial.print("Device Signature: ");
    Serial.println(deviceSignature, HEX);
}

void loop()
{
}
