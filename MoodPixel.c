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

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "Libs/uart.h"
#include "Libs/rf12.h"
#include "Libs/led_pwm.h"

#define UART_BAUD_RATE 57600
#define F_CPU 16000000



int main(void)
{
	//Initialisieren
	uart1_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );  //Uart 1 initialisieren (Debug)
//	ds1820_init();	//Temperatursensor initialisieren
	led_init();		//LED Initialisieren

	//Funkmodul initialisieren
	rf12_init();					// ein paar Register setzen (z.B. CLK auf 10MHz)
	rf12_setfreq(RF12FREQ(433.25));	// Sende/Empfangsfrequenz auf 433,92MHz einstellen
	rf12_setbandwidth(4, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
	rf12_setbaud(19200);			// 19200 baud
	rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120kHz Frequenzshift

	sei();		//Interrupts aktivieren


	struct rgb Color;
	led_on();
	
	Color.Red = 255;
	Color.Green = 0;
	Color.Blue = 0;	
	set_led_color(&Color);
	_delay_ms(500);
	
	
	uint8_t temp = 0;
	
	struct hsv color2;
		
	color2.hue = 0;
	color2.saturation = 100;
	color2.value = 100;
	uint16_t x;
	uint8_t* buf_temp;
	
	//Mainloop
	while(23)
	{
		//Wenn Puffer lesebereit ist
		if (rf12_getStatus() == rf12_data_status_ready)
		{
			rf12_getData(buf_temp); 
			Comand(&buf_temp);
			rf_data_reset();
		}
	}
}
		
		

		

		
	

void command(uint8_t* buf)
{
	/*
	Folgende Kommands sind möglich:
	0x00	=	MoodPxl einschalten
	0x01	=	MoodPxl ausschalten

	0x02	=	MoodPxl Demo 1
	0x03	=	MoodPxl Demo 2
	0x04	=	MoodPxl Demo 3
			
	0x10	=	Fader_Init Param: int Zeit; byte Rot; 
						byte Grün; byte Gelb; int Verweilzeit;
	0x11	=	Farbe_Konstant
	0x02	=	Programm starten Paran: 
	*/
	uint8_t length = buf[0];
	uint8_t Comand = buf[1];
		
	switch (command) {
		case 0x00:
			led_off();
			break:
				
		case 0x01:
			led_on();
			break;
				
		case 0x02:
			Demo1();
			break;
				
		case 0x10:
			fader_init();
			break;
	}		
}


#define r_Demo1 0	//Demo 1 läuft
#define r_Demo2 1	//Demo 2 läuft
#define r_Demo3 2	//Demo 3 läuft
#define r_com	3	//Interpreter läuft

#define i_Demo1	0	//Demo 1 initialisiert
#define i_Demo2 1	//Demo 2 initialisiert
#define i_Demo3 2	//Demo 3 initialisiert
#define i_com	3	//Interpreter initialisiert



uint8_t Lock;	//Blokiert das gleichzeitige ausführen mehrerer Programme
uint8_t Init;	//Speichert ob ein Programm initialisiert wurde

void Demo1()	
{
	if (temp == 0)
	{
		Color.Red = 50;
		Color.Green = 0;
		Color.Blue = 100;
		rgb_fade_int(Color, 200);
		temp++;
		uart1_putc('1');
	}
	
	if(temp == 2)
	{
		Color.Red = 0;
		Color.Green = 70;
		Color.Blue = 255;
		rgb_fade_int(Color, 200);
		temp++;
		uart1_putc('2');
	}
	
	if(temp == 4)
	{
		Color.Red = 55;
		Color.Green = 255;
		Color.Blue = 0;
		rgb_fade_int(Color, 200);
		temp++;
		uart1_putc('3');
	}
	
	if(temp == 6)
	{
		Color.Red = 200;
		Color.Green = 0;
		Color.Blue = 150;
		rgb_fade_int(Color, 500);
		temp++;
		uart1_putc('4');
	}
	
	if(temp == 8)
	{
		Color.Red = 250;
		Color.Green =200;
		Color.Blue = 0;
		rgb_fade_int(Color, 500);
		temp++;
		uart1_putc('8');
	}
	
	if(temp == 10)
	{
		Color.Red = 5;
		Color.Green = 40;
		Color.Blue = 50;
		rgb_fade_int(Color, 500);
		temp++;
		uart1_putc('4');
	}
	
	if(temp == 12)
	{
		Color.Red = 10;
		Color.Green =255;
		Color.Blue = 170;
		rgb_fade_int(Color, 500);
		temp++;
		uart1_putc('8');
	}
	

	if (fade_state() == fader_idle)
	{
		uart1_putc('X');
		temp++;
		if (temp == 14) temp = 0;
	}
}


void Demo_2()
{
	for (x = 0; x < 360; x++)
	{
		color2.hue=x;
		hsv2rgb(&color2, &Color);
		set_led_color(&Color);
		_delay_ms(50);
	}
}

	










