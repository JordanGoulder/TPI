#include <TPI.h>

static const int RESET_PIN = 10;

void setup()
{
    Serial.begin(9600);

    if (TPI.begin(RESET_PIN)) {
        uint32_t deviceSignature = TPI.deviceSignature();
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
