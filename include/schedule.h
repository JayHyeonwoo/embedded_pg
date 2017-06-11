#ifndef _SCHEDULE_H
#define _SCHEDULE_H

typedef struct schedule {
	char date[7];
	char name_sub[20];
	char time[5];
} schedule;

/*-------------------prototype---------------------*/

int menu_sel();
void myflush();
void menu_sched();
int register_sched();
void check_sched();
void re_align(schedule *a, int n);
void check_sched_date();
void change_sched();
void delete_sched();
char getch();
int txt_read(schedule *a);
int txt_write(schedule *a, int n);
void cur_subject(schedule *a, int n ,char *buf);
void output_schedtime(schedule *a, int n, char (*buf)[11]);

void sigusr1_handler(int signo);

/*-------------------------------------------------*/

#endif
