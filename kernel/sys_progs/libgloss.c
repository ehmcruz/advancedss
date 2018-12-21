#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#undef errno

extern int errno;

char *__env[1] = { 0 };
char **environ = __env;

#define INVALID_SYSCALL(text)  { \
		char str[] = "invalid syscall " #text "\n"; \
		write(1, str, sizeof(str)); \
		while (1); \
	}

int read (int file, void *ptr, int len)
{
	int ret_val;

	__asm__ __volatile__(
		"movq $0, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		: "D"(file), "S"(ptr), "d"(len)
		: "%rcx", "%r11"
		);

	return ret_val;
}

int write (int file, const void *ptr, int len)
{
	int ret_val;

	__asm__ __volatile__(
		"mov $1, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		: "D"(file), "S"(ptr), "d"(len)
		: "%rcx", "%r11"
		);

	return ret_val;
}

void* sbrk (int incr)
{
	void *new_break, *ret_val;
	static void *program_break = 0;
	
	if (program_break == 0) {
		__asm__ __volatile__(
			"mov $0, %%rdi\n"
			"mov $12, %%rax\n"
			"syscall\n"
			: "=a"(program_break)
			:
			: "%rdi", "%rcx", "%r11");
	}
	
	new_break = (void*)((char*)program_break + incr);

	__asm__ __volatile__(
		"mov $12, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		: "D"(new_break)
		: "%rcx", "%r11"
		);

	if (new_break == ret_val) { // success
		ret_val = program_break;
		program_break = new_break;
	}
	else {
		ret_val = (void*)-1;
		errno = ENOMEM;
	}

	return ret_val;
}

int open (char *fname, int flags, int mode)
{
	int ret_val;
	int len;
	char n[2];

write(1, "open ", 5);
	len = 0;
	while (fname[len])
		len++;
	write(1, fname, len);
	
	__asm__ __volatile__(
		"mov $2, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		: "D"(fname), "S"(flags), "d"(mode)
		: "%rcx", "%r11"
		);
write(1, "!", 1);

	n[1] = 0;
	n[0] = ret_val + '0';
	write(1, n, 1);
	write(1, "\n", 1);


	return ret_val;
}

int close (int file)
{
	int ret_val;

	__asm__ __volatile__(
		"mov $3, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		: "D"(file)
		: "%rcx", "%r11"
		);

	return ret_val;
}

int execve(char *name, char **argv, char **env)
{
	errno = ENOMEM;
	return -1;
}

int fork()
{
	int ret_val;

	__asm__ __volatile__(
		"mov $57, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		:
		: "%rcx", "%r11"
		);

	if (ret_val == -1)
		errno = EAGAIN;

	return ret_val;
}

int getpid()
{
	int ret_val;

	__asm__ __volatile__(
		"mov $39, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		:
		: "%rcx", "%r11"
		);

	return ret_val;
}

int isatty(int file)
{
	return (file <= 2);
}

int link(char *old, char *new)
{
	INVALID_SYSCALL(link)
	errno = EMLINK;
	return -1;
}

int lseek(int file, int ptr, int dir)
{
	int ret_val;
	char n[2];
	
	if (file > 2) {
	n[1] = 0;
	n[0] = file+'0';
	write(1, n, 1);
	
	INVALID_SYSCALL(lseek)

	__asm__ __volatile__(
		"movq $8, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		: "D"(file), "S"(ptr), "d"(dir)
		: "%rcx", "%r11"
		);
	}
	else
		ret_val = 0;
	return ret_val;
}

int fstat(int file, struct stat *st)
{
	int ret_val;
static int v = 0;
	if (file >= 3) {
	char n[2];

n[1] = 0;
n[0]= file + '0';
write(1, n, 1);

//		INVALID_SYSCALL(fstat)
/*		
		__asm__ __volatile__(
			"mov $5, %%rax\n"
			"syscall\n"
			: "=a"(ret_val), "=b" (st->st_size)
			: "D" (file)
			: "%rcx", "%r11"
			);*/
st->st_size = 1000000;
//if (v==0){st->st_size = 1000000;}
//else if (v==1){st->st_size = 58;}
//else{INVALID_SYSCALL(fstat2)}
ret_val = 0;
v++;
		if (ret_val != -1) {
			st->st_dev = 0;
			st->st_ino = 0;
			st->st_mode = S_IFREG;
			st->st_nlink = 1;
			st->st_uid = 1;
			st->st_gid = 1;
			//st->st_rdev = 0;
			st->st_blksize = 4096;
			st->st_blocks = st->st_size >> 9;
			st->st_atime = 0;
			st->st_mtime = 0;
			st->st_ctime = 0;
		}
	}
	else {
		st->st_dev = 0;
		st->st_ino = 0;
		st->st_mode = S_IFCHR;
		st->st_nlink = 1;
		st->st_uid = 1;
		st->st_gid = 1;
		//st->st_rdev = 0;
		st->st_size = 0;
		st->st_blksize = 1024;
		st->st_blocks = 0;
		st->st_atime = 0;
		st->st_mtime = 0;
		st->st_ctime = 0;
		
		ret_val = 0;
	}

	return ret_val;
}

int stat(const char *path, struct stat *st)
{
	int fd, r;
	
	fd = open(path, 0, 0);
	if (fd == -1)
		return -1;
	
	r = fstat(fd, st);
	
	close(fd);
	
	return r;
}

int lstat(const char *path, struct stat *st)
{
	return stat(path, st);
}

int chmod(const char *path, mode_t mode)
{
	return 0;
}

int times(struct tms *buf)
{
	INVALID_SYSCALL(times)
	return -1;
}

int unlink(char *name)
{
	INVALID_SYSCALL(unlink)
	errno = ENOENT;
	return -1;
}



pid_t waitpid (pid_t pid, int *status, int options)
{
	int ret_val;
	
	INVALID_SYSCALL(waitpid)

	__asm__ __volatile__(
		"movq $247, %%rax\n"
		"syscall\n"
		: "=a"(ret_val)
		: "D"(pid), "S"(status), "d" (options)
		: "%rcx", "%r11"
		);

	return ret_val;
}

int wait(int *status)
{
	int r;
	r = waitpid(-1, status, 0);
	return r;
}

int kill(int pid, int sig)
{
	INVALID_SYSCALL(kill)
	
	errno = EINVAL;
	return -1;
}

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	return -1;
}









