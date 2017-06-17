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
#define PROJECT_EXEC_PATH "./project"

#define BUF_SIZE 1024
#define STUDYING_NUM 10

#define trigPin 5
#define echoPin 4

int send_sigusr1_and_fifo(int pid, char *buf);
int get_light_proc_pid();
void sigusr1_handler(int signo);

int child_pid;

struct timeval tv_start, tv_end;

int total_sec;

int main(void)
{
	printf("hi\n");
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

	int light_pid = get_light_proc_pid();
	printf("light_pid : %d\n", light_pid);

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
	//		printf("Studying\n");
			studying_arr[cnt] = 1;
		}

		else 
		{
	//		printf("NO\n");
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

//					printf("exec\n");

					if ((child_pid = fork()) < 0)
					{
						fprintf(stderr, "fork error\n");
						exit(1);
					}
					else if (child_pid == 0)
					{
						execl("./study_system", "./study_system", NULL);
//						execl(PROJECT_EXEC_PATH, PROJECT_EXEC_PATH, NULL);
//
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
					//printf("send sigusr1 : %d\n", child_pid);

					gettimeofday(&tv_end, NULL);
					total_sec += (tv_end.tv_sec -tv_start.tv_sec);
					printf("total_sec : %d\n", total_sec);
				}
				is_studying = 0;
			}
			cnt = 0;
			//printf("is_studying : %d\n", is_studying);
			continue;
		}
		cnt++;
		delay(1000);
	}
}

// sending signal
int send_sigusr1_and_fifo(int pid, char *buf)
{
	FILE *fifo_fp;
	if ((fifo_fp = fopen(FIFO_LIGHT, "w")) == NULL)
	{
		fprintf(stderr, "open error for %s\n", FIFO_LIGHT);
		exit(1);
	}
	fprintf(fifo_fp, "%s", buf);
	fclose(fifo_fp);

//	sleep(1);
//	while (kill(pid, SIGUSR1) != 0)
//	{
//		printf("no kill signal\n");
//		sleep(1);
//	}
	kill(pid, SIGUSR1);


	return 0;
}

// Light 프로세스의 pid를 얻는 함수
int get_light_proc_pid(void)
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
		if (!is_pid_dir)
			continue;

		// /proc/[pid]/cmdline 파일에서 해당 프로세스의 이름을 확인한다.
		pid = atoi(dir_entry -> d_name);

		strcat(path, dir_entry -> d_name);
		strcat(path, "/cmdline");
		
		//printf("path : %s\n",path);


		int fd;
		if ((fd = open(path, O_RDONLY)) < 0)
		{
			fprintf(stderr, "open error for %s\n", path);
			return -1;
		}

		int argCount = 0;
		int ch;
		while(read(fd, &ch, 1) > 0)
		{
			buf[i++] = ch;
			if(ch == '\0')				// NULL로 argv를 구분하여 출력한다.
			{
				if (argCount == 1)
				{
					if (strcmp(buf, LIGHT_PROC_NAME) == 0)
					{
						//printf("\npid : %d\n", pid);
						return pid;
					}
					else
						break;

				}
//				printf("argv[%d] : %s\n", argCount++, buf);
				i = 0;	
				argCount++;
			}
		}
		strcpy(path, "/proc/");
	}
	return -1;
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
