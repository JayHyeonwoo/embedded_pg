#ifndef _BRIGHT_CONTROL_H
#define _BRIGHT_CONTROL_H

#define FIFO_FILE "/tmp/fifo_light"
#define FIFO_REQUIRED_TIME "/tmp/fifo_required_time"

#define BRIGHT_INFO_FILE "light_bright_info.txt"
#define LIGHT_PROC_NAME "esp_lightWifiBulbLanCtrl.py"

#define BUF_SIZE 1024

int select_bright_menu();
int set_each_light_bright();
int select_subject_to_change_bright();
int change_subject_bright(int subject_no);
int set_current_light_bright();
int get_light_proc_pid();
int get_total_min();
int get_process_id(char *proc_name);

struct Subject
{
	char name[20];
	int bright;
};

#endif
