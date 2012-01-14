#include <string.h>
#include "gui/controls.h"
#include "mpxlRF.h"
#include "lcd.h"
#include "fonty.h"
#include "keyboard.h"
#include "../flash/bfs.h"
#include "../cc1100/cc1100.h"
#include "../audio/sid.h"
#include "moodpxlSettings.h"



struct Colorset colors;
char mpxlcol_tb_buffer[20];
int pointer = 0;

const struct Colorset default_Colorset =
{
	{
		{"Rot"},	{"Gruen"},
		{"Blau"},	{"Gelb"},
		{"Lila"},	{"Pink"},
		{""},		{""},
		{""},		{""},
		{""},		{""},
		{""},		{""},
		{""},		{""},
		{""},		{""},
		{""},		{""},
	},
	{
		{170,0,0},	{0,170,0},
		{0,0,170},	{170,150,0},
		{170,0,150},{170,0,100},
		{127,127,127},	{127,127,127},
		{127,127,127},	{127,127,127},
		{127,127,127},	{127,127,127},
		{127,127,127},	{127,127,127},
		{127,127,127},	{127,127,127},
		{127,127,127},	{127,127,127},
		{127,127,127},	{127,127,127}		
	}
	
};

void mpxlcol_load (FORM* form) 
{	
	//Wenn noch kein Colorset gespeichert wurde, wird das Default Colorset
	//Abgespeichert. Sonst wird das COlorset aus dem Speicher geladen.
 	if((BFS_LoadFile(BFS_ID_RGB_Colors, sizeof(struct Colorset), (unsigned char*) &colors))	
 		!= sizeof(struct Colorset)) 
 	{
 		colors = default_Colorset;
 		BFS_SaveFile(BFS_ID_RGB_Colors, sizeof(struct Colorset), (unsigned char*) &colors);
 		msgbox(50,BC_OKOnly | BC_DefaultButton1,"Defaults geladen");
 	}

	pointer = 0;
	//Controls mit den Werten im Speicher initialisieren.
	memcpy(((TXTBOX*)form->controls[3])->text,colors.Name[pointer],20);
	((TRACKBAR*)form->controls[4])->actval = colors.color[pointer].Red;
	((TRACKBAR*)form->controls[5])->actval = colors.color[pointer].Green;
	((TRACKBAR*)form->controls[6])->actval = colors.color[pointer].Blue;
	//und zeichnen
	control_draw(form->controls[3], 0);
	control_draw(form->controls[4], 0);
	control_draw(form->controls[5], 0);
	control_draw(form->controls[6], 0);	

	//setzen der ersten Farbe im Speicher
	cc1100_init();
	setRGB(colors.color[0]);
	//Einschalten des LED-Stacks
	led_state(mpxl_state_on);
}
void mpxlcol_bt_save_click(FORM* form, CONTROL* control)
{
	if (!(BFS_SaveFile(BFS_ID_RGB_Colors, sizeof(struct Colorset), (unsigned char*)&colors)))
	{
		msgbox(50,BC_OKOnly | BC_DefaultButton1,"Speichern fehlgeschlagen");
	}
	
}
void mpxlcol_tb_R_change(FORM* form, CONTROL* control)
{
	colors.color[pointer].Red = ((TRACKBAR*)control)->actval;
	setRGB(colors.color[pointer]);
}
void mpxlcol_tb_G_change(FORM* form, CONTROL* control)
{
	colors.color[pointer].Green = ((TRACKBAR*)control)->actval;
	setRGB(colors.color[pointer]);
}
void mpxlcol_tb_B_change(FORM* form, CONTROL* control)
{
	colors.color[pointer].Blue = ((TRACKBAR*)control)->actval;
	setRGB(colors.color[pointer]);
}
void mpxlcol_txtb_change(FORM* form, CONTROL* control)
{
	memcpy(colors.Name[pointer],((TXTBOX*)control)->text,20);
}
void mpxlcol_nb_change(FORM *form, CONTROL *control)
{
	//Speicherstelle lesen und Controls entsprechend setzen
	pointer = ((NUMBOX*)control)->value;

	memcpy(((TXTBOX*)form->controls[3])->text,colors.Name[pointer],20);
	((TRACKBAR*)form->controls[4])->actval = colors.color[pointer].Red;
	((TRACKBAR*)form->controls[5])->actval = colors.color[pointer].Green;
	((TRACKBAR*)form->controls[6])->actval = colors.color[pointer].Blue;
	
	control_draw(form->controls[3], 0);
	control_draw(form->controls[4], 0);
	control_draw(form->controls[5], 0);
	control_draw(form->controls[6], 0);
	
	setRGB(colors.color[pointer]);
}

void mpxlopt_color_exec(void) 
{
	const LABEL mpxlcol_lbl2	= 
		{BC_Labelnotab,0,20,40,9,BOLDFONT,0,"Pos:",0};
	
	const NUMBOX   mpxlcol_numbox_pos = 
		{BC_Numbox,45,20,20,9,SMALLFONT,0,0,20,NULL,mpxlcol_nb_change};	
		
	const LABEL  mpxlcol_lbl1	= 
		{BC_Labelnotab,0,35,40,9,BOLDFONT,0,"Name:",0};
	
	const TXTBOX   mpxlcol_txtbox_Name = 
		{BC_Txtbox,45,35,75,10,SMALLFONT,20,0,mpxlcol_tb_buffer,NULL,mpxlcol_txtb_change};
	
	const TRACKBAR mpxlcol_trackbar_R = 
		{BC_Trackbar,0,50,120,16,0,255,SMALLFONT,0,"R",mpxlcol_tb_R_change};
	
	const TRACKBAR mpxlcol_trackbar_G = 
		{BC_Trackbar,0,70,120,16,0,255,SMALLFONT,0,"G",mpxlcol_tb_G_change};
	
	const TRACKBAR mpxlcol_trackbar_B = 
		{BC_Trackbar,0,90,120,16,0,255,SMALLFONT,0,"B",mpxlcol_tb_B_change};
	
	const BUTTON   mpxlcol_button_Save = 
		{BC_Button,0,120,128,20,SMALLFONT,0,"Speichern",mpxlcol_bt_save_click};
	
	
	//Controls erstellen
	CONTROL* controls[8] = {
								(CONTROL*)&mpxlcol_lbl1,
								(CONTROL*)&mpxlcol_lbl2,
								(CONTROL*)&mpxlcol_numbox_pos,
								(CONTROL*)&mpxlcol_txtbox_Name,
								(CONTROL*)&mpxlcol_trackbar_R,
								(CONTROL*)&mpxlcol_trackbar_G,
								(CONTROL*)&mpxlcol_trackbar_B,
								(CONTROL*)&mpxlcol_button_Save,
							};
	
	FORM form  = {"Farbspeicher","Moodpixel Optionen",mpxlcol_load,NULL,0,0,controls,0,8,0,0};
	form_exec(&form);
}

