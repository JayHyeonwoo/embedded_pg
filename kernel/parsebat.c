#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <asm/processor.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/limits.h>

#include <linux/devinfo.h>

/* 
 * buf는 fd로부터 읽어온 데이터를 버퍼링한다 
 * fd는 파이프일 수 있기에 최대 크기는 PIPE_BUF
 */
static char buf[PIPE_BUF];

/* bufsize는 buf가 가진 데이터의 크기를 말한다 */
static off_t bufsize = 0;

/* offset은 하단에 정의된 문자 처리 함수에 의해 조정되는 오프셋 */
static off_t offset = 0;

/* 
 * fd에서 문자 하나를 읽는다
 * 내부적으로 버퍼링함
 * 프로세스의 addr_limit가 KERNEL_DS이어야 정상 수행
 *
 * Return
 * 읽기 오류이거나 더 이상 읽을 문자가 없을 경우 -1을 리턴,
 * 정상적으로 한 문자를 읽었을 경우 해당 문자를 리턴
 */
static inline int getchar_from_fd(int fd)
{
	/* buf의 모든 데이터를 읽었기에 fd로부터 다시 읽음 */
	if (offset == bufsize) {
		bufsize = sys_read(fd, buf, sizeof buf);
		offset = 0;
	}

	return bufsize == 0 || bufsize == -1 ? -1 : buf[offset++];
}

/* 
 * Return
 * fd가 읽기 오류이면 1, 아니면 0을 리턴
 */
static inline int fd_err(void)
{
	return bufsize == -1;
}

/*
 * Return
 * fd가 더 이상 읽을 문자가 없다면 1, 아니면 0을 리턴
 */
static inline int fd_eof(void)
{
	return bufsize == 0;
}

/*
 * Return
 * ch가 공백 문자이면 1, 아니면 0을 리턴
 */
static inline int isspace(int ch)
{
	switch (ch) {
		case '\n':
		case ' ':
		case '\t':
			return 1;
		default:
			return 0;
	}
}

/*
 * fd에서 n개의 줄만큼을 무시한다
 */
static inline void skip_line(int fd, int n)
{
	int ch;

	while (n--) {
		while ((ch = getchar_from_fd(fd)) != '\n') {
			if (ch == -1) return;
		}
	}
}

/*
 * fd에서 공백 문자를 무시한다
 */
static inline void skip_space(int fd)
{
	int ch;
	while ((ch = getchar_from_fd(fd)) != -1 && isspace(ch))
		;

	/* offset이 전진했기에 1을 뺌 */
	if (ch != -1) 
		offset--;
}

/*
 * fd에서 공백 문자가 아닌 문자를 무시한다
 */
static inline void skip_nospace(int fd)
{
	int ch;
	while ((ch = getchar_from_fd(fd)) != -1 && !isspace(ch))
		;

	/* offset이 전진했기에 1을 뺌 */
	if (ch != -1) 
		offset--;
}

/* 
 * long parsebat(int fd, long maxdev, struct devinfo *devinfo)
 * fd에서 문자열을 읽어 파싱한 결과를 devinfo에 최대 maxdev개 만큼 저장한다
 *
 * Return
 * 성공시 파싱한 struct devinfo의 개수, 실패시 -1을 리턴
 */
SYSCALL_DEFINE3(parsebat, int, fd, long, maxdev,
		struct devinfo __user *, devinfo)
{
	mm_segment_t oldfs;
	long ndev = 0;
	int ch, i;

	/* Backup address limit */
	oldfs = get_fs();
	set_fs(KERNEL_DS);

	/* 첫 두 줄은 무시한다 */
	skip_line(fd, 2);
	if (fd_err()) {
		printk("sys_read: error\n");
		goto errout;
	} else if (fd_eof()) {
		goto ret;
	}

	/* 최대 maxdev만큼을 검색 */
	for (; ndev < maxdev; ndev++) {
		skip_space(fd);
		if (fd_err()) {
			printk("sys_read: error\n");
			goto errout;
		} else if (fd_eof()) {
			break;
		}

		skip_nospace(fd);
		if (fd_err()) {
			printk("sys_read: error\n");
			goto errout;
		} else if (fd_eof()) {
			break;
		}

		skip_space(fd);
		if (fd_err()) {
			printk("sys_read: error\n");
			goto errout;
		} else if (fd_eof()) {
			break;
		}

		/* MAC주소를 struct devinfo에 씀 */
		for (i = 0; i < sizeof devinfo[0].mac - 1; ++i) {
			ch = getchar_from_fd(fd);
			if (fd_err()) {
				printk("sys_read: error\n");
				goto errout;
			} else if (fd_eof()) {
				goto ret;
			}

			devinfo[ndev].mac[i] = ch;
		}
		devinfo[ndev].mac[sizeof devinfo[0].mac - 1] = '\0';
		skip_line(fd, 1);
	}

ret:
	set_fs(oldfs);
	return ndev;

errout:
	set_fs(oldfs);
	return -1;
}
