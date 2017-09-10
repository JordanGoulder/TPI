#include <TPI.h>

static const int RESET_PIN = 10;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    showNvmProtectionMode();

    bool shouldCancel = true;

    char mode = getNewNvmProtectionMode(shouldCancel);

    if (!shouldCancel) {
        performOperation(mode);
    }
}

void showNvmProtectionMode()
{
    if (TPI.begin(RESET_PIN)) {

        TPIClass::NvmProtectionMode mode = TPI.nvmProtectionMode();

        TPI.end();

        Serial.print(F("NVM Protection Mode: "));

        switch (mode) {
            case TPIClass::NvmProtectionMode::NO_PROTECTION:
                Serial.println(F("No protection"));
                break;

            case TPIClass::NvmProtectionMode::WRITE_PROTECTION:
                Serial.println(F("Write protection"));
                break;

            case TPIClass::NvmProtectionMode::WRITE_AND_VERIFY_PROTECTION:
                Serial.println(F("Write and Verify protection"));
                break;

            default:
                Serial.println(F("Undefined protection mode"));
                break;
        }

    } else {
        Serial.println(F("Error: Unable to read configuration."));
    }
}

char getNewNvmProtectionMode(bool &cancel)
{
    while (true) {
        Serial.println(F(""));
        Serial.println(F("Which NVM Protection Mode would you like to use?"));
        Serial.println(F("    a - No protection"));
        Serial.println(F("    b - Write protection"));
        Serial.println(F("    c - Write and Verify protection"));
        Serial.println(F(""));
        Serial.println(F("Enter your choice (x to cancel): "));

        int ch = -1;
        
        while (ch == -1) {
            while (Serial.available() == 0);

            ch = Serial.read();

            if (ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ') {
                ch = -1;
            }
        }

        cancel = false;

        if (ch == 'a' or ch == 'A') {
            return 'a';
        } else if (ch == 'b' or ch == 'B') {
            return 'b';
        } else if (ch == 'c' or ch == 'C') {
            return 'c';
        } else if (ch == 'x' or ch == 'X') {
            cancel = true;
            return ch;
        } else {
            Serial.println(F(""));
            Serial.println(F("Invalid choice!"));
        }
    }
}

void performOperation(char mode)
{
    bool success = false;

    if (TPI.begin(RESET_PIN))
    {
        switch (mode) {
            case 'a':
                success = TPI.setNvmProtectionMode(TPIClass::NvmProtectionMode::NO_PROTECTION);
                break;

            case 'b':
                success = TPI.setNvmProtectionMode(TPIClass::NvmProtectionMode::WRITE_PROTECTION);
                break;

            case 'c':
                success = TPI.setNvmProtectionMode(TPIClass::NvmProtectionMode::WRITE_AND_VERIFY_PROTECTION);
                break;

            default:
                break;
        }

        TPI.end();

        if (success) {
            Serial.println(F("Operation SUCCESS!"));
        } else {
            Serial.println(F("Operation FAILED!"));
        }
    }
}
