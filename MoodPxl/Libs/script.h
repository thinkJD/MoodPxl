#include <stdlib.h>
#include <avr/io.h>

#define scrcmd_init		0x00
#define scrcmd_exec		0x01

#define scrcmd_on		0x02
#define scrcmd_off		0x03
#define scrcmd_set		0x04
#define scrcmd_wait		0x05
#define scrcmd_fade		0x06
#define scrcmd_loop		0x07
#define scrcmd_stop		0x08


struct script_command
{
	uint8_t cmd;
	uint8_t param1;
	uint8_t param2;
	uint8_t param3;
	uint8_t param4;
	uint8_t param5;
};



#define scrsta_start	0
#define scrsta_running	1
#define scrsta_stop		2
#define scrsta_next		3
#define scrsta_init		4



extern void script_init();
extern void script_tick();
extern void script_handler(uint8_t *Comand);