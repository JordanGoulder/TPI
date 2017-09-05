#include <TPI.h>

static const int RESET_PIN = 10;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    showConfiguration();

    bool shouldCancel = true;

    char configBit = getConfigBit(shouldCancel);

    bool set;

    if (!shouldCancel) {
        set = shouldSet(shouldCancel);
    }

    if (!shouldCancel) {
        performOperation(configBit, set);
    }
}

void showConfiguration()
{
    Serial.println(F(""));
    Serial.println(F("Current Configuration"));

    if (TPI.begin(RESET_PIN)) {

        Serial.print(F("    External Reset disable:   "));
        Serial.println(TPI.externalResetDisable() ? F("ON") : F("OFF"));

        Serial.print(F("    Watchdog Timer always on: "));
        Serial.println(TPI.watchdogTimerAlwaysOn() ? F("ON") : F("OFF"));

        Serial.print(F("    System Clock output:      "));
        Serial.println(TPI.systemClockOutput() ? F("ON") : F("OFF"));

        TPI.end();

    } else {
        Serial.println(F("Error: Unable to read configuration."));
    }
}

char getConfigBit(bool &cancel)
{
    while (true) {
        Serial.println("");
        Serial.println("Which configuration bit would you like to change?");
        Serial.println("    r - External Reset disable");
        Serial.println("    w - Watchdog Timer always on");
        Serial.println("    s - System Clock output");
        Serial.println("");
        Serial.println("Enter your choice (x to cancel): ");

        int ch = -1;
        
        while (ch == -1) {
            while (Serial.available() == 0);

            ch = Serial.read();

            if (ch == '\r' || ch == '\n' || ch == '\t' || ch == ' ') {
                ch = -1;
            }
        }

        cancel = false;

        if (ch == 'r' or ch == 'R') {
            return ch;
        } else if (ch == 'w' or ch == 'W') {
            return ch;
        } else if (ch == 's' or ch == 'S') {
            return ch;
        } else if (ch == 'x' or ch == 'X') {
            cancel = true;
            return ch;
        } else {
            Serial.println("");
            Serial.println("Invalid choice!");
        }
    }
}

bool shouldSet(bool &cancel)
{
    while (true) {
        Serial.println(F(""));
        Serial.println(F("Which operation would you like to perform?"));
        Serial.println(F("    s - Set configuration bit"));
        Serial.println(F("    c - Clear configuration bit"));
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

        if (ch == 's' or ch == 'S') {
            return true;
        } else if (ch == 'c' or ch == 'C') {
            return false;
        } else if (ch == 'x' or ch == 'X') {
            cancel = true;
            return false;
        } else {
            Serial.println(F(""));
            Serial.println(F("Invalid choice!"));
        }
    }
}

void performOperation(char configBit, bool set)
{
    bool success = false;

    if (TPI.begin(RESET_PIN))
    {
        switch (configBit) {
            case 'r':
                success = TPI.setExternalResetDisable(set);
                break;

            case 'w':
                success = TPI.setWatchdogTimerAlwaysOn(set);
                break;

            case 's':
                success = TPI.setSystemClockOutput(set);
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
