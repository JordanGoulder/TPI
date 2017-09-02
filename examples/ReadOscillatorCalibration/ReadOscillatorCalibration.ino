#include <TPI.h>

static const int RESET_PIN = 10;

void setup()
{
    Serial.begin(9600);

    if (TPI.begin(RESET_PIN)) {
        uint32_t oscillatorCalibration = TPI.oscillatorCalibration();
        TPI.end();

        Serial.print("Oscillator Calibration: 0x");
        Serial.println(oscillatorCalibration, HEX);
    } else {
        Serial.println("Error: Unable to read oscillator calibration.");
    }
}

void loop()
{
}
