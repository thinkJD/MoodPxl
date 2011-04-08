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


extern unsigned short rf12_trans(unsigned short wert);					// transfer 1 word to/from module
extern void rf12_init(void);											// initialize module
extern void rf12_setfreq(unsigned short freq);							// set center frequency
extern void rf12_setbaud(unsigned short baud);							// set baudrate
extern void rf12_setpower(unsigned char power, unsigned char mod);		// set transmission settings
extern void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi);	// set receiver settings
extern void rf12_txdata(unsigned char *data, unsigned char number);		// transmit number of bytes from array
extern void rf12_rxdata();		// receive number of bytes into array
extern void rf12_ready(void);											// wait until FIFO ready (to transmit/read data)
void rf12_getData(uint8_t* text);
void rf_data_reset();
uint8_t rf12_getStatus();
void rf12_Poll();

#define RF12FREQ(freq)	((freq-430.0)/0.0025)							// macro for calculating frequency value out of frequency in MHz

typedef struct 
{
	char Status;
	uint8_t Length;
	uint8_t Count;
    char Adress;      
    char Data[10]; // Text, maximal TEXT_LEN Zeichen lang
} rf_data;

#define rf12_data_status_empty 0	//Puffer bereit sum empfang
#define rf12_data_status_nio 	1	//Header oder Datencheck nio
#define rf12_data_status_ready 2	//Puffer bereit zum lesen
#define rf12_data_status_progress 3	//Mudul empfängt gerade
