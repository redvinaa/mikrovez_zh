#include "xc.h"
#include "EEPROM.h"



//1 b�jt k�ld�se �s fogad�sa
uint8_t WriteSPI1(uint8_t i)
{   
    SPI1BUF = i;			// buffer �r�sa k�ld�sre
    while( !SPI1STATbits.SPIRBF);	// v�rakoz�s az �tvitel
                                    // befejez�s�ig
    return SPI1BUF;    		// be�rkez? adat kiolvas�sa
}

// EEPROM �s SPI inicializ�l�sa
// SPI perif�ri�kat �ssze kell rendelni a l�bakkal!!!
void EEInit(void) 	
{
    TCSEE = 0;		// CS1 kimenet
    CSEE = 1;		// EEPROM elenged�se
    
    //SPI inicializ�l�sa 2MHz-es �rajellel
    SPI1CON1 = 0x013A; // master m�d, CKP=0, CKE=1, 8-bites, 1:2, 1:4
    SPI1STAT = 0x8000; // SPI enged�lyez�se, st�tuszok t�rl�se
    
    // SPI perif�ri�kat �ssze kell rendelni a l�bakkal!!!
}

// st�tusz regiszter olvas�sa
uint8_t EEReadSR(void)
{
    uint8_t i;	
    CSEE = 0;               // EEPROM kiv�laszt�sa
    WriteSPI1(SEE_RDSR);  	// st�tusz regiszter olvas�sa
    i = WriteSPI1(0);		// k�ld�s/fogad�s
    CSEE = 1;               // EEPROM elenged�se
    return i;
}

// �r�s enged�lyez�se
void EEWriteEnable(void)
{
	CSEE = 0;               // EEPROM kiv�laszt�sa
	WriteSPI1(SEE_WREN);	// �r�s enged�lyez�se parancs
	CSEE = 1;               // EEPROM elenged�se
}

// �r�s enged�lyez�se
void EEWriteDisable(void)
{
	CSEE = 0;               // EEPROM kiv�laszt�sa
	WriteSPI1(SEE_WREN);	// �r�s enged�lyez�se parancs
	CSEE = 1;               // EEPROM elenged�se
}

// v�rakoz�s az �r�s befejez�s�re
void EEEndOfWriteProcess(void)
{
    while(EEReadSR() & 0x01);	// �r�s folyamat�nak v�ge (WIP)
}

// �r�s �s blokk v�delem t�rl�se
void EEClearProtect(void)
{
    EEWriteEnable();		// �r�s enged�lyez�se
    CSEE = 0;               // EEPROM kiv�laszt�sa
    WriteSPI1(SEE_WRSR);  	// st�tusz regiszter �r�sa
    WriteSPI1(0);           // v�delem t�rl�se
    CSEE = 1;               // EEPROM elenged�se
    EEEndOfWriteProcess();  // v�rakoz�s az �r�s folyamat�nak a v�g�re (WIP)
}
// �r�s �s blokk v�delem be�ll�t�sa
void EESetProtect(void)
{
    EEWriteEnable();		// �r�s enged�lyez�se
    CSEE = 0;               // EEPROM kiv�laszt�sa
    WriteSPI1(SEE_WRSR);  	// st�tusz regiszter �r�sa
    WriteSPI1(0x8C);		// �r�s(0x80) �s/vagy blokk(0x0C) v�delem
    CSEE = 1;               // EEPROM elenged�se
    EEEndOfWriteProcess();  // v�rakoz�s az �r�s folyamat�nak a v�g�re (WIP)
}

// 16 bites c�m tartalm�nak olvas�sa
uint8_t EERead(uint16_t address)
{
    uint8_t val;
    CSEE = 0;                       // EEPROM kiv�laszt�sa
    WriteSPI1(SEE_READ);            // olvas�s parancs
    WriteSPI1(address>>8);          // a c�m fels? r�sze (MSB)
    WriteSPI1(address & 0x00ff);	// a c�m als� r�sze (LSB)
    val = WriteSPI1(0);     // dummy �rt�k k�ld�se/�rt�k beolvas�sa
    CSEE = 1;               // EEPROM elenged�se
    return val;
}

// 16 bites c�mt?l n adat olvas�sa data bufferbe
void EEReadN(uint16_t address, uint8_t *data, uint8_t n)
{
    int i;
    CSEE = 0;                       // EEPROM kiv�laszt�sa
    WriteSPI1(SEE_READ);            // olvas�s parancs
    WriteSPI1(address>>8);          // a c�m fels? r�sze (MSB)
    WriteSPI1(address & 0x00ff);	// a c�m als� r�sze (LSB)
    for(i = 0; i < n; i++)
        data[i] = WriteSPI1(0);	// dummy �rt�k k�ld�se/�rt�k beolvas�sa
    CSEE = 1;                   // EEPROM elenged�se
}

//Adat �r�sa a 16 bites c�mre
void EEWrite(uint16_t address, uint8_t data)
{
    EEWriteEnable();                //�r�s enged�lyez�se
    CSEE = 0;                       // EEPROM kiv�laszt�sa
    WriteSPI1(SEE_WRITE);           // �r�s parancs
    WriteSPI1(address>>8);          // a c�m fels? r�sze (MSB)
    WriteSPI1(address & 0x00ff);	// a c�m als� r�sze (LSB)
    WriteSPI1(data);	// az adat k�ld�se
    CSEE = 1;			// EEPROM elenged�se
    EEEndOfWriteProcess();          // v�rakoz�s az �r�s folyamat�nak a v�g�re (WIP)
}



//Adat �r�sa a 16 bites c�mt?l n adattal egy bufferb?l
void EEWriteN(uint16_t address, uint8_t *data, uint8_t n)
{
    int i=0;   
    EEWriteEnable();                // �r�s enged�lyez�se
    CSEE = 0;                       // EEPROM kiv�laszt�sa
    WriteSPI1(SEE_WRITE);           // �r�s parancs
    WriteSPI1(address>>8);          // a c�m fels? r�sze (MSB)
    WriteSPI1(address & 0x00ff);	// a c�m als� r�sze (LSB)
    for(i=0; i < n; i++)
        WriteSPI1( *data++ );       // az adat k�ld�se
    CSEE = 1;                       // EEPROM elenged�se
    EEEndOfWriteProcess();          // v�rakoz�s az �r�s folyamat�nak a v�g�re (WIP)
}