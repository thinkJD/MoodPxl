#define mpxl_state_off  0x00
#define mpxl_state_on  	0x01

#define scrcmd_init		0x00
#define scrcmd_exec		0x01
#define scrcmd_on		0x02
#define scrcmd_off		0x03
#define scrcmd_set		0x04
#define scrcmd_wait		0x05
#define scrcmd_fade		0x06
#define scrcmd_loop		0x07
#define scrcmd_stop		0x08

#define mpxl_cmd_on		0x00
#define mpxl_cmd_off	0x01
#define mpxl_cmd_setRGB	0x02
#define mpxl_cmd_setHSV 0x03
#define	mpxl_cmd_script 0x04


struct rgb {
    unsigned char Red;
    unsigned char Green;
	unsigned char Blue;
};

struct hsv {
    union {
        struct {
            short hue;
            unsigned char saturation;
            unsigned char value;
        };
        unsigned char hsv[4];
    };
};
struct Color {
	char Name[20];
	struct rgb values;
};
//Representiert ein einzelnes Komando
//innerhalb einer User-Scripts
struct UserScriptCmd {
	unsigned char CMD;
	struct Color color;
	unsigned char time;
};

extern void setRGB(struct rgb);
extern void transmit_script();
extern void led_state(unsigned char state);
extern void start_script();