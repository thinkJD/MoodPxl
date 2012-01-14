#define mpxl_cmd_on		0x00
#define mpxl_cmd_off	0x01
#define mpxl_cmd_setRGB	0x02
#define mpxl_cmd_setHSV 0x03
#define mpxl_cmd_script 0x04
#define mpxl_cmd_fade	0x05

#define fan_init	 0x00
#define fan_wait	 0x01
#define fan_dim_down 0x02
#define fan_diming_down 0x03
#define fan_meas 	 0x04
#define fan_dim_up	 0x05
#define fan_set 	 0x06

#define UART_BAUD_RATE 57600
#define F_CPU 16000000

struct fan_control{
	uint8_t state;		//Zustand
	uint8_t speed;		//Lüfterstufe
	uint8_t temp_high;	//Temperatur
	uint8_t temp_low;	//Temoeratur Nachkommastelle
};

//Prototypen
void command(uint8_t *buf);
uint8_t fanspeed_tick();