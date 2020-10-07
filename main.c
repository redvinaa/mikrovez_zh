#include <xc.h>		/* altalanos header a PIC24 csaladhoz */
#include <stdio.h>      //sprintf miatt
#include <stdlib.h>     
#include <string.h>     //memset
#include "EEPROM.h"

// PIC24FJ256GB108 - uMOGI2 Configuration Bit Settings
// CONFIG3
#pragma config WPFP = WPFP511           // Write Protection Flash Page Segment Boundary (Highest Page (same as page 170))
#pragma config WPDIS = WPDIS            // Segment Write Protection Disable bit (Segmented code protection disabled)
#pragma config WPCFG = WPCFGDIS         // Configuration Word Code Page Protection Select bit (Last page(at the top of program memory) and Flash configuration words are not protected)
#pragma config WPEND = WPENDMEM         // Segment Write Protection End Page Select bit (Write Protect from WPFP to the last page of memory)

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (HS oscillator mode selected)
#pragma config DISUVREG = ON            // Internal USB 3.3V Regulator Disable bit (Regulator is enabled)
#pragma config IOL1WAY = ON             // IOLOCK One-Way Set Enable bit (Write RP Registers Once)
#pragma config OSCIOFNC = OFF           // Primary Oscillator Output Function (OSCO functions as CLKO (FOSC/2))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-safe Clock Monitor are disabled)
#pragma config FNOSC = PRIPLL           // Oscillator Select (Primary oscillator (XT, HS, EC) with PLL module (XTPLL,HSPLL, ECPLL))
#pragma config PLL_96MHZ = ON           // 96MHz PLL Disable (Enabled)
#pragma config PLLDIV = DIV3            // USB 96 MHz PLL Prescaler Select bits (Oscillator input divided by 3 (12MHz input))
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-speed start-up)disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR32             // WDT Prescaler (Prescaler ratio of 1:32)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Standard Watchdog Timer enabled,(Windowed-mode is disabled))
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config ICS = PGx2               // Comm Channel Select (Emulator functions are shared with PGEC2/PGED2)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

// Hardver tulajdonsagok:

// PLL utani orajel
#define SYS_FREQ		32000000L
#define FCY				SYS_FREQ/2

// Masodlagos orajel forras - orakvarc
#define F_SEC_OSC		32768

// Delay fuggvenyek
#define DELAY_MS(ms)	__delay_ms(ms);
#define DELAY_US(us)	__delay_us(us);
#include <libpic30.h>   // __delay_ms

// LED-ek
#define LED1		_LATG6
#define LED2		_LATG7
#define LED3		_LATG8
#define LED4		_LATG9
#define LEDR		_LATD15
#define LEDG		_LATF4
#define LEDB		_LATF5
#define R_LED()		{LEDR=1; LEDG=0; LEDB=0;}
#define G_LED()		{LEDR=0; LEDG=1; LEDB=0;}
#define B_LED()		{LEDR=0; LEDG=0; LEDB=1;}

// Nyomogombok
#define SW1			!_RC1
#define SW2			!_RC3
#define SW3			!_RE8
#define SW4			!_RE9

#define BOOL unsigned char

// LCD
#define putLCD(c) writeLCD(c, 1) //karakter k�ld�se
#define cmdLCD(d) writeLCD(d, 0) //parancs k�ld�se
#define LCDclr() writeLCD(0x01,0) //LCD t�rl�se
#define LCD2row() writeLCD(0xC0,0) //LCD a 2. sorba
#define LCDhome() writeLCD(0x02,0) //LCD az 1. sorba

//magyar �kezetes karakterek
const unsigned char hu_char[] = {
0x02,0x04,0x0E,0x01,0x0F,0x11,0x0F,0x00, //�
0x02,0x04,0x0E,0x11,0x1F,0x10,0x0E,0x00, //�
0x02,0x04,0x0C,0x04,0x04,0x04,0x0E,0x00, //�
0x02,0x04,0x0E,0x11,0x11,0x11,0x0E,0x00, //�
0x02,0x04,0x11,0x11,0x11,0x13,0x0D,0x00, //�
0x0A,0x00,0x11,0x11,0x11,0x13,0x0D,0x00, //�
0x05,0x0A,0x11,0x11,0x11,0x13,0x0D,0x00, //�
0x05,0x0A,0x0E,0x11,0x11,0x11,0x0E,0x00, //�
};

void pulseE(void);
void configLCD(void);
void toLCD(char c);
void writeLCD(uint8_t c, BOOL rs);
void initLCD();
void inithuLCD(void);
void putsLCD(char *s);


// #########################################################################
// #########################################################################








float timer = 0;
const float TIMER_MAX = 8.;
enum ProgState {WORKING, STOPPED} state;

void initT1()
{
    _T1IF = 0; // flag
    _T1IE = 1; // enable
    // __builtin_write_OSCCONL(OSCCON | 0x02); // kulso orakvarc be
    PR1 = 6250; // TMR1 MAX
    TMR1 = 0; // belso counter
    T1CONbits.TCS = 0; // belso (int/ext)
    T1CONbits.TCKPS = 0b11; // elooszto (0->1, 1->8, 2->64, 3->256)
    T1CONbits.TON = 1; // timer modul inditasa
}


// ISR
void __attribute__((interrupt(auto_psv))) _T1Interrupt() {
    _T1IF = 0;
    timer += 0.1;
}










int main()
{
 //Inicializalas - setup
    
    //Orajel forras beallitasa
    // PLL kimenete
    CLKDIVbits.CPDIV = 0;    // 1:1 32MHz
    // megvarjuk amig a PLL modul vegez
    while(!OSCCONbits.LOCK) Nop();
	
    // szoftveres WDT kikapcsolasa
    RCONbits.SWDTEN = 0;
	
	// Orakvarc engedelyezese
	//__builtin_write_OSCCONL(OSCCON | 0x02);

	// Periferia - lab osszerendeles PPS (pp.135)
	//__builtin_write_OSCCONL(OSCCON & 0xbf);  //PPSUnLock;
	//
	//__builtin_write_OSCCONL(OSCCON | 0x40); //PPSLock;
    
	
    // Labak iranya	
	_TRISG6 = 0;	// LED1 kimenet
	_TRISG7 = 0;	// LED2 kimenet
	_TRISG8 = 0;	// LED3 kimenet
    _TRISG9 = 0;	// LED4 kimenet
    _TRISD15 = 0;	// LEDR kimenet
    _TRISF4 = 0;	// LEDG kimenet
    _TRISF5 = 0;	// LEDB kimenet
	_TRISC1 = 1;	// SW1 bemenet
	_TRISC3 = 1;	// SW2 bemenet 
	_TRISE8 = 1;	// SW3 bemenet
    _TRISE9 = 1;	// SW4 bemenet
	
	initLCD();                  //LCD modul inicializ�l�sa
    inithuLCD();                //Magyar karakterek bet�lt�se
    
    initT1();
    /*putsLCD("R�da Vince\nZ697LX");    //Sz�veg ki�rat�sa
    G_LED();
    DELAY_MS(1000);*/
    
    char LCD[80];
    
    state = WORKING;
    LCDclr();
    

 //V�gtelen ciklus - loop
    while(1)	
    {
        if (timer > TIMER_MAX) state = STOPPED;
        
        switch (state)
        {
            case WORKING:
                B_LED();
                    
                    LCDclr();
                    sprintf(LCD, "WORKING\ntimer = %3.1f", timer);
                    putsLCD(LCD);
                if (SW1)
                {
                    timer = 0;
                    DELAY_MS(20);
                    while (SW1)
                    {
                        Nop();
                        timer = 0;
                    }
                
                }
                break;
            case STOPPED:
                
                R_LED();
                LCDclr();
                putsLCD("STOPPED");
                DELAY_MS(2000);
                state = WORKING;
                timer = 0;
    
                break;
        }
        
        DELAY_MS(100);
    }
	
 return (0);
}










// #########################################################################
// #########################################################################



//LCD

//Strobe jel
void pulseE(void) {
    _LATF1=1; //E magas
    Nop(); Nop(); Nop();
    _LATF1=0; //E alacsony
}

//LCD l�bak konfigur�l�sa
void configLCD(void) {
    TRISE &= 0xff00; //D0-D7 kimenet
    _TRISG0 = 0; //RS kimenet
    _TRISG1 = 0; //RW kimenet
    _TRISF1 = 0; //E kimenet
    _LATG0 = 0; //RS alacsony
    _LATG1 = 0 ;//RW alacsony
    _LATF1 = 0; //E alacsony
}

//Adatvonalak haszn�lata
void toLCD(char c) {
	LATE= (LATE & 0xff00) | c;
}

// Adat vagy parancs k�ld�se, c adat , rs 0 - parancs, 1 - adat
void writeLCD(uint8_t c, BOOL rs)
{
    //foglalts�g figyel�se
    TRISE |= 0x00ff; //l�bak bemenetek
    _LATG1 = 1;      //Read
    _LATG0 = 0;
    BOOL bf;
    do {
	 _LATF1=1; //E magas
	 Nop(); Nop(); Nop(); bf=_RE7;
	 _LATF1=0; //E alacsony 
	} while (bf);
	TRISE &= 0xff00; //l�bak kimenetek
	_LATG1 = 0; //Write
		   
	_LATG0 = rs ? 1 : 0;   //adat vagy parancs
    toLCD(c); // 8 bit k�ld�se
    pulseE(); // strobe
}

void initLCD() {
   
    configLCD(); //az LCD vez�rl� vonalainak be�ll�t�sa

    DELAY_MS(500); //v�r az eszk�z be�ll�s�ra

    cmdLCD(0x38); // 2 soros display, 5x7 karakter
    cmdLCD(0x08); // dispaly off
    cmdLCD(0x01); // k�perny? t�rl�se, kurzor alaphelyzetbe �ll�t�s
    cmdLCD(0x06); // automatikus inkrement�l�s, nem l�pteti a kijelz?t
    cmdLCD(0x0C); // a display bekapcsol�sa; kurzor �s villog�s kikapcsolva

    DELAY_MS(3);
}


//--Ki�r egy karakterf�z�rt az LCD-re
void putsLCD(char *s) {
  while (*s) {
    char  c = *s;
      switch(c) {   //magyar karakterek cser�je a felt�lt�ttre
	    case '�':  c = 0x00; break;
	    case '�':  c = 0x01; break;
	    case '�':  c = 0x02; break;
	    case '�':  c = 0x03; break;
	    case '�':  c = 0x04; break;
	    case '�':  c = 0x05; break;
	    case '�':  c = 0x06; break;
	    case '�':  c = 0x07; break;
	    case '�':  c = 0xEF; break;	}
    if (c == '\n')
      LCD2row();      //Kurzor mozgat�sa a m�sodik sor elej�re
    else
      putLCD(c);      //Karakter ki�rat�sa
    s++;
  }
}

//Az LCD modulra magyar �kezetes karakterek felt�lt�se
void inithuLCD(void) {
    int i;
    cmdLCD(0x40);            // kurzor a CGRAM elej�re
    for(i=0; i<64; i++) {
      putLCD(hu_char[i]);    // defini�lhat� karakterek felt�lt�se
    }                        // �kezetes karakterekkel
    cmdLCD(0x80);            // kurzor vissza, a DDRAM elej�re
}