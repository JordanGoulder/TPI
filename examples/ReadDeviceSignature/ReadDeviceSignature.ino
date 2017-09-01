#include <TPI.h>

void setup()
{
    Serial.begin(9600);

    if (TPI.begin()) {
        uint32_t deviceSignature = TPI.readDeviceSignature();
        TPI.end();

        Serial.print("Device Signature: ");
        Serial.println(deviceSignature, HEX);
    } else {
        Serial.println("Error: Unable to read device signature.");
    }
}

void loop()
{
}
