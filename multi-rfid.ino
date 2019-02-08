// * Pull in dependencies
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>
#include <SD.h>

// * Define constants for the music maker wing

// * Define constants for the RFID reader
#define ONE_SS_PIN 5   // Configurable, see typical pin layout above
#define ONE_RST_PIN 10 // Configurable, see typical pin layout above
#define TWO_SS_PIN 6   // Configurable, see typical pin layout above
#define TWO_RST_PIN 11 // Configurable, see typical pin layout above

#define ONBOARD_LED 13

// * Instantiate our classes
MFRC522 mfrc522_ONE(ONE_SS_PIN, ONE_RST_PIN); // Create MFRC522 instance
MFRC522 mfrc522_TWO(TWO_SS_PIN, TWO_RST_PIN); // Create MFRC522 instance

// * setup variables for the RFID tags we're looking for
String content = "";
String targetTag1 = "04 72 d2 4a e6 4c 81";
String targetTag2 = "04 52 c5 4a e6 4c 80";

// * setup pokemon RFID tags
String bulbasaur = "04 60 d0 4a e6 4c 81";
String charmander = "04 78 d2 4a e6 4c 81";
String squirtle = "04 59 d0 4a e6 4c 81";
String pikachu = "04 60 d1 4a e6 4c 81";

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    // while (!Serial)
    // ; // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    setupRFIDCardReader();

    signalReady();
    Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}

void setupRFIDCardReader()
{
    SPI.begin();                           // Init SPI bus
    mfrc522_ONE.PCD_Init();                // Init MFRC522
    mfrc522_ONE.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
    mfrc522_TWO.PCD_Init();                // Init MFRC522
    mfrc522_TWO.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
                                           // Serial.println(mfrc522.PCD_GetAntennaGain());
                                           // mfrc522.PCD_SetAntennaGain(mfrc522.PCD_RxGain.RxGain_48dB);
                                           // Serial.println(mfrc522.PCD_GetAntennaGain());
}

void signalReady()
{
    pinMode(ONBOARD_LED, OUTPUT);
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

    // * Look for new cards
    if (!mfrc522_ONE.PICC_IsNewCardPresent() && !mfrc522_TWO.PICC_IsNewCardPresent())
    {
        if (readACardOnThePreviousLoop != true)
        {
        }
        readACardOnThePreviousLoop = false;
        return;
    }

    // * Select one of the cards
    if (!mfrc522_ONE.PICC_ReadCardSerial() && !mfrc522_TWO.PICC_ReadCardSerial())
    {
        return;
    }

    // * Dump debug info about the card; PICC_HaltA() is automatically called
    mfrc522_ONE.PICC_DumpToSerial(&(mfrc522_ONE.uid));
    mfrc522_TWO.PICC_DumpToSerial(&(mfrc522_TWO.uid));
    return;

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
void printDirectory(File dir, int numTabs)
{
    while (true)
    {

        File entry = dir.openNextFile();
        if (!entry)
        {
            // no more files
            //Serial.println("**nomorefiles**");
            break;
        }
        for (uint8_t i = 0; i < numTabs; i++)
        {
            Serial.print('\t');
        }
        Serial.print(entry.name());
        if (entry.isDirectory())
        {
            Serial.println("/");
            printDirectory(entry, numTabs + 1);
        }
        else
        {
            // files have sizes, directories do not
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}