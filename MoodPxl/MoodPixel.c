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
#include "MoodPixel.h"
#include "Libs/OneWire.h"
#include "Libs/timer_delay.h"
#include "Libs/rf12.h"
#include "Libs/led_pwm.h"
#include "Libs/script.h"


struct fan_control fan = {fan_init,0,0,0};


int main(void)
{
	//Initialisieren der Komponenten
	led_init();		//LED Initialisieren
	td_init();		//Zeitbasis initialisieren
	script_init();	//Sriptengine initialisieren
	
	//Funkmodul initialisieren
	rf12_init();					
	rf12_setfreq(RF12FREQ(433.25));	// 433,92MHz
	rf12_setbandwidth(4, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
	rf12_setbaud(19200);			// 19200 baud
	rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120kHz Frequenzshift

	sei();		//Interrupts aktivieren

	uint8_t buf_temp[10];
	
	//Mainloop
	while(23)
	{
		//Zustandsmaschinen aktualisieren
		script_tick();
		rgb_fade_tick();

		//Prüfen ob ein neues Datenframe empfangen wurde.
		if (rf12_getStatus() == rf12_data_status_ready)
		{
			rf12_getData(buf_temp); //Daten abholen
			rf_data_reset();		//Empfangspuffer zurücksetzen
			command(buf_temp);		//Empfangenes Kommando ausführen
		}
	}
}	
		
//Kommunikation
void command(uint8_t *buf) {
	uint8_t m_comand = buf[0];
	struct rgb rgb_color;
	struct hsv hsv_color;
		
	switch (m_comand) {
		case mpxl_cmd_off:
			led_off();
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
			
	   case mpxl_cmd_fade:
			rgb_color.Red = buf[1];
 			rgb_color.Green = buf[2];
 			rgb_color.Blue = buf[3];
 			rgb_fade_int(rgb_color, buf[4]);
 			
		case mpxl_cmd_script:
			script_handler(buf);
			break;
	}		
}

