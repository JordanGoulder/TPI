#include <TPI.h>

static const int RESET_PIN = 10;

void setup()
{
    Serial.begin(9600);

    if (TPI.begin(RESET_PIN)) {

        Serial.print("External Reset disable:   ");
        Serial.println(TPI.externalResetDisable() ? "ON" : "OFF");

        Serial.print("Watchdog Timer always on: ");
        Serial.println(TPI.watchdogTimerAlwaysOn() ? "ON" : "OFF");

        Serial.print("System Clock output:      ");
        Serial.println(TPI.systemClockOutput() ? "ON" : "OFF");

        TPI.end();

    } else {
        Serial.println("Error: Unable to read configuration.");
    }
}

void loop()
{
}
