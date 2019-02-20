// * Pull in dependencies
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>

#include "RFIDData.h"

// * Define constants
#define RIFD_NUMBER_OF_READERS 2

#define RFID_RESET_PIN 10
#define RFID_ONE_SELECT_PIN 5
#define RFID_TWO_SELECT_PIN 6

#define ONBOARD_LED 13

#define NEOPIXEL_PIN 12
#define NEOPIXEL_TOTAL_LEDS 8

// * Instantiate our classes
MFRC522 mfrc522[RIFD_NUMBER_OF_READERS];
RFIDData RFIDDataCache[RIFD_NUMBER_OF_READERS];

// * Create our target RFID data instances
int charmanderId[7] = {0x04, 0x78, 0xd2, 0x4a, 0xe6, 0x4c, 0x81};
int squirtleId[7] = {0x04, 0x59, 0xd0, 0x4a, 0xe6, 0x4c, 0x81};
int pikachuId[7] = {0x04, 0x60, 0xd1, 0x4a, 0xe6, 0x4c, 0x81};
int bulbasaurId[7] = {0x04, 0x60, 0xd0, 0x4a, 0xe6, 0x4c, 0x81};

RFIDData charmander(charmanderId);
RFIDData bulbasaur(bulbasaurId);
RFIDData squirtle(squirtleId);
RFIDData pikachu(pikachuId);

byte RFID_selectPins[] = {RFID_ONE_SELECT_PIN, RFID_TWO_SELECT_PIN};

Adafruit_NeoPixel bar = Adafruit_NeoPixel(NEOPIXEL_TOTAL_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// * Colors
uint32_t RED = bar.Color(255, 0, 0);
uint32_t GREEN = bar.Color(0, 255, 0);
uint32_t BLUE = bar.Color(0, 0, 255);
uint32_t YELLOW = bar.Color(255, 255, 0);
uint32_t BLACK = bar.Color(0, 0, 0);

enum BarSections
{
    TOP_SECTION = 0,
    BOTTOM_SECTION,
    FULL_SECTION,
};

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    // while (!Serial)
    //     ; // Do nothing if no serial port is opened

    // * setup devices
    pinMode(ONBOARD_LED, OUTPUT);
    setupRFIDCardReader();
    setupNeopixelBar();

    // * Let everyone know we're ready
    signalReady();
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void setupRFIDCardReader()
{
    SPI.begin(); // Init SPI bus

    for (uint8_t i = 0; i < RIFD_NUMBER_OF_READERS; i++)
    {
        mfrc522[i].PCD_Init(RFID_selectPins[i], RFID_RESET_PIN);
        Serial.print("Reader ");
        Serial.print(i);
        Serial.print(F(": "));
        mfrc522[i].PCD_DumpVersionToSerial();
    }
}

void signalReady()
{
    fillBar(bar.Color(50, 200, 50), 100);
    delay(1000);
    fillBar(bar.Color(0, 0, 0), 100);
    delay(1000);

    digitalWrite(ONBOARD_LED, HIGH);
    delay(100);
    digitalWrite(ONBOARD_LED, LOW);
    delay(100);
    digitalWrite(ONBOARD_LED, HIGH);
    delay(100);
    digitalWrite(ONBOARD_LED, LOW);
    delay(100);
    digitalWrite(ONBOARD_LED, HIGH);
    delay(100);
    digitalWrite(ONBOARD_LED, LOW);
    delay(100);
}

bool holdOverDeadLoop[2] = {false, false};

void loop()
{
    // Serial.println("*********************************** Top of loop ");
    for (uint8_t reader = 0; reader < RIFD_NUMBER_OF_READERS; reader++)
    {
        if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
        {
            holdOverDeadLoop[reader] = true;
            Serial.print(F("Reader "));
            Serial.print(reader);
            Serial.print(F(": Card UID:"));
            dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);

            // | capture new id in new instance
            // | compare to cached id
            // | if they're different, replace the cached RFIDData instance with the new instance
            // | if they're the same do nothing
            // | fire logic based on cached reader
            RFIDData newCard(mfrc522[reader]);

            if (RFIDDataCache[reader] == newCard)
            {
                Serial.println("");
                Serial.println("");
                Serial.print("=== Reader");
                Serial.print(reader);
                Serial.println(" ===");

                Serial.println("=== already captured this card!! ===");
                handleCardLogic(reader);
                Serial.println("====================================");
            }
            else
            {
                Serial.println("");
                Serial.println("");
                Serial.print("+++ Reader");
                Serial.print(reader);
                Serial.println(" +++");
                Serial.println("!!! we got a new card here !!!");
                RFIDDataCache[reader] = newCard;
                handleCardLogic(reader);
                Serial.println("++++++++++++++++++++++++++++++");
            }
        }
        else
        {
            if (holdOverDeadLoop[reader] == false)
            {
                BarSections section = reader == 0 ? BOTTOM_SECTION : TOP_SECTION;
                fillBar(BLACK, section);
            }
            holdOverDeadLoop[reader] = false;
        }
    }
}

void handleCardLogic(int readerNumber)
{
    BarSections section = readerNumber == 0 ? BOTTOM_SECTION : TOP_SECTION;

    if (RFIDDataCache[readerNumber] == charmander)
    {
        Serial.println("Charmander!!");
        fillBar(RED, section);
    }
    if (RFIDDataCache[readerNumber] == bulbasaur)
    {
        Serial.println("Bulbasaur??!!!");
        fillBar(GREEN, section);
    }
    if (RFIDDataCache[readerNumber] == squirtle)
    {
        Serial.println("Squirtle :O");
        fillBar(BLUE, section);
    }
    if (RFIDDataCache[readerNumber] == pikachu)
    {
        Serial.println("Pika pika!");
        fillBar(YELLOW, section);
    }
}

void setupNeopixelBar()
{
    Serial.println("Starting neopixel bar");
    bar.begin();
    bar.setBrightness(127);
    bar.show();
}

void fillBar(uint32_t color)
{
    for (int i = 0; i < NEOPIXEL_TOTAL_LEDS; i++)
    {
        bar.setPixelColor(i, color);
    }
    bar.show();
}
void fillBar(uint32_t color, uint32_t pauseDuration)
{
    for (int i = 0; i < NEOPIXEL_TOTAL_LEDS; i++)
    {
        bar.setPixelColor(i, color);

        bar.show();
        delay(pauseDuration);
    }
}

void fillBar(uint32_t color, BarSections section)
{
    uint8_t start, end;
    if (section == TOP_SECTION)
    {
        start = 4;
        end = 7;
    }
    else if (section == BOTTOM_SECTION)
    {
        start = 0;
        end = 3;
    }
    else if (section == FULL_SECTION)
    {
        start = 0;
        end = 7;
    }

    for (int i = start; i <= end; i++)
    {
        bar.setPixelColor(i, color);
    }
    bar.show();
}

void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println("");
}