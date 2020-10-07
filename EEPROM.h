#ifndef EEPROM_HEADER
#define	EEPROM_HEADER

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

//EEPROM CS láb
#define CSEE 	_LATD7	// CS1 láb
#define TCSEE 	_TRISD7  // CS1 láb iránya

//  25LC256 SPI EEPROM parancsai 
#define SEE_WRSR	1		// státusz regiszter írása	
#define SEE_WRITE	2		// írás parancs
#define SEE_READ	3		// olvasás parancs
#define SEE_WRDI	4 		// írás tiltása parancs
#define SEE_RDSR	5		// státusz regiszter olvasása
#define SEE_WREN	6		// írás engedélyezése parancs

// EEPROM és SPI inicializálás
void EEInit(void);
//1 bájt küldése és fogadása
uint8_t WriteSPI1(uint8_t i);
// státusz regiszter olvasása
uint8_t EEReadSR(void);
// írás engedélyezése
void EEWriteEnable(void);
// írás tiltása
void EEWriteDisable(void);
// várakozás az írás befejezésére
void EEEndOfWriteProcess(void);
// írás és blokk védelem törlése
void EEClearProtect(void);
// írás és blokk védelem beállítása
void EESetProtect(void);
// 16 bites cím tartalmának olvasása
uint8_t EERead(uint16_t address);
// 16 bites címt?l n adat olvasása data bufferbe
void EEReadN(uint16_t address, uint8_t *data, uint8_t n);
// adat írása a 16 bites címre
void EEWrite(uint16_t address, uint8_t data);
// adat írása a 16 bites címt?l n adattal egy bufferb?l
void EEWriteN(uint16_t address, uint8_t *data, uint8_t n);


#endif	/* XC_HEADER_TEMPLATE_H */
