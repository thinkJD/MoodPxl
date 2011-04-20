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
#include "Libs/uart.h"		//Zu debugzwecken
#include "Libs/OneWire.h"	//
#include "Libs/rf12.h"
#include "Libs/led_pwm.h"


#define UART_BAUD_RATE 57600
#define F_CPU 16000000


void command(uint8_t *buf);



void adjust_fanspeed()
{
	const uint16_t fansteps[6] = {900, 700, 500, 300, 100, 50};
	start_meas();
	uint16_t temp = read_meas();
	uint8_t	Hight = (temp>>4);
	uint8_t Lowt  = (temp << 12) / 6553;

		



	char buffer[10];
	itoa( Hight, buffer, 10);
	uart1_puts(buffer);
	uart1_puts(".");
	itoa( Lowt, buffer,10);
	uart1_puts(buffer);

		
	
}

int main(void)
{
	//Initialisieren
	uart1_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );  //Uart 1 initialisieren (Debug)
	led_init();		//LED Initialisieren

	//Funkmodul initialisieren
	rf12_init();					// ein paar Register setzen (z.B. CLK auf 10MHz)
	rf12_setfreq(RF12FREQ(433.25));	// Sende/Empfangsfrequenz auf 433,92MHz einstellen
	rf12_setbandwidth(4, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
	rf12_setbaud(19200);			// 19200 baud
	rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120kHz Frequenzshift

	sei();		//Interrupts aktivieren

	set_fanspeed(1024);
	
	uint8_t buf_temp[10];
	uint16_t temp = 0;
	//Mainloop
	while(23)
	{
		//Wenn Zeichen empfangen wurden, wird der fertiggelesene 
		//Emofangspuffer an die Command-Funktion übergeben
		if (rf12_getStatus() == rf12_data_status_ready)
		{
			rf12_getData(buf_temp); 
			rf_data_reset();
			command(buf_temp);
		}

		
		
		//adjust_fanspeed();
	}
}	
		
#define mpxl_cmd_on		0x00
#define mpxl_cmd_off	0x01
#define mpxl_cmd_setRGB	0x02
#define mpxl_cmd_setHSV 0x03

void command(uint8_t *buf)
{
	/*
	Folgende Kommands sind möglich:
	0x00	=	MoodPxl einschalten
	0x01	=	MoodPxl ausschalten
	0x02	=	RGB Farbe setzen Param:
						byte Rot; byte Grün; Byte Gelb;
	0x03	=	HSV Farbe setzen Param:
						word hue; byte saturation; byte Value
			
	0x10	=	Fader_Init Param: int Zeit; byte Rot; 
						byte Grün; byte Gelb; int Verweilzeit;
	0x02	=	Programm starten Paran: 
	*/
		
	uint8_t m_comand = buf[0];
	struct rgb rgb_color;
	struct hsv hsv_color;
		
	switch (m_comand) {
		case mpxl_cmd_off:
			led_sig_ok();
			//led_off();
			break;
				
		case mpxl_cmd_on:
			led_on();
			break;
				
		case mpxl_cmd_setRGB:
			rgb_color.Red = buf[1];
			rgb_color.Green = buf[2];
			rgb_color.Blue = buf[3];
			set_led_color(&rgb_color);
			break;
			
		case mpxl_cmd_setHSV:
			hsv_color.hsv[0] = buf[1];
			hsv_color.hsv[1] = buf[2];
			hsv_color.saturation = buf[3];
			hsv_color.value = buf[4];
			hsv2rgb(&hsv_color,&rgb_color);
			set_led_color(&rgb_color);
			break;
			
	}		
}


// #define r_Demo1 0	//Demo 1 läuft
// #define r_Demo2 1	//Demo 2 läuft
// #define r_Demo3 2	//Demo 3 läuft
// #define r_com	3	//Interpreter läuft
// 
// #define i_Demo1	0	//Demo 1 initialisiert
// #define i_Demo2 1	//Demo 2 initialisiert
// #define i_Demo3 2	//Demo 3 initialisiert
// #define i_com	3	//Interpreter initialisiert
// 
// 
// 
// uint8_t Lock;	//Blokiert das gleichzeitige ausführen mehrerer Programme
// uint8_t Init;	//Speichert ob ein Programm initialisiert wurde
// 
// void Demo1()	
// {
// 	if (temp == 0)
// 	{
// 		Color.Red = 50;
// 		Color.Green = 0;
// 		Color.Blue = 100;
// 		rgb_fade_int(Color, 200);
// 		temp++;
// 		uart1_putc('1');
// 	}
// 	
// 	if(temp == 2)
// 	{
// 		Color.Red = 0;
// 		Color.Green = 70;
// 		Color.Blue = 255;
// 		rgb_fade_int(Color, 200);
// 		temp++;
// 		uart1_putc('2');
// 	}
// 	
// 	if(temp == 4)
// 	{
// 		Color.Red = 55;
// 		Color.Green = 255;
// 		Color.Blue = 0;
// 		rgb_fade_int(Color, 200);
// 		temp++;
// 		uart1_putc('3');
// 	}
// 	
// 	if(temp == 6)
// 	{
// 		Color.Red = 200;
// 		Color.Green = 0;
// 		Color.Blue = 150;
// 		rgb_fade_int(Color, 500);
// 		temp++;
// 		uart1_putc('4');
// 	}
// 	
// 	if(temp == 8)
// 	{
// 		Color.Red = 250;
// 		Color.Green =200;
// 		Color.Blue = 0;
// 		rgb_fade_int(Color, 500);
// 		temp++;
// 		uart1_putc('8');
// 	}
// 	
// 	if(temp == 10)
// 	{
// 		Color.Red = 5;
// 		Color.Green = 40;
// 		Color.Blue = 50;
// 		rgb_fade_int(Color, 500);
// 		temp++;
// 		uart1_putc('4');
// 	}
// 	
// 	if(temp == 12)
// 	{
// 		Color.Red = 10;
// 		Color.Green =255;
// 		Color.Blue = 170;
// 		rgb_fade_int(Color, 500);
// 		temp++;
// 		uart1_putc('8');
// 	}
// 	
// 
// 	if (fade_state() == fader_idle)
// 	{
// 		uart1_putc('X');
// 		temp++;
// 		if (temp == 14) temp = 0;
// 	}
// }
// 
// 
// void Demo_2()
// {
// 	for (x = 0; x < 360; x++)
// 	{
// 		color2.hue=x;
// 		hsv2rgb(&color2, &Color);
// 		set_led_color(&Color);
// 		_delay_ms(50);
// 	}
// }

	










