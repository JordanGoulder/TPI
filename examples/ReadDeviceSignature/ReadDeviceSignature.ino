#include <TPI.h>

static const int RESET_PIN = 8;

void setup()
{
    Serial.begin(9600);

    if (TPI.begin(RESET_PIN)) {
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
