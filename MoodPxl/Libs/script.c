#include "script.h"
#include "led_pwm.h"
#include "timer_delay.h"
#include "uart.h"

static struct script
{
	uint8_t state;	//Status
	uint8_t count;	//Anzahl der Einträge
	uint8_t pos;	//Aktuelle Position im Script
	struct script_command command[20];
} _script;

void script_init()
{
	_script.state = scrsta_stop;
	_script.count = 0;
	_script.pos = 0;
}


void script_handler(uint8_t *Command)
{
	//cmd[0] ist immer mpxl_cmd_script.
	//Siehe MoodPixel.c
	uint8_t cmd = Command[1]; 
	
	switch (cmd)
	{
		//Startet die Abarbeitung eines Userscripts
		case scrcmd_exec:
			_script.state = scrsta_start;
			break;
		
		//Stoppt die Abarbeitung eines Userscripts
		case scrcmd_stop:
			_script.state = scrsta_stop;
			_script.pos = 0;
			break;
		
		//Initialisiert den Speicher und 
		case scrcmd_init:
			_script.state = scrsta_init;
			_script.count = 0;
			_script.pos = 0;
			break;
		
		//Fügt einen neuen Befehl an
		default:
			if (_script.state == scrsta_init)
			{
				struct script_command temp;
				temp.cmd = cmd;
				temp.param1 = Command[2];
				temp.param2 = Command[3];
				temp.param3 = Command[4];
				temp.param4 = Command[5];
				temp.param5 = Command[6];
				script_NewEntry(temp);
			}
	}
}

//Behandelt das jeweils nächste Komando 
//im Struct
void Command_handler(struct script_command cmd)
{
	struct rgb color;
	
	switch (cmd.cmd)
	{
		case scrcmd_on:
			led_on();
			_script.state = scrsta_running;
			break;
		
		case scrcmd_off:
			led_off();
			_script.state = scrsta_running;
			break;
		
		case scrcmd_set:
			color.Red = cmd.param1;
			color.Green = cmd.param2;
			color.Blue = cmd.param3;
			set_led_color(&color);
			_script.state = scrsta_running;
			break;
		
		case scrcmd_wait:	
			td_setTimer(0,cmd.param1);
			_script.state = scrsta_running;
			break;		
		
		case scrcmd_fade:	
			color.Red = cmd.param1;
			color.Green = cmd.param2;
			color.Blue = cmd.param3;
			rgb_fade_int(color, (uint16_t)cmd.param4 * 1000); //In sekunden
			_script.state = scrsta_running;
			break;		
		
		case scrcmd_loop:	
			_script.state = scrsta_start;
			break;		
		
		case scrcmd_stop:
			_script.state = scrsta_stop;
			break;
	}
	
}

void script_NewEntry(struct script_command cmd)
{
	_script.command[_script.count] = cmd;
	_script.count++;
	
}

//Wird periodisch aufgerufen. Steuert und überwacht 
//die Script Zustandsmaschine.
void script_tick()
{
	
	if (_script.state == scrsta_start)
	{
		_script.pos = 0;								//Zeiger auf ersten Befehl setzen
		_script.state = scrsta_running;					//Statemachine starten
		Command_handler(_script.command[_script.pos]);	//Ersten Befehl ausführen
	}
	
	//Nächsten Befehl ausführen
	if (_script.state == scrsta_next)
	{
		_script.pos++;					//Zeiger auf nächsten Befehl setzen
		
		_script.state = scrsta_running;	
		Command_handler(_script.command[_script.pos]);
	}
	
	//Prüfen ob Befehl abgearbeitet wurde
	if (_script.state == scrsta_running)
	{
		if (_script.command[_script.pos].cmd == scrcmd_on)
			_script.state = scrsta_next;
		
		if (_script.command[_script.pos].cmd == scrcmd_off)
			_script.state = scrsta_next;
			
		if (_script.command[_script.pos].cmd == scrcmd_set)
			_script.state = scrsta_next;
			
		if (_script.command[_script.pos].cmd == scrcmd_wait)
		{
			if (td_timeout(0)) _script.state = scrsta_next;
		}
		
		if (_script.command[_script.pos].cmd == scrcmd_fade)
		{
			if (fade_state() == fader_idle) _script.state = scrsta_next;
		}
		
	}
	
	
}
