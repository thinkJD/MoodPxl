#include "cc1100/cc1100.h"
#include "mpxlRF.h"
#include "timerfuncs.h"




void mpxl_send(unsigned char *buf, unsigned char length) {
	unsigned char sendbuffer[20];
	sendbuffer[0] = length + 2;	//Länge der Daten + Heeader
	sendbuffer[1] = 0x01;		//Adresse
	sendbuffer[2] = 0xFF; //sendbuffer[0]^sendbuffer[1]^0xFF;	//Checksumme
	
	int i;
	for (i=0;i<length;i++) 
	{
		sendbuffer[i+3] = buf[i];
	}
	
	RF_send(sendbuffer, sendbuffer[0]+1);
}

void setRGB(struct rgb color) {
	unsigned char buf[4];
	buf[0] = mpxl_cmd_setRGB;
	buf[1] = color.Red;
	buf[2] = color.Green;
	buf[3] = color.Blue;
	
	mpxl_send(buf, 4);
}

void setHSV(struct hsv color) {
	unsigned char buf[5];
	
	buf[0] = mpxl_cmd_setHSV;
	buf[1] = (unsigned char)(color.hue << 8);
	buf[2] = (unsigned char)color.hue;
	buf[3] = color.saturation;
	buf[4] = color.value;
						
	mpxl_send(buf, 5);
}


void led_state(unsigned char state) {
	unsigned char buf;
	
	if (state == mpxl_state_on)
	{
		buf = mpxl_cmd_on;
	}
	
	if (state == mpxl_state_off)
	{
		buf = mpxl_cmd_off;
	}

	mpxl_send(&buf, 1);
}



void transmit_script(struct UserScriptCmd cmd[20]) {
	unsigned char bufio[10];

	//Scriptengine initialisieren
	bufio[0] = mpxl_cmd_script;
	bufio[1] = scrcmd_init;
	mpxl_send(bufio, 2);

	int i;		
	for (i=0;i<20;i++) {
	
		wait5ms(); //Warten, dass die Befehle verarbeitet werden können
		wait5ms();
		wait5ms();
		
		bufio[0] = mpxl_cmd_script;
		bufio[1] = cmd[i].CMD;
				
		if (bufio[1] == scrcmd_loop)
		{
			mpxl_send(bufio, 2);
			break;
		}

		if (bufio[1] == scrcmd_stop)
		{
			mpxl_send(bufio, 2);
			break;
		}

				
		switch (bufio[1]) {
			case scrcmd_on:
				mpxl_send(bufio, 2);
				break;

			case scrcmd_off:
				mpxl_send(bufio, 2);
				break;
	
			case scrcmd_set:
				bufio[2] = cmd[i].color.values.Red;
				bufio[3] = cmd[i].color.values.Green;
				bufio[4] = cmd[i].color.values.Blue;
				mpxl_send(bufio, 5);
				break;
	
			case scrcmd_wait:
				bufio[2] = cmd[i].time;
				mpxl_send(bufio,3);
				break;
			
			case scrcmd_fade:
				bufio[2] = cmd[i].color.values.Red;
				bufio[3] = cmd[i].color.values.Green;
				bufio[4] = cmd[i].color.values.Blue;
				bufio[5] = cmd[i].time;
				mpxl_send(bufio, 6);
				break;
		}
	}
	wait5ms();
	wait5ms();
}

void start_script() {
	unsigned char buf[2];
	buf[0] = mpxl_cmd_script;
	buf[1] = scrcmd_exec;
	mpxl_send(buf, 2);
}

void stop_script() {
	unsigned char buf[2];
	buf[0] = mpxl_cmd_script;
	buf[1] = scrcmd_stop;
	mpxl_send(buf, 2);
}