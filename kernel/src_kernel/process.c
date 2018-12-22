#define _KERNEL_IMPLEMENTATION_PROCESS_C_

#include "process.h"
#include "vars.h"
#include "printk.h"

struct process_to_open {
	process_t *process;
	void *requester_data;
	void (*callback)(uint64_t r, void *d);
	external_hd_file_descriptor file;
};

typedef struct process_to_open process_to_open;

static process_t process_table[CONFIG_KERNEL_MAX_PROCESS];
process_t *process_idle_process[CONFIG_KERNEL_MAX_CPUS];
thread_t *process_idle_thread[CONFIG_KERNEL_MAX_CPUS];

static uint32_t next_pid = 2;
static process_to_open open_queue[CONFIG_KERNEL_PROCESS_OPEN_QUEUE_SIZE];

static void process_callback_open_file(uint64_t r, void *d);
static void process_callback_read_file(uint64_t r, void *d);

process_t *process_active_process_at_cpu[CONFIG_KERNEL_MAX_CPUS];
thread_t *process_active_thread_at_cpu[CONFIG_KERNEL_MAX_CPUS];

static thread_t *process_last_scheduled_thread;
static process_t *process_last_scheduled_process;

process_t *process_blocked_window_keyboard[CONFIG_KERNEL_NUMBER_OF_WINDOWS];

void initialize_process_table()
{
	uint32_t i;

	for (i=0; i<CONFIG_KERNEL_MAX_PROCESS; i++) {
		process_table[i].state = PROCESS_STATE_EMPTY;
	}
	
	for (i=0; i<CONFIG_KERNEL_PROCESS_OPEN_QUEUE_SIZE; i++) {
		open_queue[i].process = NULL;
	}
	
	for (i=0; i<CONFIG_KERNEL_MAX_CPUS; i++) {
		process_active_process_at_cpu[i] = NULL;
		process_active_thread_at_cpu[i] = NULL;
	}
	
	for (i=0; i<CONFIG_KERNEL_NUMBER_OF_WINDOWS; i++) {
		process_blocked_window_keyboard[i] = NULL;
	}
}

process_t* process_allocate(process_t *parent)
{
	uint32_t i, j;
	process_t *p;

	for (i=0; i<CONFIG_KERNEL_MAX_PROCESS; i++) {
		p = process_table + i;
		if (p->state == PROCESS_STATE_EMPTY) {
			p->state = PROCESS_STATE_OCCUPIED;
			for (j=0; j<CONFIG_KERNEL_MAX_THREADS_PER_PROCESS; j++) {
				p->threads[j].state = THREAD_STATE_EMPTY;
				p->threads[j].owner = p;
			}
			for (j=0; j<CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS; j++) {
				p->files[j].avl = 0;
			}
			//video_clear_buffer(&p->video_buffer);
			p->pid = next_pid++;
			p->keyboard_buffer_used = 0;
			p->virtual_memory_table = memory_initialize_table(p->pid);
			p->stack_start = 0;
			p->stack_end = 0;
			p->heap_end = 0;
			p->parent = parent;
			if (parent != NULL) {
				p->video_buffer = parent->video_buffer;
			}
			return p;
		}
	}

	return NULL;
}

void process_kill_process(process_t *p)
{

}

void process_free_resources(process_t *p)
{
	uint32_t i;
	
	for (i=0; i<CONFIG_KERNEL_MAX_THREADS_PER_PROCESS; i++) {
		KERNEL_ASSERT(p->threads[i].state == THREAD_STATE_EMPTY || p->threads[i].state == THREAD_STATE_READY);
	}

	for (i=0; i<CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS; i++) {
		if (p->files[i].avl == 1) {
			p->files[i].avl = 0;
		}
	}
	
	memory_free_process(p);
}

void process_prepare_for_execve(process_t *p)
{
	process_free_resources(p);
	p->virtual_memory_table = memory_initialize_table(p->pid);
}

thread_t* process_fork(process_t *dest, process_t *src)
{
	uint32_t i;
	thread_t *thread = NULL;
	
	dest->state = src->state;
	memcpy(dest->files, src->files, sizeof(external_hd_file_descriptor) * CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS);
	strcpy(dest->fname, src->fname);
	
	for (i=0; i<CONFIG_KERNEL_MAX_THREADS_PER_PROCESS; i++) {
		KERNEL_ASSERT(src->threads[i].state == THREAD_STATE_EMPTY || src->threads[i].state == THREAD_STATE_READY);
		memcpy(dest->threads + i, src->threads +i, sizeof(thread_t));
		dest->threads[i].owner = dest;
		if (src->threads[i].state == THREAD_STATE_READY) {
			KERNEL_ASSERT(thread == NULL);
			thread = dest->threads + i;
		}
	}

	memory_copy_virtual_memory_space(dest, src);

	dest->stack_start = src->stack_start;
	dest->stack_end = src->stack_end;
	dest->heap_end = src->heap_end;
	dest->static_heap_end = src->static_heap_end;
	dest->heap_allocated = src->heap_allocated;

	return thread;
}

external_hd_file_descriptor* process_allocate_file(process_t *process)
{
	uint32_t i;
	
	for (i=0; i<CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS; i++) {
		if (process->files[i].avl == 0) {
			process->files[i].avl = 1;
			return process->files + i;
		}
	}
	
	return NULL;
}

uint32_t process_get_file_id(process_t *process, external_hd_file_descriptor *file)
{
	uint32_t i;
	
	for (i=0; i<CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS; i++) {
		if ((process->files + i) == file)
			return i+1000;
	}
	
	KERNEL_ASSERT(0);
}

external_hd_file_descriptor* process_get_file_by_id(process_t *p, uint64_t id)
{
	external_hd_file_descriptor *r;
	if (id < 1000)
		r = NULL;
	else {
		id -= 1000;
		if (id >= CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS)
			r = NULL;
		else if (p->files[id].avl == 0)
			r = NULL;
		else
			r = p->files + id;
	}
	return r;
}

uint8_t process_is_not_idle_thread(thread_t *t)
{
	uint32_t i;
	
	for (i=0; i<CONFIG_KERNEL_MAX_CPUS; i++) {
		if (t == process_idle_thread[i])
			return 0;
	}
	
	return 1;
}

void process_thread_save_context(process_t *p, thread_t *t)
{
	uint8_t i;
	
	for (i=0; i<CONFIG_KERNEL_NUMBER_GPR; i++) {
		t->gpr[i] = interrupt_context_saved_gpr[i];
	}

	t->rip = interrupt_context_saved_rip;
	t->rflags = interrupt_context_saved_rflags;
}

void process_thread_load_context(process_t *p, thread_t *t)
{
	uint8_t i;
	
	for (i=0; i<CONFIG_KERNEL_NUMBER_GPR; i++) {
		interrupt_context_saved_gpr[i] = t->gpr[i];
	}

	interrupt_context_saved_rip = t->rip;
	interrupt_context_saved_rflags = t->rflags;
	
//	printk("process %s context loaded...addr 0x%llX  rip=0x%llX\n", p->fname, interrupt_context_saved_rip, t->rip);
}

void process_save_syscall_response(process_t *process, thread_t *thread, uint8_t reg, uint64_t r)
{
	thread->gpr[reg] = r;
	if (process_active_thread_at_cpu[ get_cpuid() ] == thread) {
		interrupt_context_saved_gpr[reg] = r;
	}
}

void process_schedule_thread(process_t *process, thread_t *thread)
{
	uint64_t *stack_base;
	uint32_t cpuid;
	
//	printk("thread state: %u\n", thread->state);
	
	KERNEL_ASSERT(thread->state == THREAD_STATE_READY);
	
	cpuid = get_cpuid();
	
	thread->state = THREAD_STATE_RUNNING;
	
	process_last_scheduled_thread = thread;
	process_last_scheduled_process = process;
	
	if (process_active_thread_at_cpu[ cpuid ] != thread) {	
		process_thread_load_context(process, thread);
	
		memory_setup_page_base_addr(process);
	
		process_active_process_at_cpu[ cpuid ] = process;
		process_active_thread_at_cpu[ cpuid ] = thread;
	}
}

thread_t* process_allocate_thread(process_t *process, uint64_t rip)
{
	uint32_t i, j, k;
	thread_t *t;
	uint64_t stack;
	void *ptr;

	for (i=0; i<CONFIG_KERNEL_MAX_THREADS_PER_PROCESS; i++) {
		t = process->threads + i;
		if (t->state == THREAD_STATE_EMPTY) {
			t->state = THREAD_STATE_READY;

			for (j=0; j<CONFIG_KERNEL_NUMBER_GPR; j++) {
				t->gpr[i] = 0;
			}
			
			t->gpr[11] = 0x200;

			// create a stack for thread
			// be careful because threads can't share stacks
//			stack = (uint64_t)1 << 39;
//			stack -= i << 13;

			//stack = 0x7FFFFFFFDF40 - 16;
			stack = 0x7FFFFFFFDFFF;
			t->gpr[CONFIG_KERNEL_GPR_CODE_RSP] = stack;
			if (process->stack_start == 0) {
				process->stack_start = stack | 0x0FFF;
			}

			for (k=0; k<CONFIG_KERNEL_THREAD_DEFAULT_STACK_SIZE; k+=CONFIG_KERNEL_PAGE_SIZE) {
				ptr = memory_create_page(process, stack, MEMORY_FLAG_WRITE);
				KERNEL_ASSERT(ptr != NULL);
				stack -= CONFIG_KERNEL_PAGE_SIZE;
			}
			
			process->stack_end = process->stack_start - (CONFIG_KERNEL_THREAD_DEFAULT_STACK_SIZE - 1);

			t->rflags = 0;
			t->rip = rip;

			return t;
		}
	}
	
	return NULL;
}

void process_load(process_t *process, char *fname, void (*callback)(uint64_t r, void *d), void *requester_data)
{
	uint32_t i;
	process_to_open *p;
	
	for (i=0; i<CONFIG_KERNEL_PROCESS_OPEN_QUEUE_SIZE; i++) {
		p = open_queue + i;
		if (p->process == NULL) {
			p->process = process;
			strcpy(process->fname, fname);
			p->callback = callback;
			p->requester_data = requester_data;
			
			external_hd_request_open_file(&p->file, fname, process_callback_open_file, p);
			
			return;
		}
	}
	
	printk("Too many process to open!");
	panic();
}

void process_scheduler_timer_interrupt()
{
	printk("timer interrupt occured\n");
	interrupt_happened();
	process_scheduler_schedule();
}

thread_t* process_scheduler_schedule()
{
	uint32_t i, j, cpuid;
	process_t *p;
	thread_t *t;
	
	cpuid = get_cpuid();

	p = process_last_scheduled_process + 1;

	for (i=0; i<CONFIG_KERNEL_MAX_PROCESS; i++) {	
		if (p == (process_table + CONFIG_KERNEL_MAX_PROCESS))
			p = process_table;

		if (p->state == PROCESS_STATE_OCCUPIED) {
			for (j=0; j<CONFIG_KERNEL_MAX_THREADS_PER_PROCESS; j++) {
				t = p->threads + j;
			
				if (t->state == THREAD_STATE_READY && process_is_not_idle_thread(t)) {
					uint64_t rip;
					rip = (process_active_thread_at_cpu[ cpuid ] != t) ? t->rip: interrupt_context_saved_rip;
					printk("scheduling process pid %u (%s) thread %u rip=0x%llX...\n", p->pid, p->fname, j, rip);
					process_schedule_thread(p, t);
					return t;
				}
			}
		}
		
		p++;
	}
	
	printk("scheduling idle process...\n");
	
	// all process are blocked, so schedule the idle process
	process_idle_thread[ cpuid ]->state = THREAD_STATE_READY;
//	printk("state: %u\n", d->state);
	process_schedule_thread(process_idle_process[ cpuid ], process_idle_thread[ cpuid ]);
	return process_idle_thread[ cpuid ];
}

static uint8_t buffer_read[CONFIG_KERNEL_PROCESS_BIN_FILE_MAX_SIZE];

struct __attribute__ ((__packed__)) process_elf64_header {
	uint8_t ident[16];
	uint16_t type;
	uint16_t machine;
	uint32_t version;
	uint64_t entry;
	uint64_t phoff;
	uint64_t shoff;
	uint32_t flags;
	uint16_t ehsize;
	uint16_t phentsize;
	uint16_t phnum;
	uint16_t shentsize;
	uint16_t shnnum;
	uint16_t shstrndx;
};

typedef struct process_elf64_header process_elf64_header;

struct __attribute__ ((__packed__)) process_elf64_program_header {
	uint32_t type;
	uint32_t flags;
	uint64_t offset;
	uint64_t vaddr;
	uint64_t paddr;
	uint64_t filesz;
	uint64_t memsz;
	uint64_t align;
};

typedef struct process_elf64_program_header process_elf64_program_header;

#define PROCESS_ELF64_PROGRAM_HEADER_TYPE_LOAD     0x01

static void process_callback_open_file(uint64_t r, void *d)
{
	process_to_open *p;
	
	p = (process_to_open*)d;
	
	if (!r) {
		printk("process file %s failled to open!\n", p->file.fname);
		(*p->callback)(0, p->requester_data);
		return;
	}
	printk("process file %s openned!\n", p->file.fname);
	KERNEL_ASSERT(p->file.fsize < CONFIG_KERNEL_PROCESS_BIN_FILE_MAX_SIZE);
	external_hd_request_read_file(&p->file, buffer_read, p->file.fsize, process_callback_read_file, p);
}

static void process_callback_read_file(uint64_t r, void *d)
{
	process_to_open *p;
	process_elf64_header *elf_header;
	process_elf64_program_header *program_header;
	uint32_t i, page_to_write, page_start, fname_len;
	uint8_t *ptr, *ptr_;
	uint64_t vaddr, vaddr_max, j, z, k, max, last_addr, vaddr_args, n_args, null_par, vaddr_;
	thread_t *thread;
	uint8_t flags, copy_result;
	
	p = (process_to_open*)d;
	
	elf_header = (process_elf64_header*)buffer_read;
	
//	printk("program binary size is %llu\n", r);
	printk("program entry: 0x%llX\n", elf_header->entry);
	printk("program header start is %llu bytes (n=%u)\n", elf_header->phoff, elf_header->phnum);
	
	program_header = (process_elf64_program_header*)(buffer_read + elf_header->phoff);
//		printk("prog %u\n", elf_header->phnum);
	for (i=0; i<elf_header->phnum; i++) {
		printk("header %u: %u\n", i, elf_header->phnum);
		if (program_header->type == PROCESS_ELF64_PROGRAM_HEADER_TYPE_LOAD) {
			printk("h_%u: file offset 0x%llX -> vaddr 0x%llX\n", i, program_header->offset, program_header->vaddr);
			printk("h_%u: file size 0x%llX -> memsize 0x%llX\n", i, program_header->filesz, program_header->memsz);
			
			last_addr = program_header->vaddr + program_header->memsz - 1;
			if (last_addr > p->process->heap_end) {
				p->process->heap_end = last_addr | 0x0FFF;
				p->process->static_heap_end = last_addr | 0x0FFF;
				p->process->heap_allocated = last_addr | 0x0FFF; // points to page end
			}

			vaddr = program_header->vaddr;
			z = program_header->offset;
			max = program_header->offset + program_header->filesz;
			vaddr_max = vaddr + program_header->memsz;
			
			flags = 0;
			if (program_header->flags & 0x01)
				flags |= MEMORY_FLAG_EXECUTE;
			if (program_header->flags & 0x02)
				flags |= MEMORY_FLAG_WRITE;
			
			while (vaddr < vaddr_max) {
				ptr = memory_create_page(p->process, vaddr, flags);

				page_start = vaddr & 0x0FFF;
				page_to_write = CONFIG_KERNEL_PAGE_SIZE - page_start;
				
				printk("page_start=0x%llX        page_to_write=%u\n", page_start, page_to_write);

				for (k=page_start; k<CONFIG_KERNEL_PAGE_SIZE && z<max; k++) {
					ptr[k] = buffer_read[z];
					z++;
				}
				
				vaddr += page_to_write;
			}
		}
		program_header++;
	}//while(1);
	
//	printk("program entry: 0x%llX\n", elf_header->entry);while(1);
	
	thread = process_allocate_thread(p->process, elf_header->entry);
	KERNEL_ASSERT(thread != NULL);
	
	/*
		add program parameters
		allocate 1 page in the end to the parameters
	*/
	
	fname_len = strlen(p->process->fname) + 1;
	null_par = NULL;	
	n_args = 1;
	
	// allocate page for args
	
	vaddr_args = p->process->heap_end + 1;
	vaddr_ = vaddr_args;
	ptr = memory_create_page(p->process, vaddr_args, 0);
	KERNEL_ASSERT(ptr != NULL);
	ptr_ = ptr;
	
	// write the arguments in the page
	
	strcpy(ptr_, p->process->fname);
	ptr_ += fname_len;
	vaddr_ += fname_len;
	
	// write file name in the page

	strcpy(ptr_, p->process->fname);
	ptr_ += fname_len;
	vaddr_ += fname_len;
	
	// write NULL on the page

	memcpy(ptr_, &null_par, sizeof(uint64_t));
	ptr_ += sizeof(uint64_t);
	vaddr_ += sizeof(uint64_t);

	// push NULL

	thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP] -= 8;
	copy_result = memory_copy_data_to_user_space(p->process, thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP], sizeof(uint64_t), &null_par);
	KERNEL_ASSERT(copy_result == 0);

	// push NULL

	thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP] -= 8;
	copy_result = memory_copy_data_to_user_space(p->process, thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP], sizeof(uint64_t), &null_par);
	KERNEL_ASSERT(copy_result == 0);

	// push NULL

	thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP] -= 8;
	copy_result = memory_copy_data_to_user_space(p->process, thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP], sizeof(uint64_t), &null_par);
	KERNEL_ASSERT(copy_result == 0);

	// push the address of file name
	
	thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP] -= 8;
	copy_result = memory_copy_data_to_user_space(p->process, thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP], sizeof(uint64_t), &vaddr_args);
	KERNEL_ASSERT(copy_result == 0);
	
	// push arguments count

	thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP] -= 8;
	copy_result = memory_copy_data_to_user_space(p->process, thread->gpr[CONFIG_KERNEL_GPR_CODE_RSP], sizeof(uint64_t), &n_args);
	KERNEL_ASSERT(copy_result == 0);

	// setup the end of the program memory
	
	p->process->heap_allocated = (p->process->heap_allocated+1) | 0x0FFF;	
	p->process->heap_end = p->process->heap_allocated;
	p->process->static_heap_end = p->process->heap_allocated;
	
	/*
		finished adding program parameters
	*/

	thread->state = THREAD_STATE_READY;
	
	strcpy(p->process->fname, p->file.fname);
	
	p->process = NULL; // free from open queue
	
	(*p->callback)(1, p->requester_data);
}

thread_t* process_warn_keyboard_typed(process_t *p, uint8_t key)
{
	thread_t *thread = NULL;
	
	if (key >= 32 || key == SIM_KEYBOARD_SCAN_CODE_NEWLINE) { // printable chars
		if (p->keyboard_buffer_used < (CONFIG_KERNEL_PROCESS_KEYBOARD_BUFFER_SIZE-1)) {
			p->keyboard_buffer[ p->keyboard_buffer_used++ ] = key;
			
			video_set_cursor_x_pos(p->video_buffer, p->keyboard_cursor_pos_x);
			p->keyboard_buffer[p->keyboard_buffer_used] = 0;
			video_print_string(p->video_buffer, p->keyboard_buffer);
		}
		if (key == SIM_KEYBOARD_SCAN_CODE_NEWLINE)
			thread = process_check_unblock_keyboard(p);
	}
	else if (key == SIM_KEYBOARD_SCAN_CODE_BACKSPACE) {
		if (p->keyboard_buffer_used > 0) {
			p->keyboard_buffer_used--;
			
			video_set_cursor_x_pos(p->video_buffer, p->keyboard_cursor_pos_x);
			p->keyboard_buffer[p->keyboard_buffer_used] = 0;
			video_print_string(p->video_buffer, p->keyboard_buffer);
		}
	}
	// else discard key
	return thread;
}

thread_t* process_check_unblock_keyboard(process_t *p)
{
	thread_t *t;
	uint32_t i, j;
	uint8_t copy_result;
	//printk("checking process %s\n",p->fname);
	
/*		video_print_string(&process_focus_process->video_buffer, "checkin process ");
		video_print_string(&process_focus_process->video_buffer, p->fname);
		video_print_string(&process_focus_process->video_buffer, "\n");
		video_refresh(&process_focus_process->video_buffer);*/
	
	for (i=0; i<CONFIG_KERNEL_MAX_THREADS_PER_PROCESS; i++) {
		t = p->threads + i;
//		printk("thread %u state is %u\n",i, t->state);
		if (t->state == THREAD_STATE_BLOCKED_KB) {
			uint32_t max;
//			printk("used %u   req %u\n", p->keyboard_buffer_used, t->blocked_keyboard_len);
			if (p->keyboard_buffer_used < t->blocked_keyboard_len)
				max = p->keyboard_buffer_used;
			else
				max = t->blocked_keyboard_len;
			
			process_save_syscall_response(p, t, CONFIG_KERNEL_GPR_CODE_RAX, max);
				
			//printk("thread %u unblocked!\n", i);
			t->state = THREAD_STATE_READY;
			copy_result = memory_copy_data_to_user_space(p, t->blocked_keyboard_vaddr, max, p->keyboard_buffer);
			KERNEL_ASSERT(copy_result == 0);
			
			// now, clear the buffer
			p->keyboard_buffer_used = 0;
			
			return t;
		}
	}
	
	return NULL;
}













