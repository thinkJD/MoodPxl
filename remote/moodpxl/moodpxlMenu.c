#include "menu.h"
#include "keyboard.h"
#include "lcd.h"
#include "fonty.h"
#include "controls.h"
#include "global.h"
#include "mpxlRF.h"
#include "moodpxlMenu.h"
#include "Flash/bfs.h"

struct Colorset{
	struct Color color[20];
};

const struct Colorset default_Colorset = {
	{
		{{"Rot"},	{170,0,0}},
		{{"Gruen"},	{0,170,0}},
		{{"Blau"},	{0,0,170}},
		{{"Gelb"},	{170,150,0}},
		{{"Lila"},	{170,0,150}},
		{{"Pink"},	{170,0,100}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
		{{""},		{125,125,125}},
	}
};

struct Colorset colorset;
void load_colorset() {
	//Wenn noch kein Colorset gespeichert wurde, wird das Default Colorset
	//abgespeichert. Sonst wird das Colorset aus dem Speicher geladen.
	
	if((BFS_LoadFile(BFS_ID_RGB_Colors, sizeof(struct Colorset), (unsigned char*) &colorset))	
 		!= sizeof(struct Colorset)) 
 	{
 		colorset = default_Colorset;
 		BFS_SaveFile(BFS_ID_RGB_Colors, sizeof(struct Colorset), (unsigned char*) &colorset);
 		msgbox(50,BC_OKOnly | BC_DefaultButton1,"Defaults geladen");
 	}
}

char frm_tb_buffer[20];
int pointer = 0;

void rfgfrm_load (FORM* form) {	
	load_colorset();

	pointer = 0;
	//Controls mit den Werten im Speicher initialisieren.
	memcpy(((TXTBOX*)form->controls[3])->text,colorset.color[pointer].Name,20);
	((TRACKBAR*)form->controls[4])->actval =  colorset.color[pointer].values.Red;
	((TRACKBAR*)form->controls[5])->actval =  colorset.color[pointer].values.Green;
	((TRACKBAR*)form->controls[6])->actval =  colorset.color[pointer].values.Blue;
	//und zeichnen
	control_draw(form->controls[3], 0);
	control_draw(form->controls[4], 0);
	control_draw(form->controls[5], 0);
	control_draw(form->controls[6], 0);	

	//setzen der ersten Farbe im Speicher
	cc1100_init();
	setRGB(colorset.color[pointer].values);
	//Einschalten des LED-Stacks
	led_state(mpxl_state_on);
}
void rfgfrm_bt_save_click(FORM* form, CONTROL* control) {
	if (!(BFS_SaveFile(BFS_ID_RGB_Colors, sizeof(struct Colorset), (unsigned char*)&colorset)))
	{
		msgbox(50,BC_OKOnly | BC_DefaultButton1,"Speichern fehlgeschlagen");
	}
	
}
void rfgfrm_tb_R_change(FORM* form, CONTROL* control) {
	colorset.color[pointer].values.Red = ((TRACKBAR*)control)->actval;
	setRGB(colorset.color[pointer].values);
}
void rfgfrm_tb_G_change(FORM* form, CONTROL* control) {
	colorset.color[pointer].values.Green = ((TRACKBAR*)control)->actval;
	setRGB(colorset.color[pointer].values);
}
void rfgfrm_tb_B_change(FORM* form, CONTROL* control) {
	colorset.color[pointer].values.Blue = ((TRACKBAR*)control)->actval;
	setRGB(colorset.color[pointer].values);
}
void rfgfrm_txtb_change(FORM* form, CONTROL* control) {
	memcpy(colorset.color[pointer].Name,((TXTBOX*)control)->text,20);
}
void rfgfrm_nb_change(FORM *form, CONTROL *control) {
	//Speicherstelle lesen und Controls entsprechend setzen
	pointer = ((NUMBOX*)control)->value;

	memcpy(((TXTBOX*)form->controls[3])->text, colorset.color[pointer].Name,20);
	((TRACKBAR*)form->controls[4])->actval = colorset.color[pointer].values.Red;
	((TRACKBAR*)form->controls[5])->actval = colorset.color[pointer].values.Green;
	((TRACKBAR*)form->controls[6])->actval = colorset.color[pointer].values.Blue;
	
	control_draw(form->controls[3], 0);
	control_draw(form->controls[4], 0);
	control_draw(form->controls[5], 0);
	control_draw(form->controls[6], 0);
	
	setRGB(colorset.color[pointer].values);
}

void mpxlopt_color_exec(void) {
	const LABEL rfgfrm_lbl2	= 
		{BC_Labelnotab,0,20,40,9,BOLDFONT,0,"Pos:",0};
	
	const NUMBOX   rfgfrm_numbox_pos = 
		{BC_Numbox,45,20,20,9,SMALLFONT,0,0,20,NULL,rfgfrm_nb_change};	
		
	const LABEL  rfgfrm_lbl1	= 
		{BC_Labelnotab,0,35,40,9,BOLDFONT,0,"Name:",0};
	
	const TXTBOX   rfgfrm_txtbox_Name = 
		{BC_Txtbox,45,35,75,10,SMALLFONT,20,0,frm_tb_buffer,NULL,rfgfrm_txtb_change};
	
	const TRACKBAR rfgfrm_trackbar_R = 
		{BC_Trackbar,0,50,120,16,0,255,SMALLFONT,0,"R",rfgfrm_tb_R_change};
	
	const TRACKBAR rfgfrm_trackbar_G = 
		{BC_Trackbar,0,70,120,16,0,255,SMALLFONT,0,"G",rfgfrm_tb_G_change};
	
	const TRACKBAR rfgfrm_trackbar_B = 
		{BC_Trackbar,0,90,120,16,0,255,SMALLFONT,0,"B",rfgfrm_tb_B_change};
	
	const BUTTON   rfgfrm_button_Save = 
		{BC_Button,0,120,128,20,SMALLFONT,0,"Speichern",rfgfrm_bt_save_click};
	
	
	//Controls erstellen
	CONTROL* controls[8] = {
								(CONTROL*)&rfgfrm_lbl1,
								(CONTROL*)&rfgfrm_lbl2,
								(CONTROL*)&rfgfrm_numbox_pos,
								(CONTROL*)&rfgfrm_txtbox_Name,
								(CONTROL*)&rfgfrm_trackbar_R,
								(CONTROL*)&rfgfrm_trackbar_G,
								(CONTROL*)&rfgfrm_trackbar_B,
								(CONTROL*)&rfgfrm_button_Save,
							};
	
	FORM form  = {"Farbspeicher","Moodpixel Optionen",rfgfrm_load,NULL,0,0,controls,0,8,0,0};
	form_exec(&form);
}


//Formular HSV-Einstellungen
struct hsv hsv_color;

void hsvfrm_load (FORM* form) {
		cc1100_init();
		led_state(mpxl_state_on);
		
		hsv_color.hue = 0;
		hsv_color.saturation = 0;
		hsv_color.value = 0;
}
void hsvfrm_timer (FORM* form) {
	
}
void hsvfrm_close (FORM* form) {
}
void hsvfrm_trackbar_H_change(FORM* form, CONTROL* control) {
	((NUMBOX*)form->controls[0])->value = ((TRACKBAR*)control)->actval;
	control_draw(form->controls[0], 0);
	
	hsv_color.hue = ((TRACKBAR*)control)->actval;
	setHSV(hsv_color);
}
void hsvfrm_trackbar_S_change(FORM* form, CONTROL* control) {
	((NUMBOX*)form->controls[1])->value = ((TRACKBAR*)control)->actval;
	control_draw(form->controls[1], 0);
	
	hsv_color.saturation = ((TRACKBAR*)control)->actval;
	setHSV(hsv_color);
}
void hsvfrm_trackbar_V_change(FORM* form, CONTROL* control) {
	((NUMBOX*)form->controls[2])->value = ((TRACKBAR*)control)->actval;
	control_draw(form->controls[2], 0);

	hsv_color.value = ((TRACKBAR*)control)->actval;
	setHSV(hsv_color);
}
void hsvfrm_numchange_H(FORM* form, CONTROL* control) {
	((TRACKBAR*)form->controls[3])->actval = ((NUMBOX*)control)->value;
	control_draw(form->controls[3], 0);
	
	hsv_color.hue = ((NUMBOX*)control)->value;
	setHSV(hsv_color);
}
void hsvfrm_numchange_S(FORM* form, CONTROL* control) {
	((TRACKBAR*)form->controls[4])->actval = ((NUMBOX*)control)->value;
	control_draw(form->controls[4], 0);
	
	hsv_color.saturation = ((NUMBOX*)control)->value;
	setHSV(hsv_color);
}
void hsvfrm_numchange_V(FORM* form, CONTROL* control) {
	((TRACKBAR*)form->controls[5])->actval = ((NUMBOX*)control)->value;
	control_draw(form->controls[5], 0);
	
	hsv_color.value = ((NUMBOX*)control)->value;
	setHSV(hsv_color);
}
void hsvfrm_bt_Ok_click(FORM* form, CONTROL* control) {
	led_state(mpxl_state_off);
}
void hsvfrm_exec(void) {
	//Numboxen initialisieren	
	const NUMBOX hsvfrm_numbox_H = 
		{BC_Numbox,0,20,20,9,SMALLFONT,0,0,365,NULL,hsvfrm_numchange_H};
	const NUMBOX hsvfrm_numbox_S = 
		{BC_Numbox,54,20,20,9,SMALLFONT,0,0,100,NULL,hsvfrm_numchange_S};
	const NUMBOX hsvfrm_numbox_V = 
		{BC_Numbox,100,20,20,9,SMALLFONT,0,0,100,NULL,hsvfrm_numchange_V};
	
	//Trackbars initialisieren
	const TRACKBAR hsvfrm_trackbar_H = 
		{BC_Trackbar,0,50,120,16,0,365,SMALLFONT,0,"H",hsvfrm_trackbar_H_change};
	const TRACKBAR hsvfrm_trackbar_S = 
		{BC_Trackbar,0,70,120,16,0,100,SMALLFONT,0,"S",hsvfrm_trackbar_S_change};
	const TRACKBAR hsvfrm_trackbar_V = 
		{BC_Trackbar,0,90,120,16,0,100,SMALLFONT,0,"V",hsvfrm_trackbar_V_change};
	//Buttons initialisieren
	const BUTTON hsvfrm_button_Ok 	= 
		{BC_Button,0,110,128,20,SMALLFONT,0,"OK",hsvfrm_bt_Ok_click};
	
	//Controls erstellen
	CONTROL* controls[9] = {
								(CONTROL*)&hsvfrm_numbox_H,
								(CONTROL*)&hsvfrm_numbox_S,
								(CONTROL*)&hsvfrm_numbox_V,
								(CONTROL*)&hsvfrm_trackbar_H,
								(CONTROL*)&hsvfrm_trackbar_S,
								(CONTROL*)&hsvfrm_trackbar_V,
								(CONTROL*)&hsvfrm_button_Ok,
							};
	
	FORM form  = {"HSV-Einstellungen","Moodpixel Optionen",hsvfrm_load,hsvfrm_close,
		hsvfrm_timer,0,controls,4,7,0,10};

	form_exec(&form);
}


//20 Scripts mit je 20 Befehlen können
//gespeichert werden
struct UserScripts {
	char Name[20][20];
	struct UserScriptCmd Script[20][20];
};
struct UserScripts default_Scripts = {
	{
		{"Pink Pulse"}, {"Strobo"}, {"Active"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, 
		{""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}
	},
	{
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},	
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		},
		{
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}, 
			{0x08,{{""},{0,0,0}},1}, {0x08,{{""},{0,0,0}},1}
		}
	}
};
struct UserScripts Scripts;
int pointer_script = 0;
int pointer_script_com = 0;

void setup_controls(FORM* form) {
	memcpy(((TXTBOX*)form->controls[4])->text, Scripts.Name[pointer_script],20);
	
	((LISTBOX*)form->controls[7])->listindex = 
		Scripts.Script[pointer_script][pointer_script_com].CMD - 2;

	int i;
	for(i=0;i<20;i++)
	{
		if(strcmp(Scripts.Script[pointer_script][pointer_script_com].color.Name,
			colorset.color[i].Name)==0)
		{
			((LISTBOX*)form->controls[8])->listindex = i;
			break;
		}
	}
		
	((NUMBOX*)form->controls[9])->value = Scripts.Script[pointer_script][pointer_script_com].time;
		
	control_draw(form->controls[4], 0);
	control_draw(form->controls[7], 0);
	control_draw(form->controls[8], 0);
	control_draw(form->controls[9], 0);
}

void scrset_load (FORM* form) {
 	setup_controls(form);
}
void scrset_close (FORM* form) {
	stop_script();
}
void scrset_nb_pos1_change(FORM *form, CONTROL *control) {
	pointer_script = ((NUMBOX*)control)->value;
	setup_controls(form);
}
void scrset_nb_pos2_change(FORM *form, CONTROL *control) {
	pointer_script_com = ((NUMBOX*)control)->value;
	setup_controls(form);
}
void scrset_tb_name_change(FORM *form, CONTROL *control) {
	memcpy(Scripts.Name[pointer_script],
		((TXTBOX*)control)->text,20);
}
void scrset_lb_func_change(FORM *form, CONTROL *control) {
	Scripts.Script[pointer_script][pointer_script_com].CMD =
		((LISTBOX*)control)->listindex + 2;
}
void scrset_lb_color_change(FORM *form, CONTROL *control) {
	Scripts.Script[pointer_script][pointer_script_com].color =
		colorset.color[((LISTBOX*)control)->listindex];
}
void scrset_nb_time_change(FORM *form, CONTROL *control) {
	Scripts.Script[pointer_script][pointer_script_com].time = ((NUMBOX*)control)->value;
}
void scrset_bt_save_click(FORM *form, CONTROL *control) {
	BFS_SaveFile(BFS_ID_Scripts, sizeof(struct UserScripts), (unsigned char*) &Scripts);
}
void scrset_bt_test(FORM *form, CONTROL *control) {
	transmit_script(Scripts.Script[pointer_script]);
 	start_script();
}

char *lbColorItems[20];
void scrset_exec(void) {

	const char* lbFuncItems[7] = {  "LED-Ein",
									"LED-Aus",
									"Setze Farbe",
									"Warten in Sek",
									"Faden in Sek",
									"Wiederholen",
									"Stopp"	};
									
	//Ladend er Farbeinstellungen
	load_colorset();
	
	int i;
	for (i=0;i<20;i++)
	{
		lbColorItems[i] = colorset.color[i].Name;
	}
	
	const LABEL scrset_lbl_3 = 
		{BC_Labelnotab,0,15,40,10,SMALLFONT,0,"Name:",NULL};
	const TXTBOX scrset_txb_Name =
		{BC_Txtbox,45,15,80,10,SMALLFONT,20,0,frm_tb_buffer,NULL,scrset_tb_name_change};
	const LABEL scrset_lbl_1 = 
		{BC_Labelnotab,0,30,40,10,SMALLFONT,0,"Script:",NULL};
	const NUMBOX scrset_nb_Pos1	= 
		{BC_Numbox,45,30,20,9,SMALLFONT,0,0,20,NULL,scrset_nb_pos1_change};
	const LABEL scrset_lbl_2 = 
		{BC_Labelnotab,0,45,40,10,SMALLFONT,0,"Pos:",NULL};
	const NUMBOX scrset_nb_Pos2	= 
		{BC_Numbox,45,45,20,9,SMALLFONT,0,0,20,NULL,scrset_nb_pos2_change};
	const LABEL scrset_lbl_6 = 
		{BC_Labelnotab,0,60,40,10,SMALLFONT,0,"ToDo:",NULL};
	const LISTBOX scrset_lb_Func =
		{BC_Listbox,45,60,83,10,SMALLFONT,0,7,(char**)lbFuncItems,NULL,scrset_lb_func_change};
	const LABEL scrset_lbl_5 = 
		{BC_Labelnotab,0,75,40,10,SMALLFONT,0,"Farbe:",NULL};	
	const LISTBOX scrset_lb_Color =
		{BC_Listbox,45,75,83,10,SMALLFONT,0,20,(char**)lbColorItems,NULL,scrset_lb_color_change};
	const LABEL scrset_lbl_4 = 
		{BC_Labelnotab,0,90,40,10,SMALLFONT,0,"Zeit:",NULL};
	const NUMBOX scrset_nb_time	=
		{BC_Numbox,45,90,80,9,SMALLFONT,1,1,255,NULL,scrset_nb_time_change};
	const BUTTON scrset_bt_save	= 	
		{BC_Button,0,120,60,20,SMALLFONT,0,"Speichern",scrset_bt_save_click};
	const BUTTON scrset_bt_Test	=
		{BC_Button,68,120,60,20,SMALLFONT,0,"Test",scrset_bt_test};

	
	//Controls erstellen
	CONTROL* controls[14] = {
								(CONTROL*)&scrset_lbl_1,
								(CONTROL*)&scrset_lbl_2,
								(CONTROL*)&scrset_lbl_3,
								(CONTROL*)&scrset_lbl_4,
								(CONTROL*)&scrset_txb_Name,
								(CONTROL*)&scrset_nb_Pos1,
								(CONTROL*)&scrset_nb_Pos2,
								(CONTROL*)&scrset_lb_Func,
								(CONTROL*)&scrset_lb_Color,
								(CONTROL*)&scrset_nb_time,
								(CONTROL*)&scrset_bt_save,
								(CONTROL*)&scrset_bt_Test,
								(CONTROL*)&scrset_lbl_5,
								(CONTROL*)&scrset_lbl_6
							};
	
	
	FORM form  = {"User-Scripts","Moodpixel Optionen",scrset_load,scrset_close,NULL,0,
		controls,5,14,0,0};

	if((BFS_LoadFile(BFS_ID_Scripts, sizeof(struct UserScripts), (unsigned char*) &Scripts))	
 		!= sizeof(struct UserScripts)) 
 	{
 		Scripts = default_Scripts;
 		BFS_SaveFile(BFS_ID_Scripts, sizeof(struct UserScripts), (unsigned char*) &Scripts);
 		msgbox(50,BC_OKOnly | BC_DefaultButton1,"Defaults geladen");
 	}

	form_exec(&form);
}

const struct MENU_ENTRY moodpxlMenuEntries[] = {
	{mpxlopt_color_exec,	0,	FUNCENTRY,	"RGB","Einstellungen"},
	{hsvfrm_exec,			0,	FUNCENTRY,	"HSV",	"HSV-Farbraum"},
	{scrset_exec,			0,	FUNCENTRY,	"Script opt","Fade zu X in n Sek"}
};
const struct MENU moodpxlMenu = {
	3, (MENU_ENTRY*)&moodpxlMenuEntries, "MoodPxl Optionen"
};
