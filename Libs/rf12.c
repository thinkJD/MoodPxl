/*
    MoodPxl a fully configurable, remote controlled, moodlamp.
    Copyright (C) 2011  Jan-Daniel Georgens jd.georgens@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"
#include "rf12.h"
#include <util/delay.h>
#include "uart.h"


#define UART_BAUD_RATE 57600
#define F_CPU 16000000

#define RF_PORT	PORTB
#define RF_DDR	DDRB
#define RF_PIN	PINB

#define SDI		5
#define SCK		7
#define CS		0
#define SDO		6

volatile rf_data strRX;
uint8_t INT_Status;
uint8_t i;

unsigned short rf12_trans(unsigned short wert)
{	
	unsigned short werti=0;
	unsigned char i;

	cbi(RF_PORT, CS);
	for (i=0; i<16; i++)
	{	if (wert&32768)
			sbi(RF_PORT, SDI);
		else
			cbi(RF_PORT, SDI);
		werti<<=1;
		if (RF_PIN&(1<<SDO))
			werti|=1;
		sbi(RF_PORT, SCK);
		wert<<=1;
		_delay_us(0.3);
		cbi(RF_PORT, SCK);
	}
	sbi(RF_PORT, CS);
	return werti;
}

void rf12_init(void)
{
	RF_DDR=(1<<SDI)|(1<<SCK)|(1<<CS);
	RF_PORT=(1<<CS);
	
     _delay_ms(100);			// wait until POR done

	rf12_trans(0xC0E0);			// AVR CLK: 10MHz
	rf12_trans(0x80D7);			// Enable FIFO
	rf12_trans(0xC2AB);			// Data Filter: internal
	rf12_trans(0xCA81);			// Set FIFO mode
	rf12_trans(0xE000);			// disable wakeuptimer
	rf12_trans(0xC800);			// disable low duty cycle
	rf12_trans(0xC4F7);			// AFC settings: autotuning: -10kHz...+7,5kHz

	//Einschalten des Receivers	
	rf12_trans(0x82C8);			// RX on
	rf12_trans(0xCA83);			// enable FIFO         
	rf12_trans(0x0000);			//Status lesen
	
	strRX.Status = 0;
	
	//Interrupt 
	MCUCR |= (1 << ISC11);	//Trigger auf fallende Flanke
	GICR |= (1 << INT1);	//Int aktivieren
	
}

void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi)
{
	rf12_trans(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
}

void rf12_setfreq(unsigned short freq)
{	if (freq<96)				// 430,2400MHz
		freq=96;
	else if (freq>3903)			// 439,7575MHz
		freq=3903;
	rf12_trans(0xA000|freq);
}

void rf12_setbaud(unsigned short baud)
{
	if (baud<663)
		return;
	if (baud<5400)					// Baudrate= 344827,58621/(R+1)/(1+CS*7)
		rf12_trans(0xC680|((43104/baud)-1));
	
	if (baud==19200)
     	rf12_trans(0xC611);
	//else
	//	rf12_trans(0xC600|((344828UL/baud)-1));
	
}

void rf12_setpower(unsigned char power, unsigned char mod)
{	
	rf12_trans(0x9800|(power&7)|((mod&15)<<4));
}

void rf12_ready(void)
{	
	cbi(RF_PORT, CS);
	while (!(RF_PIN&(1<<SDO))); // wait until FIFO ready
}

void rf12_txdata(unsigned char *data, unsigned char number)
{	
	unsigned char i;
	rf12_trans(0x8238);			// TX on
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB8AA);
	rf12_ready();
	rf12_trans(0xB82D);
	rf12_ready();
	rf12_trans(0xB8D4);
	for (i=0; i<number; i++)
	{		rf12_ready();
		rf12_trans(0xB800|(*data++));
	}
	rf12_ready();
	rf12_trans(0x8208);			// TX off
}

/*
Hier werden die Interrupts des Funkmoduls behandelt. 
*/
ISR(INT1_vect)
{
	/*
	Das Statusregister wird gelesen. RFM12 schiebt nach der ersten empfangenen 0 die Daten
	aus dem Puffer. RGIT und FFIT teilen sich das selbe Statusbit.
	Da die Niederwertigen bits zuerst ausgegeben werden, müssen sie gedreht werden.
	Uns interessiert also das Highbyte genauer das höchstwertige Bit des Highbyte.
	*/
		
	
	// Wenn das "Daten im FiFo"-Flag gesetzt ist, wird empfangen.
	uint8_t temp = (rf12_trans(0x0000)>>8);
  	if (temp & (1 << 7)) //FFIT Interrupt wurde ausgeloest.
  	{
  		/*
		Implementiert ein einfaches Funkprotokoll.
		Die Daten werden in folgendem Format gesendet:
		2x Preambel 0xAA (Synchronisiert den Empfänger)
    	1x Syncpattern 0x2DD4 (Aktiviert den empfangspuffer des Moduls)
    	1x LengthByte (Gibt die Länge der Nutzdaten an)
    	1x Headercheck
    	nx Daten ...
    	1x Datacheck
		*/
		
		//Datenempfang läuft
  		if(strRX.Status == rf12_data_status_progress)
  		{
  			//Übertragung abgeschlossen, wenn die Länge - das Längenbit
  			//sebst erreicht ist.
  			if (strRX.Count == (strRX.Length + 1))
  			{
  				strRX.Status = rf12_data_status_ready;
				strRX.Data[strRX.Count - 3] = '\0'; //Ende
				
				/*
				Checksumme über die Daten prüfen
				*/
				
				//FIFO Reset
				rf12_trans(0xCA81);
				rf12_trans(0xCA83);
				
				goto END;
  			}

  			if (strRX.Count > 2)
  			{
  				rf12_ready();
  				strRX.Data[strRX.Count - 3] = rf12_trans(0xB000);
  				strRX.Count++;
  				//uart1_putc(strRX.Data[strRX.Count - 4]);
  			}

  			//Header Check
  			if (strRX.Count == 2)
  			{
  				rf12_ready();
  				uint8_t Checksumme = rf12_trans(0xB000);
				/*
				Check IO?
				*/
				strRX.Count++;
  			}
			
			//Adressbyte lesen
  			if (strRX.Count == 1)
  			{
  				rf12_ready();
				strRX.Adress = rf12_trans(0xB000);
				strRX.Count++;
  			}
  		}
  		
  		//Empfang beginnt
  		if (strRX.Status == rf12_data_status_empty)
  		{
  			strRX.Status = rf12_data_status_progress;
			rf12_ready();
			//Warum funktioniert das?
			//Der Data variable vom typ Char wird der Short zugewiesen, welcher
			//vom Modul empfangen wurde. Da nur die letzten 8 bit die Daten enthalten, wird der 
			//Rest einfach abgeschnitten. Das ist kein Problem.
			//Sicher gige das auch etwas elegenter.
			strRX.Length = rf12_trans(0xB000); 	//Empfang des Längenbytes
  			strRX.Count++;
  		}
  		
		END:
		asm volatile ("nop");
	}
}
void rf_data_reset()
{
	//cli();	//Diese Methode muss atomar behandelt werden. 

	strRX.Count = 0;
	strRX.Length = 0;
	strRX.Adress = 0;
	
	//Örks ... wenn das mal nicht ekelhaft ist.
	//Da muss es doch eine bessere Möglichkeit geben ...
	uint8_t i;
	for (i=0; i<20; i++) strRX.Data[i] = 0;
	
	strRX.Status = rf12_data_status_empty;
	
	//sei();	//Interrupts wieder an
	
}

void rf12_getData(uint8_t *text)
{
	memmove(text, strRX.Data, 10);
}

uint8_t rf12_getStatus()
{
	return strRX.Status;
}
