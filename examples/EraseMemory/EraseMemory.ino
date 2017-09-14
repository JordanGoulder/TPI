#include <TPI.h>
#include <ctype.h>

static const int RESET_PIN = 10;

static const int ERASE_CHIP = 0;
static const int ERASE_CONFIG = 1;
static const int ERASE_PROGRAM = 2;

uint8_t memoryBuffer[1024];

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    bool shouldCancel = true;
    char section = getOperation(shouldCancel);

    if (!shouldCancel) {

        Serial.println();

        switch (section) {
            case 'a':
                Serial.println(F("NVM Lock Bits")); 
                displayMemory(0x3F00, 1);
                break;

            case 'b':
                Serial.println(F("Configuration")); 
                displayMemory(0x3F40, 1);
                break;

            case 'c':
                Serial.println(F("Flash Program Memory")); 
                displayMemory(0x4000, 1024);
                break;

            case 'd':
                Serial.println(F("Erasing Chip")); 
                eraseMemory(ERASE_CHIP);
                break;

            case 'e':
                Serial.println(F("Erasing Configuration")); 
                eraseMemory(ERASE_CONFIG);
                break;

            case 'f':
                Serial.println(F("Erasing Flash Program Memory")); 
                eraseMemory(ERASE_PROGRAM);
                break;

            default:
                break;
        }
    }
}

void displayMemory(uint16_t address, uint16_t count)
{
    if (count) {
        if (TPI.begin(RESET_PIN)) {
            TPI.readMemory(address, memoryBuffer, count);        
            TPI.end();

            hexdump(address, memoryBuffer, count);

        } else {
            Serial.println(F("Error: Unable to read memory from device."));
        }
    }
}

void eraseMemory(uint16_t type)
{
    if (TPI.begin(RESET_PIN)) {
        
        bool success = false;
        if (type == ERASE_CHIP) {
            success = TPI.eraseChip();
        } else if (type == ERASE_CONFIG) {
            success = TPI.eraseConfigurationMemory();
        } else if (type == ERASE_PROGRAM) {
            success = TPI.eraseProgramMemory();
        }
        TPI.end();

        if (success) {
            Serial.println("Erase successful.");
        } else {
            Serial.println("Erase FAILED!!!");
        }

    } else {
        Serial.println(F("Error: Unable to read memory from device."));
    }
}

void hexdump(uint16_t address, uint8_t *buffer, uint16_t count)
{

    for (uint16_t i = 0; i < count; i++, address++) {

        if (i == 0 || (address & 0x000F) == 0) {
            uint16_t rowAddress = address & 0xFFF0;
            Serial.println();
            if (rowAddress < 0x10) {
                Serial.print(F("000"));
            } else if (rowAddress < 0x100) {
                Serial.print(F("00"));
            } else if (rowAddress < 0x1000) {
                Serial.print(F("0"));
            }
            Serial.print(rowAddress, HEX);
            Serial.print(" ");

            uint8_t offset = 16 - (address & 0x000F);
            while (offset++ & 0x0F) {
                Serial.print(F("-- "));
            }
        }

        if ((address & 0x0007) == 0) {
            Serial.print(F(" "));
        }

        if (buffer[i] < 0x10) Serial.print(F("0"));
        Serial.print(buffer[i], HEX);
        Serial.print(F(" "));

    }

    Serial.println();
}

char getOperation(bool &cancel)
{
    while (true) {
        Serial.println();
        Serial.println(F("What operation would you like to perform?"));
        Serial.println(F("    a - Display NVM Lock Bits"));
        Serial.println(F("    b - Display Configuration"));
        Serial.println(F("    c - Display Flash Program Memory"));
        Serial.println(F("    d - Erase Chip"));
        Serial.println(F("    e - Erase Configuration"));
        Serial.println(F("    f - Erase Flash Program Memory"));
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
        ch = tolower(ch);

        if (ch == 'x') {
            cancel = true;
            return ch;
        } else if (ch >= 'a' && ch <= 'f'){
            return ch;
        } else {
            Serial.println();
            Serial.println(F("Invalid choice!"));
        }
    }
}

