#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h> //Stellt Macros bereit um auf den Prog Speicher zuzugreifen
#include "led_pwm.h"

//Variablen
uint16_t pwm_setting[4];	// Einstellungen für die einzelnen PWM-Kanäle
struct rgb Color_akt;
struct fader_ctrl
{
	volatile uint8_t  state;		//Status des Faders
	uint8_t  diff[3];	//Differenz zum Zielwert
	uint8_t  step[3];	//Steps zum Zelpunkt
	volatile uint16_t  count;		//Counter Steps Zielfarbe
	volatile uint16_t  max_count;	//Steps Zielfarbe
	volatile struct   rgb from;	//Anfangsfarbe
	volatile struct 	 rgb to;		//Zielfarbe
	uint16_t time;		//100ms pro Digit
} f_ctrl;

// Timer 0 Output COMPARE Interrupt
ISR(TIMER0_COMP_vect) {
    static uint16_t pwm_cnt=(uint16_t)0;
    uint8_t tmpPort=0x00;
	
	//TImer zählt bis überlauf und startet dann wieder bei 0
	//hier werden die Takt bis zum nächsten INt einfach hinzuadiert. 
	//Der Timer läuft danach wieder über.
	//So stimmt der Takt trotz der Zeit, die die Int Routine verbraucht.
    //OCR0 += (uint8_t)T_PWM;
    //OCR0 += 174;  //CPU-Takt / (Frequenz * Auflösung) 90Hz
    OCR0 += 156;
    
//     if (pwm_setting[0] > pwm_cnt) tmpPort &= ~(1<<0);
//     if (pwm_setting[1] > pwm_cnt) tmpPort &= ~(1<<1);
//     if (pwm_setting[2] > pwm_cnt) tmpPort &= ~(1<<2);
//     if (pwm_setting[3] > pwm_cnt) tmpPort &= ~(1<<3);
//     
//     tmpPort ^= (1<<3);
    
    if (pwm_setting[0] <= pwm_cnt) tmpPort |= (1<<0);
    if (pwm_setting[1] <= pwm_cnt) tmpPort |= (1<<1);
    if (pwm_setting[2] <= pwm_cnt) tmpPort |= (1<<2);
    if (pwm_setting[3] <= pwm_cnt) tmpPort |= (1<<3);
    
    //tmpPort = (1<<3);
    
    //Später sollte das anders gelöst sein.
    //Nur die gesetzten Bist sollen auf den Port uebertragen werden
    //so kann der Port auch für andere Aufgaben genutzt werden!
    
    PWM_PORT = tmpPort;                         // PWMs aktualisieren  
    
    if (pwm_cnt==1023) //1023 schritte Auflößung
        pwm_cnt=0;
    else
        pwm_cnt++;
}

//Initialisiert die Lib 
void led_init(void) 
{
	LED_On_DDR |= (1<<LED_On_Pin);	//LED_On_Pin als Ausgang
    PWM_DDR = 0xFF;			// Port als Ausgang
    TIMSK |= (1<<OCIE0);   // Interrupt freischalten
    ETIMSK |= (1<<OCIE3A);
    f_ctrl.state = fader_stop;
    DDRD |= (1<<DDB4);
		
}

//Schaltet LED und PWM aus
void led_on(void)
{
	LED_On_Port |= (1<<LED_On_Pin); //Mosfet einschalten
	TCCR0 |= (1 << CS00);	// Timer mit vollem Sys-takt starten	
}

//Schaltet LED und PWM an
void led_off(void)
{
	TCCR0 &= ~(1 << CS00);	// Timer deaktivieren
	PWM_PORT = 0x00;		//Port abschalten
	LED_On_Port &= ~(1<<LED_On_Pin); //Mos-Fet abschalten
}

/*
Setzt eine Farbe. Die Farbe wird als Zeiger auf ein Struct
übergeben. So wird das kopieren vermieden. 
*/
void set_led_color(struct rgb *Color)
{
	//color_akt "." rgb weil es direkt adressiert wird.
	//Color "->" rgb weil es als Pointer übergeben wurde
	uint8_t i;
	for(i=0;i<3;i++)
	{
		Color_akt.rgb[i] = Color->rgb[i];	//Farbe unkorrigiert zwischenspeichern
	}
	
//	cli();		//Opperation muss atomar erfolgen, also Int abschalten
	for (i=0;i<3;i++)
	{
		pwm_setting[i] = human_correction(Color->rgb[i]);
	}
//	sei();
}

void hsv2rgb(struct hsv *Color_hsv, struct rgb *Color_rgb)
{
	//Wenn Sättigung 0 ist die Farbe immer schwarz also
	//wird nur rgb 0x00 0x00 0x00 zurück gegeben.
    if ((Color_hsv->saturation) == 0) 
    {
    	Color_rgb->Red	 = 0x00;
    	Color_rgb->Green = 0x00;
    	Color_rgb->Blue	 = 0x00;
        return;
    }

	//Hue darf nicht größer 360 sein.
	//if (Color_hsv->hue > 360) Color_hsv->hue = 360;
    uint16_t h = Color_hsv->hue % 360;
    //Werte den Arbeitsvariablen zuweisen
    //uint16_t h = Color_hsv->hue; 
    uint8_t s = Color_hsv->saturation;
    uint8_t v = Color_hsv->value;

	//Dieser Code stammt nicht von mir.
	//Siehe -> Wikipedia und PDF
    uint16_t f = ((h % 60) * 255 + 30)/60;
    uint16_t p = (v * (255-s)+128)/255;
    uint16_t q = ((v * (255 - (s*f+128)/255))+128)/255;
    uint16_t t = (v * (255 - ((s * (255 - f))/255)))/255;

    uint8_t i = h/60;

    switch (i) 
    {
        case 0:
        	Color_rgb->Red	 = v;
    		Color_rgb->Green = t;
    		Color_rgb->Blue	 = p;
            break;

        case 1:
        	Color_rgb->Red	 = q;
    		Color_rgb->Green = v;
    		Color_rgb->Blue	 = p;
            break;

        case 2:
        	Color_rgb->Red	 = p;
    		Color_rgb->Green = v;
    		Color_rgb->Blue	 = t;
            break;

        case 3:
        	Color_rgb->Red	 = p;
    		Color_rgb->Green = q;
    		Color_rgb->Blue	 = v;
            break;

        case 4:
        	Color_rgb->Red	 = t;
    		Color_rgb->Green = p;
    		Color_rgb->Blue	 = v;
            break;

        case 5:
        	Color_rgb->Red	 = v;
    		Color_rgb->Green = p;
    		Color_rgb->Blue	 = q;
            break;
    }
}  

void set_fanspeed(uint16_t speed)
{
	pwm_setting[3] = speed;
}

/*
Das Schlüsselwort PROGMEM, legt die Konstanten im 
Programmspeicher ab. SOnst würden sie in den SRAM gelegt,
was zu viel Speicher benötigt */
uint16_t pwmtable[256] PROGMEM =
{
0,
0, 
0, 
0, 
0, 
0, 
0, 
0, 
1, 
1, 
1, 
1, 
1, 
1, 
2, 
2, 
2, 
3, 
3, 
3, 
4, 
4, 
5, 
5, 
6, 
6, 
7, 
7, 
8, 
9, 
9, 
10, 
11, 
11, 
12, 
13, 
14, 
15, 
16, 
16, 
17, 
18, 
19, 
20, 
21, 
23, 
24, 
25, 
26, 
27, 
28, 
30, 
31, 
32, 
34, 
35, 
36, 
38, 
39, 
41, 
42, 
44, 
46, 
47, 
49, 
51, 
52, 
54, 
56, 
58, 
60, 
61, 
63, 
65, 
67, 
69, 
71, 
73, 
76, 
78, 
80, 
82, 
84, 
87, 
89, 
91, 
94, 
96, 
99, 
101, 
104, 
106, 
109, 
111, 
114, 
117, 
119, 
122, 
125, 
128, 
131, 
133, 
136, 
139, 
142, 
145, 
148, 
152, 
155, 
158, 
161, 
164, 
168, 
171, 
174, 
178, 
181, 
184, 
188, 
191, 
195, 
199, 
202, 
206, 
210, 
213, 
217, 
221, 
225, 
229, 
233, 
237, 
241, 
245, 
249, 
253, 
257, 
261, 
265, 
269, 
274, 
278, 
282, 
287, 
291, 
296, 
300, 
305, 
309, 
314, 
319, 
323, 
328, 
333, 
338, 
342, 
347, 
352, 
357, 
362, 
367, 
372, 
377, 
383, 
388, 
393, 
398, 
404, 
409, 
414, 
420, 
425, 
431, 
436, 
442, 
447, 
453, 
459, 
464, 
470, 
476, 
482, 
488, 
494, 
499, 
505, 
511, 
518, 
524, 
530, 
536, 
542, 
548, 
555, 
561, 
568, 
574, 
580, 
587, 
593, 
600, 
607, 
613, 
620, 
627, 
634, 
640, 
647, 
654, 
661, 
668, 
675, 
682, 
689, 
696, 
704, 
711, 
718, 
725, 
733, 
740, 
747, 
755, 
762, 
770, 
778, 
785, 
793, 
801, 
808, 
816, 
824, 
832, 
840, 
848, 
856, 
864, 
872, 
880, 
888, 
896, 
904, 
913, 
921, 
929, 
938, 
946, 
955, 
963, 
972, 
980, 
989, 
998, 
1006, 
1015, 
1024
};
                                    
//gibt den für das menschliche Auge korigierten Wert zurück.
uint16_t human_correction (uint8_t value)
{
	//Gibt den entsprechenden Farbwert aus der Tabelle zurück.
	return pgm_read_word(pwmtable + value);
}


//Berechnet die Schritte pro Tick. So erreichen die Farben
//gleichmäßig den Endwert.
uint8_t fade_calc_diff(uint8_t val1, uint8_t val2)
{
	if (val1 > val2) return val1 - val2;
	if (val1 < val2) return val2 - val1;
	return 0;
}

//Gibt den größten Wert zurück
uint8_t fade_calc_bigg(uint8_t val1, uint8_t val2, uint8_t val3)
{
	uint8_t tmp = val1;
	if (val2 > tmp) tmp = val2;
	if (val3 > tmp) tmp = val3;
	return tmp;
}

//Initialisiert den Fader
void rgb_fade_int(struct rgb Target, uint16_t time)
{
	f_ctrl.time = time;			//Target festlegen
	f_ctrl.from = Color_akt;
	f_ctrl.to = Target;
	f_ctrl.count = 0;			//Counter zurücksetzen
	TCNT3 = (uint16_t)0;		//Zälregister 16bit Timer 3 zurücksetzen
	OCR3A = (uint16_t)249;		//Nächster interupt in 1ms
	f_ctrl.state = fader_run;	//Statemachine zurücksetzen
}
 

void rgb_fade_tick(void)
{
	//Wenn der Fader gestartet wurde  	

	if (f_ctrl.state == fader_run)
	{	
		//Differenz zu jedem Farbkanal ausrechnen
		uint8_t i;
 		for(i=0; i<3; i++)
 		{
 			f_ctrl.diff[i] = fade_calc_diff(f_ctrl.from.rgb[i], f_ctrl.to.rgb[i]);
 		}
 		
 		f_ctrl.max_count = f_ctrl.time;	//Timer initialisieren (Anzahl der Ticks)
 		//Schritte pro Farbkanal, pro Tick berechnen
 		for(i=0; i<3;i++)
 		{
 			f_ctrl.step[i] = f_ctrl.max_count / f_ctrl.diff[i];
 		}
 		
		//Faulheit verhindern 		
 		for (i=0;i<3;i++)
 		{
 			if ((f_ctrl.from.rgb[i] != f_ctrl.to.rgb[i]) && (f_ctrl.step[i] == 0))
 				f_ctrl.step[i] = 1;
 		}
 		
 		//Timer mit Prescaler 64 starten CTC-Mode
 		TCCR3B |= (1<<CS31) | (1<<CS30) | (1<<WGM32);
 		
 		//Fader aktivieren
 		f_ctrl.state = fader_running;

 	}
 	
 	if (f_ctrl.state == fader_running)
 	{
 		//Wenn der Fader laeuft, werden hier die Farben aktualisiert.
 		//Das spahrt Takte in der ISR
 		set_led_color(&f_ctrl.from);
 	}
 	
 	if (f_ctrl.state == fader_last)
 	{
 		f_ctrl.state = fader_idle;
 		set_led_color(&f_ctrl.from);
 	}
 }

uint8_t fade_state()
{
	return f_ctrl.state;
}

ISR(TIMER3_COMPA_vect)
{
	TCNT3 = (uint16_t)0;
	PORTD ^= (1<<PD4);
	if (f_ctrl.count == f_ctrl.max_count)
	{	
		TCCR3B = 0;					//Timer stoppen
		f_ctrl.from = f_ctrl.to;	//Werd auf zielwert setzen 
		f_ctrl.state = fader_last;  //Statemachine auf letzten Lauf setzen
	}
	
	uint8_t i;
	for (i=0;i<3;i++)
	{
 		if (f_ctrl.from.rgb[i] > f_ctrl.to.rgb[i])
 		{
 			if(f_ctrl.count % f_ctrl.step[i] == 0)
 				f_ctrl.from.rgb[i]--;
 		}
 		
 		if (f_ctrl.from.rgb[i] < f_ctrl.to.rgb[i])
 		{
 			if(f_ctrl.count % f_ctrl.step[i] == 0)
 				f_ctrl.from.rgb[i]++;
 		}
	} 

	f_ctrl.count++;				//Counter incrementieren	

}