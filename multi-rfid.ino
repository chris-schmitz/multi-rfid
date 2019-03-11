// * Pull in dependencies
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>

#include "RFIDData.h"

// * Define constants
#define RIFD_NUMBER_OF_READERS 2
#define RFID_RESET_PIN 10

// * Reader select pins
#define RFID_ONE_SELECT_PIN 14
#define RFID_TWO_SELECT_PIN 15
#define RFID_THREE_SELECT_PIN 17
#define RFID_FOUR_SELECT_PIN 18

byte RFID_selectPins[] = {RFID_ONE_SELECT_PIN, RFID_TWO_SELECT_PIN, RFID_THREE_SELECT_PIN, RFID_FOUR_SELECT_PIN};

#define ONBOARD_LED 13

#define NEOPIXEL_PIN 12
#define NEOPIXEL_TOTAL_LEDS 8

#define VERBOSE_SERIAL true

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

enum pokemon
{
    BULBASAUR = 1,
    CHARMANDER,
    SQUIRTLE,
    PIKACHU
};

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
    while (!Serial)
        ; // Do nothing if no serial port is opened

    // * setup devices
    pinMode(ONBOARD_LED, OUTPUT);
    setupRFIDCardReader();
    setupNeopixelBar();

    // * Let everyone know we're ready
    signalReady();
    if (VERBOSE_SERIAL)
    {
        Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
    }
}

void setupRFIDCardReader()
{
    SPI.begin(); // Init SPI bus

    for (uint8_t i = 0; i < RIFD_NUMBER_OF_READERS; i++)
    {
        // Serial.print("Select pin: ");
        // Serial.println(RFID_selectPins[i]);
        mfrc522[i].PCD_Init(RFID_selectPins[i], RFID_RESET_PIN);
        if (VERBOSE_SERIAL)
        {
            Serial.print("Reader ");
            Serial.print(i);
            Serial.print(F(": "));
        }
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
    for (uint8_t reader = 0; reader < RIFD_NUMBER_OF_READERS; reader++)
    {
        // Serial.print("Reader: ");
        // Serial.println(reader);
        if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
        {
            Serial.print(F("Reader "));
            Serial.print(reader);
            // Show some details of the PICC (that is: the tag/card)
            Serial.print(F(": Card UID:"));
            dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
            Serial.println();
            // Serial.print(F("PICC type: "));
            // MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
            // Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));

            // Halt PICC
            mfrc522[reader].PICC_HaltA();
            // Stop encryption on PCD
            mfrc522[reader].PCD_StopCrypto1();
        } //if (mfrc522[reader].PICC_IsNewC
        delay(100);
        continue;

        // * The numeric data we'll be sending to the laptop that contains the enum ID of
        // * each card read
        uint8_t payload = 0;

        if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
        {
            holdOverDeadLoop[reader] = true;
            uint8_t pokemon;

            if (VERBOSE_SERIAL)
            {
                Serial.print(F("Reader "));
                Serial.print(reader);
                Serial.print(F(": Card UID:"));
                dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
            }

            RFIDData newCard(mfrc522[reader]);

            if (RFIDDataCache[reader] == newCard)
            {
                if (VERBOSE_SERIAL)
                {
                    Serial.print("=== Reader");
                    Serial.print(reader);
                    Serial.println(" ===");
                    Serial.println("=== already captured this card!! ===");
                }
                pokemon = handleCardLogic(reader);
            }
            else
            {
                if (VERBOSE_SERIAL)
                {
                    Serial.print("+++ Reader");
                    Serial.print(reader);
                    Serial.println(" +++");
                    Serial.println("!!! we got a new card here !!!");
                }

                RFIDDataCache[reader] = newCard;
                pokemon = handleCardLogic(reader);
            }

            payload = payload * 10;      // * shift the payload decimal to the left
            payload = payload + pokemon; // * Add the current pokemon enum
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

        if (payload > 0)
        {
            Serial.println(payload); // * If we've captured some RFID data, write it out to the laptop
        }
    }
}

uint8_t handleCardLogic(int readerNumber)
{
    uint8_t pokemon = 0;

    BarSections section = readerNumber == 0 ? BOTTOM_SECTION : TOP_SECTION;

    if (RFIDDataCache[readerNumber] == charmander)
    {
        fillBar(RED, section);
        pokemon = CHARMANDER;
    }
    if (RFIDDataCache[readerNumber] == bulbasaur)
    {
        fillBar(GREEN, section);
        pokemon = BULBASAUR;
    }
    if (RFIDDataCache[readerNumber] == squirtle)
    {
        fillBar(BLUE, section);
        pokemon = SQUIRTLE;
    }
    if (RFIDDataCache[readerNumber] == pikachu)
    {
        fillBar(YELLOW, section);
        pokemon = PIKACHU;
    }

    return pokemon;
}

void setupNeopixelBar()
{
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