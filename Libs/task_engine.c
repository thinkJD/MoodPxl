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


#define task_todo_fade 0
#define task_todo_jump 1
#define task_todo_off 2
#define task_todo_on 3
#define task_todo_restart 4

#define task_todo_start 244 //Daten = Tasklänge
#define task_todo_end 255	//Ende des Tasks

#define task_state_run 0
#define task_state_stopp 1

struct s_task_do
{
	uint8_t todo;	//Was soll als nächstes gemacht werden
	uint8_t[] data;	//Die benötigten Daten für den Task
	uint8_t status;	//Aktueller Status
} todo;

uint8_t task_count = 0;
todo[10] task_list;

/*	Initialisiert den Task mit der Nummer X
	1 = Fade durch das Spektrum
	2 = Pulsieren
	3 = Was weiß ich
*/
	
void task_init(uint8_t Number)
{
	
}
void task_reset();

void task_tick(void)
{
	if (task_list[task_count].status == task_state_)
}