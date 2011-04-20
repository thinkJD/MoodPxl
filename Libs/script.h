#define fscr_start 0
#define fscr_run 0
#define fscr_end 0

struct script_command
{
	//cmd_start
	//cmd_wait
	//cmd_stopp
	//cmd_loop
	
	//cmd_fade
		//Farbe; Fadezeit;
	uint8_t cmd;
	uint8_t param[4];
};

struct script
{
	uint8_t state;	//Status
	uint8_t count;	//Anzahl der Einträge
	uint8_t pos;	//Aktuelle Position im Script
	struct fadescript_command command;
};
