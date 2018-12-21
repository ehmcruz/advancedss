#include "syscalls.h"
#include "sys_types.h"
#include "vars.h"
#include "printk.h"

struct syscall_buffer_t {
	uint8_t buffer[CONFIG_KERNEL_SIZE_SYSCALL_BUFFER];
	thread_t *thread;
};

typedef struct syscall_buffer_t syscall_buffer_t;

static syscall_buffer_t syscall_buffer[CONFIG_KERNEL_NUMBER_SYSCALL_BUFFER];

void initialize_syscalls()
{
	uint32_t i;

	for (i=0; i<CONFIG_KERNEL_NUMBER_SYSCALL_BUFFER; i++) {
		syscall_buffer[i].thread = NULL;
	}
}

static void* syscall_allocate_buffer(thread_t *t, uint32_t len)
{
	uint32_t i;
	
	if (len >= CONFIG_KERNEL_SIZE_SYSCALL_BUFFER) {
		printk("requested buffer of length %u\n", len);
		KERNEL_ASSERT(0);
	}

	for (i=0; i<CONFIG_KERNEL_NUMBER_SYSCALL_BUFFER; i++) {
		if (syscall_buffer[i].thread == NULL) {
			syscall_buffer[i].thread = t;
			return syscall_buffer[i].buffer;
		}
	}

	KERNEL_ASSERT(0);
}

static void syscall_free_buffer(void *buffer)
{
	uint32_t i;

	for (i=0; i<CONFIG_KERNEL_NUMBER_SYSCALL_BUFFER; i++) {
		if (syscall_buffer[i].buffer == (uint8_t*)buffer) {
			syscall_buffer[i].thread = NULL;
			return;
		}
	}

	KERNEL_ASSERT(0);
}

static void syscall_read_callback(uint64_t r, void *d)
{
	thread_t *t;
	process_t *p;
	uint8_t copy_result;
	
	t = (thread_t*)d;
	
	KERNEL_ASSERT(t->state == THREAD_STATE_BLOCKED_FILE_READ);
	
	p = t->owner;
	t->state = THREAD_STATE_READY;

	copy_result = memory_copy_data_to_user_space(p, t->file_buffer, r, t->syscall_buffer);
	KERNEL_ASSERT(copy_result == 0);

	process_save_syscall_response(t->owner, t, CONFIG_KERNEL_GPR_CODE_RAX, r);

	syscall_free_buffer(t->syscall_buffer);
}

void syscall_read (uint64_t file, uint64_t ptr, uint64_t len)
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
printk("read syscall file=%llu len=%u\n", file, len);//while(1);
//printk_(p->video_buffer, "read syscall file=%llu len=%u\n", file, len);
//		video_print_string(&p->video_buffer, "read syscall\n");
//		if (p == process_focus_process)
//			video_refresh(&p->video_buffer);
	if (file == CONFIG_KERNEL_CODE_STDIN) {
		uint8_t window;
		t->state = THREAD_STATE_BLOCKED_KB;
		t->blocked_keyboard_len = len;
		t->blocked_keyboard_vaddr = (uint64_t)ptr;
		
		p->keyboard_cursor_pos_x = p->video_buffer->cursor_x;
		window = video_get_window_id( p->video_buffer );
		process_blocked_window_keyboard[window] = p;
	}
	else {
		external_hd_file_descriptor *file_stc;
		file_stc = process_get_file_by_id(p, file);
//printk("");
		if (file_stc != NULL) {
			t->state = THREAD_STATE_BLOCKED_FILE_READ;
			t->file_blocked = file_stc;
			t->file_length_operation = len;
			t->file_buffer = ptr;
			t->syscall_buffer = syscall_allocate_buffer(t, len);

			external_hd_request_read_file(file_stc, t->syscall_buffer, len, syscall_read_callback, t);
		}
		else {
			process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, -1);
		}
	}
}

static void syscall_write_callback(uint64_t r, void *d)
{
	thread_t *t;
	process_t *p;
	
	t = (thread_t*)d;
	
	KERNEL_ASSERT(t->state == THREAD_STATE_BLOCKED_FILE_WRITE);
	
	p = t->owner;
	t->state = THREAD_STATE_READY;

	process_save_syscall_response(t->owner, t, CONFIG_KERNEL_GPR_CODE_RAX, r);

	syscall_free_buffer(t->syscall_buffer);
}

static uint8_t syscall_write_stdout_buffer[CONFIG_KERNEL_SIZE_SYSCALL_BUFFER];

void syscall_write (uint64_t file, uint64_t ptr, uint64_t len)
{
	process_t *p;
	thread_t *t;
	uint8_t copy_response;
	uint32_t cpuid;
	
	interrupt_happened();
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
	
printk("write syscall file=%llu  len=%u\n", file, len);
//printk_(p->video_buffer, "write syscall file=%llu  len=%u\n", file, len);
	
	if (file == CONFIG_KERNEL_CODE_STDOUT) {
		KERNEL_ASSERT(len < CONFIG_KERNEL_SIZE_SYSCALL_BUFFER-1);

		copy_response = memory_copy_data_from_user_space(p, (uint64_t)ptr, len, syscall_write_stdout_buffer);
		KERNEL_ASSERT(copy_response == 0);
		
		syscall_write_stdout_buffer[len] = 0;

		video_print_string(p->video_buffer, syscall_write_stdout_buffer);
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, len);
	}
	else {
		external_hd_file_descriptor *file_stc;
		file_stc = process_get_file_by_id(p, file);
//printk("");
		if (file_stc != NULL) {
			t->state = THREAD_STATE_BLOCKED_FILE_WRITE;
			t->file_blocked = file_stc;
			t->file_length_operation = len;
			t->file_buffer = ptr;
			t->syscall_buffer = syscall_allocate_buffer(t, len);
			
			copy_response = memory_copy_data_from_user_space(p, ptr, len, t->syscall_buffer);
			KERNEL_ASSERT(copy_response == 0);

			external_hd_request_write_file(file_stc, t->syscall_buffer, len, syscall_write_callback, t);
		}
		else {
			process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, -1);
		}
	}
}

static void syscall_open_callback(uint64_t r, void *d)
{
	thread_t *t;
	uint64_t r_;
	
	t = (thread_t*)d;
	
	KERNEL_ASSERT(t->state == THREAD_STATE_BLOCKED_FILE_OPEN);
	
	t->state = THREAD_STATE_READY;
	r_ = (r != 0) ? process_get_file_id(t->owner, t->file_blocked) : -1;
	process_save_syscall_response(t->owner, t, CONFIG_KERNEL_GPR_CODE_RAX, r_);
}

void syscall_open (char *fname, int32_t flags, int32_t mode)
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	external_hd_file_descriptor *file;
	static char file_name[CONFIG_KERNEL_MAX_FILE_NAME_LENGTH];
	uint8_t copy_response;
	uint32_t i;
//video_set_focus_window(0);	
	interrupt_happened();
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];

	file = process_allocate_file(p);
	if (file != NULL) {
		t->state = THREAD_STATE_BLOCKED_FILE_OPEN;
		t->file_blocked = file;
//printk("file allocated!\n");
		i = 0;
		while (1) {
			copy_response = memory_copy_data_from_user_space(p, (uint64_t)fname + i, 1, file_name+i);
			KERNEL_ASSERT(copy_response == 0);
			if (file_name[i] == 0)
				break;
			else
				i++;
		}
//printk_(p->video_buffer, "open syscall file=%s\n", file_name);
//printk("file name is %s!\n", file_name);while(1);
		external_hd_request_open_file(file, file_name, syscall_open_callback, t);
	}
	else {
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, -1);
	}
}

void syscall_brk (uint64_t new_brk)
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	void *ptr;
	int64_t incr, b=0;
	uint8_t response;

//video_set_focus_window(0);	
	interrupt_happened();
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];

	incr = new_brk - p->heap_end - 1;

	if (new_brk == 0) {
//		printk_(p->video_buffer, "sbrk syscall incr=0     heap_end=0x%llX\n", p->heap_end+1);//while(1);
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, p->heap_end+1);
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RCX, -1);
	}
	else if (incr > 0) {
		printk_(p->video_buffer, "sbrk syscall incr=%lli\n", incr);//while(1);
		printk("sbrk syscall incr=%lli\n", incr);//while(1);
		
		if ((int64_t)((int64_t)p->heap_allocated - (int64_t)p->heap_end) < incr) {
			uint64_t end;
			
			end = (p->heap_end + incr) | 0x0FFF;
			
			response = memory_create_n_pages(p, p->heap_allocated+1, end, MEMORY_FLAG_WRITE);
			KERNEL_ASSERT(response != 0);
			p->heap_allocated = end;
		}
		
/*		while ((int64_t)((int64_t)p->heap_allocated - (int64_t)p->heap_end) < incr) {
			ptr = memory_create_page(p, p->heap_allocated+1, MEMORY_FLAG_WRITE);
			KERNEL_ASSERT(ptr != NULL);
			p->heap_allocated = (p->heap_allocated + 1) | 0x0FFF; // points to page end
			if (b++ == 100) {printk_(p->video_buffer, "     still: %lli\n", incr - ((int64_t)p->heap_allocated - (int64_t)p->heap_end));b=0;}
		}*/
		
		p->heap_end += incr;
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, p->heap_end+1);
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RCX, -1);
	}
	else {
		int64_t decr = -incr;
//		printk_(p->video_buffer, "sbrk syscall decr=%llu\n", decr);//while(1);
		printk("sbrk syscall decr=%llu\n", decr);//while(1);
		p->heap_end -= decr;
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, p->heap_end+1);
	}
}

static void syscall_close_callback(uint64_t r, void *d)
{
	thread_t *t;
	uint64_t r_;
	
	t = (thread_t*)d;
	
	KERNEL_ASSERT(t->state == THREAD_STATE_BLOCKED_FILE_CLOSE);
	
	t->state = THREAD_STATE_READY;
	t->file_blocked->avl = 0; // free the file
	r_ = (r == 1) ? 0 : -1;
	process_save_syscall_response(t->owner, t, CONFIG_KERNEL_GPR_CODE_RAX, r_);
}

void syscall_close (uint64_t file)
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	external_hd_file_descriptor *file_stc;
	
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
	printk("close syscall file=%llu\n", file);
//		video_print_string(&p->video_buffer, "read syscall\n");
//		if (p == process_focus_process)
//			video_refresh(&p->video_buffer);

	file_stc = process_get_file_by_id(p, file);
//printk("");
	if (file_stc != NULL) {
		t->state = THREAD_STATE_BLOCKED_FILE_CLOSE;
		t->file_blocked = file_stc;

		external_hd_request_close_file(file_stc, syscall_close_callback, t);
	}
	else {
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, -1);
	}
}

void syscall_uname(uint64_t stc_vaddr)
{
	static struct new_utsname stc = {
		"fake os",
		"node...",
		"0.1a",
		"wind",
		"pentium sim",
		"localdomain"
	};
	uint8_t copy_result;
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	
	interrupt_happened();
	
	cpuid = get_cpuid();
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
	
	copy_result = memory_copy_data_to_user_space(p, stc_vaddr, sizeof(struct new_utsname), &stc);
	KERNEL_ASSERT(copy_result == 0);
	process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, 0);
}

void syscall_fstat (uint64_t file)
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	external_hd_file_descriptor *file_stc;
	
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
//	printk("close syscall file=%llu\n", file);
//		video_print_string(&p->video_buffer, "read syscall\n");
//		if (p == process_focus_process)
//			video_refresh(&p->video_buffer);

	if (file >= 3) {
		file_stc = process_get_file_by_id(p, file);
	//printk("");
		if (file_stc != NULL) {
			process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, 0);
			process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RBX, file_stc->fsize);
		}
		else {
			process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, -1);
		}
	}
	else {
		printk("requested fstat to file %llu\n", file);
		panic();
	}
}

void syscall_fork ()
{
	process_t *p;
	process_t *new_process;
	thread_t *new_thread;
	thread_t *t;
	uint32_t cpuid;
	
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
	
	printk("fork pid=%u at rip=0x%llX\n", p->pid, t->rip);

	new_process = process_allocate(p);
	if (new_process == NULL) {
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, -1);
	}
	else {
		new_thread = process_fork(new_process, p);
		KERNEL_ASSERT(new_thread != NULL);
		process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, new_process->pid);
		process_save_syscall_response(new_process, new_thread, CONFIG_KERNEL_GPR_CODE_RAX, 0);
//		new_thread->state=10;
	}
}

void syscall_getpid ()
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
	
	process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, p->pid);
}

void syscall_writev ()
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];
	
	process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, -1);
}

void syscall_invalid(uint64_t syscall_number)
{
	process_t *p;
	thread_t *t;
	uint32_t cpuid;
	
//video_set_focus_window(0);
	interrupt_happened();
	
	cpuid = get_cpuid();
	
	p = process_active_process_at_cpu[ cpuid ];
	t = process_active_thread_at_cpu[ cpuid ];

	printk_(p->video_buffer, "\nkernel message: %s: invalid syscall at rip=0x%llX number=%llu\n", p->fname, t->rip, syscall_number);
	p->state = PROCESS_STATE_EMPTY;
	//halt();
}






