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


#define F_CPU 16000000
#define F_PWM 90                         // PWM-Frequenz in Hz
#define PWM_STEPS 1024                    // PWM-Schritte pro Zyklus(1..256)
#define PWM_PORT PORTA                   // Port für PWM
#define PWM_DDR DDRA                     // Datenrichtungsregister für PWM

#define LED_On_Port	PORTC
#define LED_On_DDR	DDRC
#define LED_On_Pin	0

#define T_PWM (F_CPU/(F_PWM*PWM_STEPS)) // Systemtakte pro PWM-Takt
 
#if (T_PWM<(93+5))
    #error T_PWM zu klein, F_CPU muss vergrösst werden oder F_PWM oder PWM_STEPS verkleinert werden
#endif


//Bildet eine RGB-Farbe ab.
struct rgb
{
	//Durch das union kann mit den Bezeichnern 
	//und dem Array auf die Daten zugegriffen werden.
	//Es ginge auch mit einem Zeiger, das wäre aber unsauberer Code.
    union {
        struct {
            uint8_t Red;
            uint8_t Green;
            uint8_t Blue;
        };
        uint8_t rgb[3];
    };
};

struct hsv
{
    union {
        struct {
            uint16_t hue;
            uint8_t saturation;
            uint8_t value;
        };
        //In diesem Fall wird ein Byte meht,
        //da hue zwei Byte belegt.
        uint8_t hsv[4];
    };
};

#define led_funktion_move = 0;
#define led_funktion_jump = 1;

extern void led_init(void);
extern void set_led_color(struct rgb *Color);
uint16_t human_correction (uint8_t value);
extern void led_on(void);
extern void led_off(void);
extern void set_fanspeed(uint8_t speed);
extern void hsv2rgb(struct hsv *Color_hsv, struct rgb *Color_rgb);


#define fader_idle 0
#define fader_stop	1
#define fader_run	2
#define fader_running	3 
#define fader_last 4
#define fader_tick 5
extern void rgb_fade_int(struct rgb Target, uint16_t time);
extern void rgb_fade_tick(void);
uint8_t fade_state(void);