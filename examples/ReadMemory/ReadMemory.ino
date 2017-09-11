#include <TPI.h>
#include <ctype.h>

static const int RESET_PIN = 10;

uint8_t memoryBuffer[1024];

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    bool shouldCancel = true;
    char section = getMemorySection(shouldCancel);

    uint16_t address;
    uint16_t count = 0;

    if (!shouldCancel) {

        Serial.println();

        switch (section) {
            case 'a':
                address = 0x0000;
                count = 64;
                Serial.println(F("Registers (I/O Space)")); 
                break;

            case 'b':
                address = 0x0040;
                count = 32;
                Serial.println(F("SRAM")); 
                break;

            case 'c':
                address = 0x3F00;
                count = 1;
                Serial.println(F("NVM Lock Bits")); 
                break;

            case 'd':
                address = 0x3F40;
                count = 1;
                Serial.println(F("Configuration")); 
                break;

            case 'e':
                address = 0x3F80;
                count = 1;
                Serial.println(F("Calibration")); 
                break;

            case 'f':
                address = 0x3FC0;
                count = 3;
                Serial.println(F("Device ID")); 
                break;

            case 'g':
                address = 0x4000;
                count = 1024;
                Serial.println(F("Flash Program Memory")); 
                break;

            default:
                break;
        }

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

char getMemorySection(bool &cancel)
{
    while (true) {
        Serial.println();
        Serial.println(F("Which memory section would you like to read?"));
        Serial.println(F("    a - Registers (I/O Space)"));
        Serial.println(F("    b - SRAM"));
        Serial.println(F("    c - NVM Lock Bits"));
        Serial.println(F("    d - Configuration"));
        Serial.println(F("    e - Calibration"));
        Serial.println(F("    f - Device ID"));
        Serial.println(F("    g - Flash Program Memory"));
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
        } else if (ch >= 'a' && ch <= 'g'){
            return ch;
        } else {
            Serial.println(F(""));
            Serial.println(F("Invalid choice!"));
        }
    }
}

