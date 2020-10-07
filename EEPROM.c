#include "xc.h"
#include "EEPROM.h"



//1 bájt küldése és fogadása
uint8_t WriteSPI1(uint8_t i)
{   
    SPI1BUF = i;			// buffer írása küldésre
    while( !SPI1STATbits.SPIRBF);	// várakozás az átvitel
                                    // befejezéséig
    return SPI1BUF;    		// beérkez? adat kiolvasása
}

// EEPROM és SPI inicializálása
// SPI perifériákat össze kell rendelni a lábakkal!!!
void EEInit(void) 	
{
    TCSEE = 0;		// CS1 kimenet
    CSEE = 1;		// EEPROM elengedése
    
    //SPI inicializálása 2MHz-es órajellel
    SPI1CON1 = 0x013A; // master mód, CKP=0, CKE=1, 8-bites, 1:2, 1:4
    SPI1STAT = 0x8000; // SPI engedélyezése, státuszok törlése
    
    // SPI perifériákat össze kell rendelni a lábakkal!!!
}

// státusz regiszter olvasása
uint8_t EEReadSR(void)
{
    uint8_t i;	
    CSEE = 0;               // EEPROM kiválasztása
    WriteSPI1(SEE_RDSR);  	// státusz regiszter olvasása
    i = WriteSPI1(0);		// küldés/fogadás
    CSEE = 1;               // EEPROM elengedése
    return i;
}

// írás engedélyezése
void EEWriteEnable(void)
{
	CSEE = 0;               // EEPROM kiválasztása
	WriteSPI1(SEE_WREN);	// írás engedélyezése parancs
	CSEE = 1;               // EEPROM elengedése
}

// írás engedélyezése
void EEWriteDisable(void)
{
	CSEE = 0;               // EEPROM kiválasztása
	WriteSPI1(SEE_WREN);	// írás engedélyezése parancs
	CSEE = 1;               // EEPROM elengedése
}

// várakozás az írás befejezésére
void EEEndOfWriteProcess(void)
{
    while(EEReadSR() & 0x01);	// írás folyamatának vége (WIP)
}

// írás és blokk védelem törlése
void EEClearProtect(void)
{
    EEWriteEnable();		// írás engedélyezése
    CSEE = 0;               // EEPROM kiválasztása
    WriteSPI1(SEE_WRSR);  	// státusz regiszter írása
    WriteSPI1(0);           // védelem törlése
    CSEE = 1;               // EEPROM elengedése
    EEEndOfWriteProcess();  // várakozás az írás folyamatának a végére (WIP)
}
// írás és blokk védelem beállítása
void EESetProtect(void)
{
    EEWriteEnable();		// írás engedélyezése
    CSEE = 0;               // EEPROM kiválasztása
    WriteSPI1(SEE_WRSR);  	// státusz regiszter írása
    WriteSPI1(0x8C);		// írás(0x80) és/vagy blokk(0x0C) védelem
    CSEE = 1;               // EEPROM elengedése
    EEEndOfWriteProcess();  // várakozás az írás folyamatának a végére (WIP)
}

// 16 bites cím tartalmának olvasása
uint8_t EERead(uint16_t address)
{
    uint8_t val;
    CSEE = 0;                       // EEPROM kiválasztása
    WriteSPI1(SEE_READ);            // olvasás parancs
    WriteSPI1(address>>8);          // a cím fels? része (MSB)
    WriteSPI1(address & 0x00ff);	// a cím alsó része (LSB)
    val = WriteSPI1(0);     // dummy érték küldése/érték beolvasása
    CSEE = 1;               // EEPROM elengedése
    return val;
}

// 16 bites címt?l n adat olvasása data bufferbe
void EEReadN(uint16_t address, uint8_t *data, uint8_t n)
{
    int i;
    CSEE = 0;                       // EEPROM kiválasztása
    WriteSPI1(SEE_READ);            // olvasás parancs
    WriteSPI1(address>>8);          // a cím fels? része (MSB)
    WriteSPI1(address & 0x00ff);	// a cím alsó része (LSB)
    for(i = 0; i < n; i++)
        data[i] = WriteSPI1(0);	// dummy érték küldése/érték beolvasása
    CSEE = 1;                   // EEPROM elengedése
}

//Adat írása a 16 bites címre
void EEWrite(uint16_t address, uint8_t data)
{
    EEWriteEnable();                //Írás engedélyezése
    CSEE = 0;                       // EEPROM kiválasztása
    WriteSPI1(SEE_WRITE);           // írás parancs
    WriteSPI1(address>>8);          // a cím fels? része (MSB)
    WriteSPI1(address & 0x00ff);	// a cím alsó része (LSB)
    WriteSPI1(data);	// az adat küldése
    CSEE = 1;			// EEPROM elengedése
    EEEndOfWriteProcess();          // várakozás az írás folyamatának a végére (WIP)
}



//Adat írása a 16 bites címt?l n adattal egy bufferb?l
void EEWriteN(uint16_t address, uint8_t *data, uint8_t n)
{
    int i=0;   
    EEWriteEnable();                // írás engedélyezése
    CSEE = 0;                       // EEPROM kiválasztása
    WriteSPI1(SEE_WRITE);           // írás parancs
    WriteSPI1(address>>8);          // a cím fels? része (MSB)
    WriteSPI1(address & 0x00ff);	// a cím alsó része (LSB)
    for(i=0; i < n; i++)
        WriteSPI1( *data++ );       // az adat küldése
    CSEE = 1;                       // EEPROM elengedése
    EEEndOfWriteProcess();          // várakozás az írás folyamatának a végére (WIP)
}