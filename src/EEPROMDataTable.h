#ifndef EEPROMDATATABLE_H
#define EEPROMDATATABLE_H

#include "DataTable.h"
#include <EEPROM.h>

class EEPROMDataTable : public DataTable 
{
public:
    /**
     * @brief Create a new Table object
     * 
     * @param fieldCount fixed number of columns (fields per registry)
     * @param maxMem max total hardware memory to be used (in bytes)
     */
    EEPROMDataTable(const uint8_t fieldCount, const unsigned int maxMem) : DataTable(fieldCount, maxMem) {}
    virtual ~EEPROMDataTable() {}

protected:

    // Wrappers for EEPROM, SPIFFS, etc
    virtual void beginHard(unsigned int hardMemSize) // FIXME: NOT USED
    {
        ; // nada
    };

    virtual void writeHard(unsigned int memPos, uint8_t data)
    {
        EEPROM.write(startMemory + memPos, data);
    };

    virtual uint8_t readHard(unsigned int memPos)
    {
        return EEPROM.read(startMemory + memPos);
    };


};

#endif