#define task_todo_fade 0
#define task_todo_jump 1
#define task_todo_off 2
#define task_todo_on 3
#define task_todo_restart 4

#define task_todo_start 244 //Daten = Taskl�nge
#define task_todo_end 255	//Ende des Tasks

#define task_state_run 0
#define task_state_stopp 1

struct s_task_do
{
	uint8_t todo;	//Was soll als n�chstes gemacht werden
	uint8_t[] data;	//Die ben�tigten Daten f�r den Task
	uint8_t status;	//Aktueller Status
} todo;

uint8_t task_count = 0;
todo[10] task_list;

/*	Initialisiert den Task mit der Nummer X
	1 = Fade durch das Spektrum
	2 = Pulsieren
	3 = Was wei� ich
*/
	
void task_init(uint8_t Number)
{
	
}
void task_reset();

void task_tick(void)
{
	if (task_list[task_count].status == task_state_)
}