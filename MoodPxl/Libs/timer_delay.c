/*
    timer_delay implement eight 1sec Timebases
    
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
#include <stdlib.h>
#include "timer_delay.h"

//Deklaration der Variablen
uint8_t timeout = 0;
uint8_t run=0;
uint8_t compare[8] 	 = {0,0,0,0,0,0,0,0};
uint8_t td_timers[8] = {0,0,0,0,0,0,0,0};

//Die ISR wird im Sekundentakt aufgerufen.
//Es wird geprüft, ob ein oder mehrere Timer abgelaufen sind,
//falls ja werden die entsprechenden Flags gesetzt.
ISR(TIMER1_COMPA_vect) {
	
	if (run & (1<<0)) 
	{
		td_timers[0]++;
		if (compare[0] == td_timers[0] ) 
		{
			timeout |= (1<<0);
			run &= ~(1<<0);
		}
	}

	if (run & (1<<1)) 
	{
		td_timers[1]++;
		if (compare[1] == td_timers[1])
		{	
			timeout |= (1<<1);
			run &= ~(1<<1);
		}
	}
	
	if (run & (1<<2))
	{
		td_timers[2]++;
		if (compare[2] == td_timers[2])
		{
			timeout |= (1<<2);
			run &= ~(1<<2);
		}
	}

	if (run & (1<<3))
	{
		td_timers[3]++;
		if (compare[3] == td_timers[3])
		{
			timeout |= (1<<3);
			run &= ~(1<<3);
		}
	}
	
	if (run & (1<<4))
	{
		td_timers[4]++;
		if (compare[4] == td_timers[4])
		{
			timeout |= (1<<4);
			run &= ~(1<<4);
		}
	}
	
	if (run & (1<<5))
	{
		td_timers[5]++;
		if (compare[5] == td_timers[5])
		{
			timeout |= (1<<5);
			run &= ~(1<<5);
		}
	}
	
	if (run & (1<<6))
	{
		td_timers[6]++;
		if (compare[6] == td_timers[6])
		{
			timeout |= (1<<6);
			run &= ~(1<<6);
		}
	}
	
	if (run & (1<<7))
	{
		td_timers[7]++;
		if (compare[7] == td_timers[7])
		{
			timeout |= (1<<7);
			run &= ~(1<<7);
		}
	}
}

//Initialisiert den Timer
void td_init() {
	TCNT1 = (uint16_t)0;		//Zälregister 16bit Timer 3 zurücksetzen
	OCR1A = (uint16_t)62500;	//Nächster interupt in 1s
	TIMSK |= (1<<OCIE1A);		//Interrupt freischalten
	TCCR1B |= (1<<CS12) | (1<<WGM12);
}

//Setzt einen Timer
//index = Timer, welcher gesetzt werden soll
//sec	= Zeit in Sekunden maximal 0xFF
void td_setTimer(uint8_t index, uint8_t sec) {
	if (index > 7) index = 7;
	
	td_timers[index] = 0;
	
	compare[index] = sec;
	timeout &= ~(1<<index);
	run |= (1<<index);			
}

//Muss zyklisch aufgerufen werden um zu prüfen ob der
//Timer mit dem übergebenen Index abgelaufen ist.
uint8_t td_timeout(uint8_t index) {
	if (timeout & (1<<index))
	{
		
		timeout &= ~(1<<index);
		return 1;
	}
	
	return 0;
}

