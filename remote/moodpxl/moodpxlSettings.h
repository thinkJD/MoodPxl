
//Speichert 20 Farben im Flash
struct Colorset {
	char Name[20][20];
	struct rgb color[20];
};

extern void mpxlopt_color_exec(void);
