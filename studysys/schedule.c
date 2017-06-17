#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <termio.h>
#include <time.h>

/* user defined header files */
#include <pthread.h>
#include <comm.h>
#include <devinfo.h>
#include <schedule.h>

struct termios buffer, save;

int main(void)
{
    if (comm_init() < 0) {
	perror("comm_init error");
	exit(-1);
    }

    int sel, i, status;
    system("clear");
    printf("Hello Sydney\n");
    sleep(1);

    while (1)
    {
	sel = menu_sel();
	switch (sel)
	{
	    case 1: // 일정관리

		menu_sched();

		break;

	    case 2: // 조명설정
		select_bright_menu();
		break;


	    case 3: // 공부현황
		// when information list, check mutex
		system("clear");
		print_other_infos();
		break;
	    default:
		break;
	}

    }


}

void print_other_infos(void)
{
    struct information info;
    time_t cur_time;
    int i, ch, n;

    n = 1;
    cur_time = time(NULL);
    printf("n	ip		subject		time\n");
    printf("--------------------------------------------\n");
    for (i = 0; i < DEVINFO_MAX; ++i) {
	info.index = i;
	get_other_info(&info);

	if (info.rx_time + 10 < cur_time) {
	    continue;
	}
	printf("%d	%s	%s		%lumin\n",
		n,
		index2ip(info.index), 
		info.subject_name,
		info.time);
    }

    myflush();
    ch = getch();
}

/*
   함수명 : int sel()
   함수 기능 : 메뉴에서 시스템이 어떤 기능을 할지 선택하는 함수.
   return 값: 메뉴에 대한 선택번호. (1,2,3,4 중에 하나 선택)
   */

int menu_sel()
{
    system("clear"); // 리눅스의 경우 clear 로 변경.
    printf("-----------------------------------------------------------\n");
    printf("                        1. 일정관리\n");
    printf("                        2. 조명설정\n");
    printf("                        3. 공부현황\n");
    printf("-----------------------------------------------------------\n");

    int sel, res;
    while (1) {
	printf("입력 : ");
	res = scanf("%d", &sel);
	if (res != 1)
	{
	    printf("잘못입력하셨습니다.");
	    myflush();

	}
	else if (sel < 1 || sel > 3)
	{
	    printf("해당 문자의 범위안의 숫자를 입력하세요 \n ");
	}
	else
	    break;
    }
    return sel;
}

/*
   함수명 : int sel_seched()
   기능 : 일정관리 내에 등록과 변경을 고르게 하는 메뉴.

*/
void menu_sched()
{
    while (1) {
	system("clear"); // 리눅스의 경우 clear 로 변경.
	printf("-----------------------------------------------------------\n");
	printf("                        1. 일정등록\n");
	printf("                        2. 일정변경\n");
	printf("                        3. 일정삭제\n");
	printf("                        4. 전체일정확인\n");
	printf("                        5. 일정확인\n");
	printf("                        6. 나가기\n");
	printf("-----------------------------------------------------------\n");

	int sel, res;
	while (1) {
	    printf("입력 : ");
	    res = scanf("%d", &sel);
	    if (res != 1)
	    {
		printf("잘못입력하셨습니다.");
		myflush();

	    }
	    else if ((sel < 1 || sel > 6) && res == 1)
	    {
		printf("해당 문자의 범위안의 숫자를 입력하세요 \n ");
	    }
	    else
		break;
	}
	int result;
	switch (sel)
	{
	    case 1:
		result = register_sched();
		if (result == 0) {
		    printf("등록실패!!!\n");
		    //sleep(1000);
		}
		break;
	    case 2:
		//일정변경
		change_sched();

		break;
	    case 3:
		//일정삭제
		delete_sched();

		break;
	    case 4:
		//전체일정확인
		check_sched();
		break;
	    case 5: //날짜별 일정확인
		check_sched_date();

		break;

	    case 6:
		return;
	    default:
		break;
	}

    }


}


/*
   함수명 : void myflush()
   기능 :    버퍼비우기.
   리턴값 : 없음*/
void myflush()
{
    while (getchar() != '\n');
}
/*
   함수명 : int register_seched()
   기능 : 일정 등록
   날짜 6자리(char)|과목수(int)|과목이름(char)|시간(char)| ... 이런식으로 schedule_info.txt 에 저장.
   리턴값 : 성공시 1 실패시 0
   */

int register_sched()
{
    char date[7];//날짜 6자리
    int num_sub; // 과목갯수
    char name_sub[20]; // 과목이름
    char time[5];//과목 시간

    int res; // 임시 결과값.


    while (1) {
	system("clear"); // 리눅스의 경우 clear 로 변경.
	printf("날짜를 다음 예 처럼 입력하세요  ex. 2017년 4월 26일 -> 170426 \n 입력 : ");
	res = scanf("%s", date);
	if (res == 1 && strlen(date) == 6) break;
	myflush();
    }

    while (1) {
	system("clear"); // 리눅스의 경우 clear 로 변경.
	printf("과목의 갯수를 입력하세요 : ");
	res = scanf("%d", &num_sub);
	if (res == 1) break;
    }

    FILE *fp;

    fp = fopen("schedule_info.txt", "a");


    assert(fp != NULL);
    if (fp == NULL)
	return 0;

    int i;
    for (i = 0; i < num_sub; i++)
    {
	fprintf(fp, "%6s ", date);
	system("clear"); // 리눅스의 경우 clear 로 변경.
	printf("%d . 과목이름 : ", i + 1);
	res = scanf("%s", name_sub);
	myflush();
	while (res != 1 || strlen(name_sub) > 19) {

	    printf("잘못 입력하셨습니다. 다시 입력해주세요. \n");
	    myflush();
	    res = scanf("%s", name_sub);
	}
	while (1) {
	    printf("과목시간을 다음 예 처럼 입력하세요  ex. 7시15분 -> 0715 \n 입력 : ");
	    res = scanf("%s", time);
	    if (res == 1 && strlen(time) == 4) break;
	    myflush();
	}
	fprintf(fp, "%s %4s", name_sub, time);
	fprintf(fp, "\n");
    }
    fclose(fp);

    schedule tmp_sched[10000];
    schedule tmp;
    int count = 0;

    if ((fp = fopen("schedule_info.txt", "r+")) == NULL)
	printf("파일오류\n");

    i = 0;
    while (fscanf(fp, "%6s%s%4s", tmp_sched[i].date, tmp_sched[i].name_sub, tmp_sched[i].time) == 3)
	i++;
    count = i;
    re_align(tmp_sched, count);

    rewind(fp);
    for (i = 0; i < count; i++)
	fprintf(fp, "%6s %s %4s\n", tmp_sched[i].date, tmp_sched[i].name_sub,tmp_sched[i].time);
    fclose(fp);

    return 1;
}

/*
   함수명 : void check_sched()
   기능 : sched_info.txt 에서 읽어들여서 전체의 일정을 출력한다
   */
void check_sched()
{
    schedule tmp_sched[10000];
    schedule tmp;
    int count = 0;//전체 일정의 수
    int i = 0;
    FILE *fp;

    if ((fp = fopen("schedule_info.txt", "r")) == NULL)
	printf("파일오류\n");

    while (fscanf(fp, "%6s%s%4s", tmp_sched[i].date, tmp_sched[i].name_sub, tmp_sched[i].time) == 3)
    {
	i++;
    }
    count = i;
    // txt_read(tmp_sched);
    re_align(tmp_sched, count);
    system("clear");
    printf("------------------------------전체일정--------------------------------\n");
    printf("    날짜    |   과목이름   |    시간    \n");
    char tmp_date[10];
    strcpy(tmp_date, "000000");
    for (i = 0; i < count; i++)
    {
	if (strcmp(tmp_date, tmp_sched[i].date) != 0)
	{
	    printf("\n");
	    strcpy(tmp_date, tmp_sched[i].date);
	    printf("%10s|%10s|%10s\n", tmp_sched[i].date, tmp_sched[i].name_sub, tmp_sched[i].time);
	}
	else
	    printf("           %10s|%10s\n", tmp_sched[i].name_sub, tmp_sched[i].time);


    }
    printf("-----------------------------------------------------------------------\n");


    myflush();
    printf("아무 버튼이나 누르세요\n");
    getch();
    //system("pause");
    return;
}

/*
   함수명 : int txt_read(schedule *a)
   기능 : txt 로 저장되어 있는 현재 과목일정들을 스케쥴구조체 a 에 저장하는 함수.
return :  전체 스케줄 수 , 실패시 -1  반환.
*/

int txt_read(schedule *a)
{
    int count = 0;
    FILE *fp;

    if ((fp = fopen("schedule_info.txt", "r")) == NULL)
    {
	printf("파일오류\n");
	return -1;
    }

    while (fscanf(fp, "%6s%s%4s", a[count].date, a[count].name_sub, a[count].time) == 3)
    {
	count++;
    }
    fclose(fp);
    return count;
}


/*
   함수명 : int txt_write(schedule *a,int n)
   기능 : n 개의 수만큼의 schedule 구조체를 지정된 형식으로 txt 에 쓰는 함수.
return :  성공시 1 , 실패시 0 반환
*/

int txt_write(schedule *a, int n)
{
    FILE *fp;

    fp = fopen("schedule_info.txt", "w");


    assert(fp != NULL);
    if (fp == NULL)
	return 0;

    int i;
    for (i = 0; i < n; i++)
    {
	fprintf(fp, "%6s ", a[i].date);
	fprintf(fp, "%s %4s", a[i].name_sub, a[i].time);
	fprintf(fp, "\n");
    }
    fclose(fp);

    return 1;

}


/*
   함수명 : void re_align(schedule *a, int n)
   기능 : 전체의 일정을 시간순으로 재정렬하는 함수
   */
void re_align(schedule *a, int n)
{
    schedule tmp;
    int i, j;
    for (i = 0; i < n; i++)
    {
	for (j = i + 1; j < n; j++)
	    if (strcmp(a[i].date, a[j].date) > 0) //date 별로 정리.
	    {
		memcpy(&tmp, &a[i], sizeof(schedule));
		memcpy(&a[i], &a[j], sizeof(schedule));
		memcpy(&a[j], &tmp, sizeof(schedule));
	    }

    }

    for (i = 0; i < n; i++)
    {
	for (j = i + 1; !strcmp(a[i].date, a[j].date); j++)
	    if (strcmp(a[i].time, a[j].time) > 0)
	    {
		memcpy(&tmp, &a[i], sizeof(schedule));
		memcpy(&a[i], &a[j], sizeof(schedule));
		memcpy(&a[j], &tmp, sizeof(schedule));
	    }
    }
}


/*
   함수명 : void check_sched_date()
   기능 : 날짜별일정확인.
   */
void check_sched_date()
{

    schedule tmp_sched[10000];
    int count = 0;
    char tmp_date[10];
    char find_date[7]; //찾으려는 일정
    int res;
    int i;
    count = txt_read(tmp_sched);
    re_align(tmp_sched, count);


    while (1) {
	system("clear"); // 리눅스의 경우 clear 로 변경.
	printf("--------------------------------------------------------------\n");
	printf("   찾으시려는 일정 날짜를 다음 예와 같이 입력해주세요 ! \n");
	printf("입력 예 ex) 2017년 4월26일 -> 170426\n");
	printf("날짜 입력 : ");
	res = scanf("%s", find_date);
	if (res == 1 && strlen(find_date) == 6) break;
	myflush();
    }

    strcpy(tmp_date, "000000");

    system("clear");
    printf("------------------------------------------------------------------------\n");
    for (i = 0; i < count; i++)
    {
	if (strcmp(find_date, tmp_sched[i].date) == 0) {
	    if (strcmp(tmp_date, tmp_sched[i].date) != 0)
	    {
		printf("\n");
		strcpy(tmp_date, tmp_sched[i].date);
		printf("%10s|%10s|%10s\n", tmp_sched[i].date, tmp_sched[i].name_sub, tmp_sched[i].time);
	    }
	    else
		printf("           %10s|%10s\n", tmp_sched[i].name_sub, tmp_sched[i].time);

	}
    }
    /*
       char cur_sub[21]={0};
       cur_subject(tmp_sched,count,cur_sub);
       printf("현재 공부하고 있는 과목 : %s \n ",cur_sub);*/ // 현재 공부하고있는 과목 예시 
    printf("-----------------------------------------------------------------------\n");
    myflush();
    printf("아무 버튼이나 누르세요\n");
    getch();

    // getchar();
    return;

}

/*
   함수명 : void delete_sched()
   기능 : 날짜별일정입력 받은 후 그 부분 수정.
   */

void delete_sched()
{
    schedule tmp_sched[10000] = { 0 };
    schedule tmp;
    int count = 0;
    FILE *fp;
    int i = 0;


    int res;

    count = txt_read(tmp_sched);
    re_align(tmp_sched, count);

    while (1) {
	system("clear"); // 리눅스의 경우 clear 로 변경.
	printf("--------------------------------------------------------------\n");
	printf("   삭제하시려는 일정 날짜를 다음 예와 같이 입력해주세요 ! \n");
	printf("입력 예 ex) 2017년 4월26일 -> 170426\n");
	printf("날짜 입력 : ");
	res = scanf("%s", tmp.date);
	if (res == 1 && strlen(tmp.date) == 6) break;
	myflush();
    }

    system("clear");
    int tmp_count = 0;
    for (i = 0; i < count; i++)
    {
	if (strcmp(tmp.date, tmp_sched[i].date) == 0) {

	    tmp_count++;

	}
    }


    printf("---------------------------해당 날짜의 일정-----------------------------\n");
    if (tmp_count != 0) {
	for (i = 0; i < count; i++)
	{
	    if (strcmp(tmp.date, tmp_sched[i].date) == 0) {

		printf("\n");
		printf("%10s|%10s|%10s\n", tmp_sched[i].date, tmp_sched[i].name_sub, tmp_sched[i].time);

	    }
	}
    }

    else
    {
	printf("찾으려는 날짜의 일정이 없습니다. 일정 등록으로 등록해주세요.\n");
	//system("pause");
	myflush();
	printf("아무 버튼이나 누르세요\n");
	getch();
	return;
    }
    printf("-----------------------------------------------------------------------\n");
    while (1) {
	printf("   삭제하려는 일정 과목을 입력해주세요 ! \n");
	printf("과목 입력 : ");
	res = scanf("%s", tmp.name_sub);
	if (res == 1) break;
	myflush();
    }


    for (i = 0; i < count; i++) //----------------------------실질적인 일정삭제---------------------------
    {
	if (strcmp(tmp.date, tmp_sched[i].date) == 0) {

	    if (strcmp(tmp.name_sub, tmp_sched[i].name_sub) == 0)
	    {
		int j;
		for (j = i; j < count; j++)
		{
		    memcpy(&tmp_sched[j],&tmp_sched[j+1],sizeof(schedule) );
		}
		count--;
		break;
	    }
	    else
	    {
		printf("일정을 잘못 입력하셨습니다. \n");

		return;
	    }

	}
    }
    re_align(tmp_sched, count);

    res = txt_write(tmp_sched,count);
    if( res ==1 ) printf("일정삭제 완료! \n");
    else printf("일정삭제 실패 \n");
    // system("pause");
    myflush();
    printf("아무 버튼이나 누르세요\n");
    getch();
    return;

}
/*
   함수명 : void change_sched()
   기능 : 날짜별일정입력 받은 후 그 부분 수정.
   */

void change_sched()
{
    schedule tmp_sched[10000];
    schedule tmp;
    int i,count = 0;
    int res;
    count=txt_read(tmp_sched);
    re_align(tmp_sched, count);

    while (1) {
	system("clear"); // 리눅스의 경우 clear 로 변경.
	printf("--------------------------------------------------------------\n");
	printf("   변경하려는 일정 날짜를 다음 예와 같이 입력해주세요 ! \n");
	printf("입력 예 ex) 2017년 4월26일 -> 170426\n");
	printf("날짜 입력 : ");
	res = scanf("%s", tmp.date);
	if (res == 1 && strlen(tmp.date) == 6) break;
	myflush();
    }

    system("clear");

    while (1) {
	printf("---------------------------해당 날짜의 일정-----------------------------\n");
	if (count != 0) {
	    for (i = 0; i < count; i++)
	    {
		if (strcmp(tmp.date, tmp_sched[i].date) == 0) {

		    printf("\n");
		    printf("%10s|%10s|%10s\n", tmp_sched[i].date, tmp_sched[i].name_sub, tmp_sched[i].time);

		}
	    }
	}

	else
	{
	    printf("찾으려는 날짜의 일정이 없습니다. 일정 등록으로 등록해주세요.\n");
	    //system("pause");
	    myflush();
	    printf("아무 버튼이나 누르세요\n");
	    getch();
	    return;
	}
	printf("-----------------------------------------------------------------------\n");

	printf(" 위의  변경하려는 일정 과목을  입력해주세요 ! \n");
	printf("과목 입력 : ");
	res = scanf("%s", tmp.name_sub);
	if (res == 1 ) break;
	myflush();
    }

    int change;

    for (i = 0; i < count; i++)
    {
	if (strcmp(tmp.date, tmp_sched[i].date) == 0)
	{
	    if (strcmp(tmp.name_sub, tmp_sched[i].name_sub) == 0)
	    {
		change = i;
	    }
	}
    }

    FILE *fp;
    if ((fp = fopen("schedule_info.txt", "w")) == NULL)
	printf("파일오류\n");

    for (i = 0; i < count; i++)
    {
	if (i != change) {
	    fprintf(fp, "%6s ", tmp_sched[i].date);
	    fprintf(fp, "%s %4s", tmp_sched[i].name_sub, tmp_sched[i].time);
	    fprintf(fp, "\n");
	}

	else {
	    printf("기존 과목이름 : %s \n", tmp_sched[i].name_sub);
	    printf("바꿀 과목이름 : ");
	    res = scanf("%s", tmp_sched[i].name_sub);
	    myflush();
	    while (res != 1 || strlen(tmp_sched[i].name_sub) > 19) {

		printf("잘못 입력하셨습니다. 다시 입력해주세요. \n");
		myflush();
		res = scanf("%s", tmp_sched[i].name_sub);
	    }
	    while (1) {
		printf("과목시간을 다음 예 처럼 입력하세요  ex. 7시15분 -> 0715 \n 입력 : ");
		res = scanf("%s", tmp_sched[i].time);
		if (res == 1 && strlen(tmp_sched[i].time) == 4) break;
		myflush();
	    }

	    fprintf(fp, "%6s ", tmp_sched[i].date);
	    fprintf(fp, "%s %4s", tmp_sched[i].name_sub, tmp_sched[i].time);
	    fprintf(fp, "\n");

	}

    }


    fclose(fp);
    //system("pause");
    printf("아무 버튼이나 누르세요\n");
    myflush();
    getch();
    return;

}

char getch()
{
    char ch;
    tcgetattr(0, &save);
    buffer = save;
    buffer.c_lflag &= ~(ICANON | ECHO);
    buffer.c_cc[VMIN] = 1;
    buffer.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buffer);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);

    return ch;
}

/*
   함수명 : void cur_subject(schedule *a,int n ,char *buf)
   기능 : 지금 공부하고 있는 과목을 알게하는 함수 buf 에다 현재 공부하고 있는 과목을 저장하게 됨.
   */

void cur_subject(schedule *a,int n ,char *buf)
{
    schedule tmp_sched[10000];
    char sched_time[10000][11]; //전체 일정을 10자리 시간으로 나타냄.
    time_t ct;
    struct tm tm;
    char month[3] = {0};
    char year[3] ={0};
    char mday[3] ={0};
    char hour[3] ={0};
    char min[3] = {0};
    char nowtime[11]={0};

    int count;
    int i;
    int now;
    count = txt_read(tmp_sched);
    ct = time(NULL);
    tm = *localtime(&ct);
    //현재의 시간을 1706061545 17년도 6월6일15시45분 상태로 알아내기
    sprintf(year,"%02d", tm.tm_year%100);
    sprintf(month,"%02d", tm.tm_mon+1);
    sprintf(mday,"%02d",tm.tm_mday);
    sprintf(hour,"%02d",tm.tm_hour);
    sprintf(min,"%02d",tm.tm_min);

    strcpy(nowtime,year);
    strcpy(nowtime+2,month);
    strcpy(nowtime+4,mday);
    strcpy(nowtime+6,hour);
    strcpy(nowtime+8,min);
    // 현재시간 nowtime 에 저장

    re_align(tmp_sched,count);
    output_schedtime(tmp_sched,n,sched_time); // 현재 등록된 모든 일정을 시간배열로 나타냄.


    for(i = 0 ; i < n ; i++)
    {
		if(strcmp(sched_time[i],nowtime) <= 0);
		else
		{
			now = i - 1; //현재 공부하고 있는 일정을 찾음
			break;
		}

	}
	if (i >= n)
		now = n - 1;
	if (i == 0)
		strcpy(buf, "NOTHING");
	else
		strcpy(buf, tmp_sched[now].name_sub);
}


/*
   함수명 : void output_schedtime(schedule *a, int n, char *buf)
   기능 : schedule 구조체에서 n개만큼의 전체 일정수를 받아서 각각 buf에 1706061545형태로 저장하기.
   */
void output_schedtime(schedule *a, int n, char (*buf)[11])
{
    int i;
    for(i=0;i < n ; i++)
    {
	strcpy(buf[i],a[i].date);
	strcpy(buf[i]+6,a[i].time);
    } 

}

void sigusr1_handler(int signo)
{
    int cnt = 3;
    while(cnt--)
    {
	system("clear");
	printf("%d second remain\n", cnt);
	sleep(1);
    }
    // Send dead packet

    printf("Bye bye\n");
    exit(1);
}
/*-------------------------------------------------*/
