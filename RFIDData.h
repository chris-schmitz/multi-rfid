#ifndef RFIDDATACACHE_H
#define RFIDDATACACHE_H

#include <MFRC522.h>

struct RFIDData
{
    static const uint8_t idLength = 7;
    int idArray[idLength] = {0};
    uint8_t currentIndex = 0;

    RFIDData() {}

    RFIDData(MFRC522 &reader)
    {
        this->captureCardUid(reader);
    }

    void captureCardUid(MFRC522 &reader)
    {
        for (uint8_t i = 0; i < reader.uid.size; i++)
        {
            this->addIdElement(reader.uid.uidByte[i]);
        }
    }

    void addIdElement(int element)
    {
        this->idArray[this->currentIndex] = element;
        this->currentIndex++;
    }

    bool idsMatch(RFIDData payload2)
    {
        if (this->currentIndex != payload2.currentIndex)
        {
            return false;
        }

        for (uint8_t i = 0; i < this->currentIndex; i++)
        {
            if (this->idArray[i] != payload2.idArray[i])
            {
                return false;
            }
        }

        return true;
    }

    void clearId()
    {
        for (uint8_t i = 0; i < this->idLength; i++)
        {
            this->idArray[i] = 0;
        }
        this->currentIndex = 0;
    }
};
#endif