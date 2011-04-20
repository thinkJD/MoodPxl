#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "timer_delay.h"


uint8_t timeout = 0;
uint8_t run=0;
uint8_t compare[8] 	 = {0,0,0,0,0,0,0,0};
uint8_t td_timers[8] = {0,0,0,0,0,0,0,0};

ISR(TIMER1_COMPA_vect)
{
	
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

void td_init()
{
	TCNT1 = (uint16_t)0;		//Zälregister 16bit Timer 3 zurücksetzen
	OCR1A = (uint16_t)62500;	//Nächster interupt in 1s
	TIMSK |= (1<<OCIE1A);		//Interrupt freischalten
	TCCR1B |= (1<<CS12) | (1<<WGM12);
}

void td_setTimer(uint8_t index, uint8_t sec)
{
	if (index > 7) index = 7;
	
	td_timers[index] = 0;
	
	compare[index] = sec;
	timeout &= ~(1<<index);
	run |= (1<<index);			
}

uint8_t td_timeout(uint8_t index)
{
	if (timeout & (1<<index))
	{
		
		timeout &= ~(1<<index);
		return 1;
	}
	
	return 0;
}

