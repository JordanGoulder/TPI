#include <TPI.h>
#include <ctype.h>

static const int RESET_PIN = 10;

uint8_t dataBuffer[32];
uint8_t memoryBuffer[512];

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    if(TPI.begin(RESET_PIN)) {
        TPI.readMemory(0x4000, memoryBuffer, sizeof(memoryBuffer));
        TPI.end();

        hexdump(0x4000, memoryBuffer, sizeof(memoryBuffer));

    } else {
        Serial.println("Error: Unable access device.");
    }

    int address = readAddressFromSerial();

    Serial.println();
    Serial.print("Wrting to address ");
    Serial.print(address, HEX);
    Serial.println();
    Serial.println("Enter data to write: ");

    String data = "";
    while (data.length() == 0) {

        while (!Serial.available());

        data = Serial.readString();
    }
    
    Serial.print("Writing data: ");
    Serial.println(data);

    data.getBytes(dataBuffer, sizeof(dataBuffer));

    if(TPI.begin(RESET_PIN)) {
        if (TPI.writeMemory(address, dataBuffer, data.length())) {
            Serial.println("Operation SUCCESSFUL.");
        } else {
            Serial.println("Operation FALIED!!!");
        }
        TPI.end();
    } else {
        Serial.println("Error: Unable access device.");
    }
}

int readAddressFromSerial()
{
    Serial.print("Enter the starting address: ");

    String inString;

    while (true) {
        while(Serial.available()) {
            int inChar = Serial.read();
            if (isDigit(inChar)) {
                inString += (char) inChar;
            }
            if (inChar == '\n') {
                return inString.toInt();
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
