#include "script.h"
#include "led_pwm.h"
#include "timer_delay.h"
#include "uart.h"

static struct script
{
	uint8_t state;	//Status
	uint8_t count;	//Anzahl der Einträge
	uint8_t pos;	//Aktuelle Position im Script
	struct script_command command[10];
} _script;

void script_init()
{
	uart1_puts("i");
	_script.state = scrsta_stop;
	_script.count = 0;
	_script.pos = 0;
}


void script_handler(uint8_t *Comand)
{
	//cmd[0] ist immer das Scriptkomando
	uint8_t cmd = Comand[1];
	
	switch (cmd)
	{
		//Startet die Abarbeitung der Befehle
		case scrcmd_exec:
			_script.state = scrsta_start;
			break;
		
		//Stoppt die Abarbeitung der Befehle
		case scrcmd_stop:
			_script.state = scrsta_stop;
			_script.pos = 0;
			break;
		
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
				temp.param1 = Comand[2];
				temp.param2 = Comand[3];
				temp.param3 = Comand[4];
				temp.param4 = Comand[5];
				temp.param5 = Comand[6];
				script_NewEntry(temp);
			}
	}
}

//Behandelt das jeweils nächste Komando 
//im Struct
void comand_handler(struct script_command cmd)
{
	struct rgb color;
	
	switch (cmd.cmd)
	{
		case scrcmd_on:
			led_on();
			break;
		
		case scrcmd_off:
			led_off();
			break;
		
		case scrcmd_set:
			color.Red = cmd.param1;
			color.Green = cmd.param2;
			color.Blue = cmd.param3;
			set_led_color(&color);
			break;
		
		case scrcmd_wait:	
			td_setTimer(0,cmd.param1);
			break;		
		
		case scrcmd_fade:	
			color.Red = cmd.param1;
			color.Green = cmd.param2;
			color.Blue = cmd.param3;
			rgb_fade_int(color, (uint16_t)cmd.param3 * 1000); //In sekunden
			break;		
		
		case scrcmd_loop:	
			_script.state = scrsta_start;
			break;		
		
		case scrcmd_stop:
			_script.state = scrsta_stop;
			break;
	}
	_script.state = scrsta_running;
}

//Fügt 
void script_NewEntry(struct script_command cmd)
{
	_script.command[_script.count] = cmd;
	_script.count++;
	
}


void script_tick()
{
	uart1_putc('T');
	uart1_putc(_script.state);
	uart1_putc(_script.count);
	uart1_putc(_script.pos);
	uart1_putc(_script.command[_script.pos].cmd);
	
	
	
	if (_script.state == scrsta_start)
	{
		_script.pos = 0;								//Zeiger auf ersten Befehl setzend
		_script.state = scrsta_running;					//Statemachine starten
		comand_handler(_script.command[_script.pos]);	//Ersten Befehl ausführen
	}
	
	if (_script.state == scrsta_next)
	{
		_script.pos++;					//Zeiger auf nächsten Befehl setzen
		if (_script.pos > _script.count)
		{
			_script.state = scrsta_stop;
			return;			
		}
		
		_script.state = scrsta_running;	//Zustand starten
		comand_handler(_script.command[_script.pos]); //Befehl ausführen
	}
	
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
