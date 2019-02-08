// * Pull in dependencies
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>

// * Define constants
#define RIFD_NUMBER_OF_READERS 2
#define RFID_RESET_PIN 10
#define RFID_ONE_SELECT_PIN 11
#define RFID_TWO_SELECT_PIN 6

#define ONBOARD_LED 13

#define NEOPIXEL_PIN 12
#define NEOPIXEL_TOTAL_LEDS 8

// * Instantiate our classes
MFRC522 mfrc522[RIFD_NUMBER_OF_READERS];
byte RFID_selectPins[] = {RFID_ONE_SELECT_PIN, RFID_TWO_SELECT_PIN};

Adafruit_NeoPixel bar = Adafruit_NeoPixel(NEOPIXEL_TOTAL_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// * setup pokemon RFID tags
String bulbasaur = "04 60 d0 4a e6 4c 81";
String charmander = "04 78 d2 4a e6 4c 81";
String squirtle = "04 59 d0 4a e6 4c 81";
String pikachu = "04 60 d1 4a e6 4c 81";

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

bool readACardOnThePreviousLoop = false;

void loop()
{
    for (uint8_t reader = 0; reader < RIFD_NUMBER_OF_READERS; reader++)
    {
        if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial())
        {
            Serial.print(F("Reader "));
            Serial.print(reader);
            Serial.print(F(": Card UID:"));
            dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
        }
    }

    // // * Look for new cards
    // if (!mfrc522_ONE.PICC_IsNewCardPresent() && !mfrc522_TWO.PICC_IsNewCardPresent())
    // {
    //     if (readACardOnThePreviousLoop != true)
    //     {
    //     }
    //     readACardOnThePreviousLoop = false;
    //     return;
    // }

    // // * Select one of the cards
    // if (!mfrc522_ONE.PICC_ReadCardSerial() && !mfrc522_TWO.PICC_ReadCardSerial())
    // {
    //     return;
    // }

    // // * Dump debug info about the card; PICC_HaltA() is automatically called
    // mfrc522_ONE.PICC_DumpToSerial(&(mfrc522_ONE.uid));
    // mfrc522_TWO.PICC_DumpToSerial(&(mfrc522_TWO.uid));
    // return;

    // captureUID();
    // Serial.println("Got tag id:");
    // Serial.println(content.substring(1));

    // // ! Note that we need to start reading the sub string at index 1 instead of 0
    // // ! otherwise we get a space at the beginning
    // if (content.substring(1) == targetTag1)
    // {
    //     Serial.println("found tag 1");
    //     readACardOnThePreviousLoop = true;
    // }
    // else if (content.substring(1) == targetTag2)
    // {
    //     Serial.println("found tag 2");
    //     readACardOnThePreviousLoop = true;
    // }
    // else if (content.substring(1) == charmander)
    // {

    //     readACardOnThePreviousLoop = true;
    // }
    // else if (content.substring(1) == bulbasaur)
    // {

    //     readACardOnThePreviousLoop = true;
    // }
    // else if (content.substring(1) == squirtle)
    // {
    //     readACardOnThePreviousLoop = true;
    // }
    // else if (content.substring(1) == pikachu)
    // {
    //     readACardOnThePreviousLoop = true;
    // }
    // else
    // {
    //     Serial.println("no tag match found");
    //     readACardOnThePreviousLoop = false;
    // }
}

void captureUID()
{
    // content = "";
    // for (byte i = 0; i < mfrc522.uid.size; i++)
    // {
    //     // Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //     // Serial.print(mfrc522.uid.uidByte[i], HEX);

    //     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    //     content.concat(String(mfrc522.uid.uidByte[i], HEX));
    // }
    // Serial.println();
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

void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x16 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println("");
}