#include <TPI.h>

static const int RESET_PIN = 10;

void setup()
{
    Serial.begin(9600);

    if (TPI.begin(RESET_PIN)) {

        TPIClass::NvmProtectionMode mode = TPI.nvmProtectionMode();

        TPI.end();

        Serial.print("NVM Protection Mode: ");

        switch (mode) {
            case TPIClass::NvmProtectionMode::NO_PROTECTION:
                Serial.println("No protection");
                break;

            case TPIClass::NvmProtectionMode::WRITE_PROTECTION:
                Serial.println("Write protection");
                break;

            case TPIClass::NvmProtectionMode::WRITE_AND_VERIFY_PROTECTION:
                Serial.println("Write and Verify protection");
                break;

            default:
                Serial.println("Undefined protection mode");
                break;
        }

    } else {
        Serial.println("Error: Unable to read configuration.");
    }
}

void loop()
{
}
