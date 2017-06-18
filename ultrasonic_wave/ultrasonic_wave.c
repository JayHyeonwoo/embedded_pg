#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <wiringPi.h>
#include <ctype.h>

#include <comm.h>

#define FIFO_LIGHT "/tmp/fifo_light"
#define FIFO_REQUIRED_TIME "/tmp/fifo_required_time"

#define LIGHT_PROC_NAME "esp_lightWifiBulbLanCtrl.py"
#define PROJECT_EXEC_PATH "./studysys"

#define BUF_SIZE 1024
#define STUDYING_NUM 10

#define trigPin 5
#define echoPin 4

int send_sigusr1_and_fifo(int pid, char *buf);
void sigusr1_handler(int signo);
int get_process_id(char *proc_name);

int child_pid;

struct timeval tv_start, tv_end;

int total_sec;

int main(void)
{
	total_sec = 0;

	signal(SIGUSR1, sigusr1_handler);

	if (access(FIFO_REQUIRED_TIME, F_OK) != 0)
	{
		if (mkfifo(FIFO_REQUIRED_TIME, 0666) < 0)
		{
			fprintf(stderr, "mkfifo error for %s\n", FIFO_REQUIRED_TIME);
			exit(1);
		}
	}

	if (access(FIFO_LIGHT, F_OK) != 0)
	{
		if (mkfifo(FIFO_LIGHT, 0666) < 0)
		{
			fprintf(stderr, "mkfifo error for %s\n", FIFO_LIGHT);
			exit(1);
		}
	}

	int light_pid = get_process_id("esp_lightWifiBu");
	func_syslog(LOG_DEBUG, "light_pid : %d\n", light_pid);

	int distance = 0;
	int pulse = 0;
	if(wiringPiSetup () == -1)
		return 1;

	pinMode (trigPin, OUTPUT);
	pinMode (echoPin, INPUT);

	int studying_arr[STUDYING_NUM] = {0,};
	int cnt = 0;
	int is_studying = 0;

	while(1)
	{
		digitalWrite (trigPin, LOW);
		usleep(2);
		digitalWrite (trigPin, HIGH);
		usleep(20);
		digitalWrite (trigPin, LOW);

		while(digitalRead(echoPin) == LOW);

		long startTime = micros();

		while(digitalRead(echoPin) == HIGH);

		long travelTime = micros() - startTime;

		int distance = travelTime / 58;
		func_syslog(LOG_DEBUG, "Distance: %dcm\n", distance);


		if (distance < 80.0)
		{
			studying_arr[cnt] = 1;
		}

		else 
		{
			studying_arr[cnt] = 0;
		}

		if (cnt >= STUDYING_NUM)
		{
			cnt = 0;
			int i;
			for (i = 0; i < STUDYING_NUM; i++)
			{
				cnt += studying_arr[i];
			}
			if (cnt >= STUDYING_NUM / 2)
			{
				if (is_studying == 0)
				{
					send_sigusr1_and_fifo(light_pid, "ON");


					if ((child_pid = fork()) < 0)
					{
						fprintf(stderr, "fork error\n");
						exit(1);
					}
					else if (child_pid == 0)
					{
						execl("./study_system", "./study_system", NULL);
						exit(0);
					}
					gettimeofday(&tv_start, NULL);
				}
				is_studying = 1;
			}
			else 
			{
				if (is_studying == 1)
				{
					send_sigusr1_and_fifo(light_pid, "OFF");

					kill(child_pid, SIGUSR1);

					gettimeofday(&tv_end, NULL);
					total_sec += (tv_end.tv_sec -tv_start.tv_sec);
					
					func_syslog(LOG_DEBUG, "total_sec : %d\n", total_sec);
				}
				is_studying = 0;
			}
			cnt = 0;
			continue;
		}
		cnt++;
		delay(1000);
	}
}

// sending signal
int send_sigusr1_and_fifo(int pid, char *buf)
{
	if (kill(pid, SIGUSR1) != 0)
	{
		fprintf(stderr, "Can't kill\n");
	}
	sleep(1);

	FILE *fifo_fp;

	if ((fifo_fp = fopen(FIFO_LIGHT, "w+")) == NULL)
	{
		fprintf(stderr, "open error for %s\n", FIFO_LIGHT);
		exit(1);
	}

 	fprintf(fifo_fp, "%s", buf);

	fclose(fifo_fp);
	func_syslog(LOG_DEBUG, "after send sigusr1 & fifo\n", "");

	return 0;
}

void sigusr1_handler(int signo)
{
	int fd;
	char buf[20];
	if ((fd = open(FIFO_REQUIRED_TIME, O_WRONLY)) < 0)
	{
		fprintf(stderr, "open error for %s\n", FIFO_REQUIRED_TIME);
		exit(1);
	}

	gettimeofday(&tv_end, NULL);
	total_sec += (tv_end.tv_sec -tv_start.tv_sec);
	gettimeofday(&tv_start, NULL);

	sprintf(buf, "%d", total_sec / 60);
	write(fd, buf, strlen(buf));
	close(fd);
}

int get_process_id(char *proc_name)
{
	int pid;
	DIR *dirp;
	struct dirent *dir_entry;

	char path[BUF_SIZE];
	strcpy(path, "/proc/");

	if ((dirp = opendir(path)) == NULL)
	{
		fprintf(stderr, "opendir error for %s\n", path);
		return -1;
	}

	int is_pid_dir;
	int i;
	char buf[BUF_SIZE];
	char tmp1[BUF_SIZE/2];
	char tmp2[BUF_SIZE/2];

	// /proc 디릭터리 하위의 pid로 명명된 디렉터리들을 확인한다.
	while((dir_entry = readdir(dirp)) != NULL)
	{
		is_pid_dir = 1;

		// 디렉터리의 이름이 모두 숫자로 이루어져있는지 확인한다.
		for (i = 0; i < (int) strlen(dir_entry -> d_name); i++)
		{
			if (!isdigit(dir_entry -> d_name[i]))
			{
				is_pid_dir = 0;
				break;
			}
		}
		if (!is_pid_dir)	// pid로 명명되지 않으면 다음 디렉터리로 넘어간다.
			continue;

		// /proc/[pid]/status 파일에서 해당 프로세스의 이름을 확인한다.
		pid = atoi(dir_entry -> d_name);
		strcpy(path, "/proc/");
		strcat(path, dir_entry -> d_name);
		strcat(path, "/status");
		
		FILE *fp;
		if ((fp = fopen(path, "r")) == NULL)
		{
			fprintf(stderr, "fopen error for %s\n", path);
			return -1;
		}

		// 프로세스의 이름이 'proc_name' 인지 확인한다.
		while (fgets(buf, BUF_SIZE, fp) != NULL)
		{
			sscanf(buf, "%s\t%s", tmp1, tmp2);
			if (strcmp(tmp1, "Name:") != 0)
				continue;

			if (strcmp(tmp2, proc_name) == 0)
				return pid;
			else 
				break;
		}
	}
	return -1;
}
