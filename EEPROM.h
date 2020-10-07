#ifndef EEPROM_HEADER
#define	EEPROM_HEADER

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

//EEPROM CS l�b
#define CSEE 	_LATD7	// CS1 l�b
#define TCSEE 	_TRISD7  // CS1 l�b ir�nya

//  25LC256 SPI EEPROM parancsai 
#define SEE_WRSR	1		// st�tusz regiszter �r�sa	
#define SEE_WRITE	2		// �r�s parancs
#define SEE_READ	3		// olvas�s parancs
#define SEE_WRDI	4 		// �r�s tilt�sa parancs
#define SEE_RDSR	5		// st�tusz regiszter olvas�sa
#define SEE_WREN	6		// �r�s enged�lyez�se parancs

// EEPROM �s SPI inicializ�l�s
void EEInit(void);
//1 b�jt k�ld�se �s fogad�sa
uint8_t WriteSPI1(uint8_t i);
// st�tusz regiszter olvas�sa
uint8_t EEReadSR(void);
// �r�s enged�lyez�se
void EEWriteEnable(void);
// �r�s tilt�sa
void EEWriteDisable(void);
// v�rakoz�s az �r�s befejez�s�re
void EEEndOfWriteProcess(void);
// �r�s �s blokk v�delem t�rl�se
void EEClearProtect(void);
// �r�s �s blokk v�delem be�ll�t�sa
void EESetProtect(void);
// 16 bites c�m tartalm�nak olvas�sa
uint8_t EERead(uint16_t address);
// 16 bites c�mt?l n adat olvas�sa data bufferbe
void EEReadN(uint16_t address, uint8_t *data, uint8_t n);
// adat �r�sa a 16 bites c�mre
void EEWrite(uint16_t address, uint8_t data);
// adat �r�sa a 16 bites c�mt?l n adattal egy bufferb?l
void EEWriteN(uint16_t address, uint8_t *data, uint8_t n);


#endif	/* XC_HEADER_TEMPLATE_H */
