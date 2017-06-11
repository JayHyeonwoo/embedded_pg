#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

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


int select_bright_menu()
{
	int num;
	while(1){
		system("clear");
		printf("-----------------------------------------------------------\n");
		printf("                        1. 과목 조명 설정\n");
		printf("                        2. 현재 조명 설정\n");
		printf("                        3. 나가기\n");
		printf("              4. get_min\n");
		printf("-----------------------------------------------------------\n");
		printf("입력  : ");
		scanf("%d", &num);
		switch(num)
		{
			case 1:
				if (select_subject_to_change_bright() < 0)
					return -1;
				break;

			case 2:
				if (set_current_light_bright() < 0)
					return -1;
				break;
			case 3:
				return 0; 
			case 4:
				get_total_min();
				break;

			default: 
				break;
		}
	}
}

int get_subject_bright(char *subject_name)
{
	FILE *bright_info_fp;
	if((bright_info_fp = fopen(BRIGHT_INFO_FILE, "r")) == NULL)
	{
		fprintf(stderr, "open error for %s\n", BRIGHT_INFO_FILE);
		return -1;
	}

	struct Subject subject;
	int res;
	while ((res = fscanf(bright_info_fp, "%s%d", subject.name, &subject.bright)) == 2)
	{
		if (strcmp(subject_name, subject.name) == 0)
		{
			fclose(bright_info_fp);
			return subject.bright;
		}
	}

	fclose(bright_info_fp);
	return -1;
}


int select_subject_to_change_bright() 
{
	FILE *bright_info_fp;
	if((bright_info_fp = fopen(BRIGHT_INFO_FILE, "r")) == NULL)
	{
		fprintf(stderr, "open error for %s\n", BRIGHT_INFO_FILE);
		return -1;
	}
	struct Subject subject;

	//    char sub_str[10][20];
	//    strcpy(sub_str[0], "Korean");
	//    strcpy(sub_str[1], "Math");
	//    strcpy(sub_str[2], "English");
	//    strcpy(sub_str[3], "Science");
	//    strcpy(sub_str[4], "History");

	int i = 0;
	char buf[BUF_SIZE];
	//while ((res = fscanf(bright_info_fp, "%s%d", &subject.name, subject.bright)) == 2)

	system("clear");
	printf("-----------------------------------------------------------\n");
	while (fgets(buf, BUF_SIZE, bright_info_fp) != NULL)
	{
		if (sscanf(buf, "%s%d", subject.name, &subject.bright) != 2)
		{
			fprintf(stderr, "sscanf error\n");
			exit(1);
		}
		printf("                        %d. %s : %d\n", i+1, subject.name, subject.bright);
		i++;
	}
	fclose(bright_info_fp);

	//    printf("2. Math : %d\n", );
	//    printf("3. English : %d\n", );
	//    printf("4. Science : %d\n", );
	//    printf("5. History : %d\n", );


	int num;
	printf("-----------------------------------------------------------\n");
	printf("설정할 과목의 번호를 입력하세요: ");
	scanf("%d", &num);

	change_subject_bright(num - 1);

	return 0;
}

int change_subject_bright(int subject_no)
{
	char sub_str[10][20];
	strcpy(sub_str[0], "Korean");
	strcpy(sub_str[1], "Math");
	strcpy(sub_str[2], "English");
	strcpy(sub_str[3], "Science");
	strcpy(sub_str[4], "History");



	FILE *bright_info_fp;
	if((bright_info_fp = fopen(BRIGHT_INFO_FILE, "r+")) == NULL)
	{
		fprintf(stderr, "open error for %s\n", BRIGHT_INFO_FILE);
		return -1;
	}
	struct Subject subject;

	int i = 0;
	char buf[BUF_SIZE];
	while (fgets(buf, BUF_SIZE, bright_info_fp) != NULL)
	{
		if (i != subject_no)
		{
			i++;
			continue;
		}

		if (sscanf(buf, "%s%d", subject.name, &subject.bright) != 2)
		{
			fprintf(stderr, "sscanf error\n");
			exit(1);
		}

		int new_bright;
		printf("설정할 조명의 밝기를 입력하세요\n");
		printf("%s : %d -> ", subject.name, subject.bright);
		scanf("%d", &new_bright);

		int buf_len = strlen(buf);
		fseek(bright_info_fp, -buf_len, SEEK_CUR);

		fprintf(bright_info_fp, "%-20s %03d\n", sub_str[i], new_bright);

		//        sprintf(buf, "%s %d\n", sub_str[i], new_bright);
		//
		//        fprintf(bright_info_fp, "%s", buf);
		//        int j;
		//        for(j = 0; j < buf_len - strlen(buf); j++)
		//            fputc('\0', bright_info_fp);


		fclose(bright_info_fp);

		printf("조명 변경 성공 \n");
		myflush();
		printf("아무키나 입력하세요. \n");
		getch();

		return 0;
	}
	fclose(bright_info_fp);
	return -1;
}

int set_current_light_bright()
{
	if(access(FIFO_FILE, F_OK) != 0)
	{
		if(mkfifo(FIFO_FILE, 0666) < 0)
		{
			fprintf(stderr, "mkfifo error for %s\n", FIFO_FILE);
			exit(1);
		}
	}
	int light_pid;
	if ((light_pid = get_light_proc_pid()) < 0)
	{
		fprintf(stderr , "There is NO light_process\n");
		exit(1);
	}

	char buf[20];
//	while (1)
	{
		printf("현재 설정할 조명의 밝기를 입력하세요.\n");
		scanf("%s", buf);

		kill(light_pid, SIGUSR1);
		FILE *fifo_fp;
		if ((fifo_fp = fopen(FIFO_FILE, "w")) == NULL)
		{
			fprintf(stderr, "open error for %s\n", FIFO_FILE);
			exit(1);
		}

//		if (strcmp(buf, "q") == 0 || strcmp(buf, "quit") == 0)
//			break;
		fprintf(fifo_fp, "%s", buf);
		fclose(fifo_fp);
	}
	printf("Success\n");
	myflush();
	getch();

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

//		printf("path : %s\n",path);


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
			if(ch == '\0')                // NULL로 argv를 구분하여 출력한다.
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
				//                printf("argv[%d] : %s\n", argCount++, buf);
				i = 0;    
				argCount++;
			}
		}
		strcpy(path, "/proc/");
	}
	return -1;
}

int get_total_min()
{
	int pid;
	char buf[20];
	printf("hi\n");

	if ((pid = get_process_id("ultrasonic_wave")) < 0)
	{
		fprintf(stderr, "no pid\n");
		return -1;
	}

	kill(pid, SIGUSR1);
	

	int fd;
	if ((fd = open(FIFO_REQUIRED_TIME, O_RDONLY)) < 0)
	{
		fprintf(stderr, "open error for %s\n", FIFO_REQUIRED_TIME);
		exit(1);
	}

	int len;
	if ((len = read(fd, buf, 20)) < 0)
	{
		fprintf(stderr, "read error for %s\n", FIFO_REQUIRED_TIME);
		return -1;
	}
	buf[len] = '\0';

	printf("get_total_min : %s\n", buf);
	return atoi(buf);
}

// 프로세스의 pid를 얻는 함수
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

	printf("hello\n");
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
		
//		printf("path : %s\n", path);

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
