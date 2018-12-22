#ifndef _KERNEL_HEADER_PROCESS_H_
#define _KERNEL_HEADER_PROCESS_H_

#include "config.h"
#include "lib.h"
#include "memory.h"
#include "external_hd.h"
#include "video.h"

typedef uint32_t thread_state_t;

#define THREAD_STATE_EMPTY                    0
#define THREAD_STATE_READY                    1
#define THREAD_STATE_RUNNING                  2
#define THREAD_STATE_BLOCKED_KB               3
#define THREAD_STATE_BLOCKED_FILE_OPEN        4
#define THREAD_STATE_BLOCKED_FILE_READ        5
#define THREAD_STATE_BLOCKED_FILE_WRITE       6
#define THREAD_STATE_BLOCKED_FILE_SEEK        7
#define THREAD_STATE_BLOCKED_FILE_CLOSE       8

struct process_t;

struct thread_stc_t {
	uint64_t gpr[CONFIG_KERNEL_NUMBER_GPR];
	uint64_t rip;
	uint64_t rflags;

	thread_state_t state;
	
	uint32_t blocked_keyboard_len;
	uint64_t blocked_keyboard_vaddr;
	
	struct process_t *owner;
	
	external_hd_file_descriptor *file_blocked;
	uint64_t file_length_operation;
	uint64_t file_buffer;

	void *syscall_buffer;
};

typedef struct thread_stc_t thread_t;

/*******************************/

typedef uint32_t process_state_t;

#define PROCESS_STATE_EMPTY      0
#define PROCESS_STATE_OCCUPIED   1

struct process_t {
	process_state_t state;
	uint32_t pid;
	thread_t threads[CONFIG_KERNEL_MAX_THREADS_PER_PROCESS];
	video_data_t *video_buffer;
	external_hd_file_descriptor files[CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS];
	x86_64_virtual_memory_pg_el *virtual_memory_table;
	char fname[CONFIG_KERNEL_MAX_FILE_NAME_LENGTH];
	uint8_t keyboard_buffer[CONFIG_KERNEL_PROCESS_KEYBOARD_BUFFER_SIZE];
	uint32_t keyboard_buffer_used;
	uint32_t keyboard_cursor_pos_x;
	struct process_t *parent;
	uint64_t stack_start, stack_end;
	uint64_t heap_end, static_heap_end, heap_allocated;
	
	/* heap allocated always points to last address of the last page */
};

typedef struct process_t process_t;

/*******************************/

void initialize_process_table();
process_t* process_allocate(process_t *parent);
void process_schedule_thread(process_t *process, thread_t *thread);
thread_t* process_allocate_thread(process_t *process, uint64_t rip);
void process_load(process_t *process, char *fname, void (*callback)(uint64_t r, void *d), void *requester_data);
thread_t* process_scheduler_schedule();
uint8_t process_is_not_idle_thread(thread_t *t);
void process_thread_save_context(process_t *p, thread_t *t);
void process_thread_load_context(process_t *p, thread_t *t);
thread_t* process_warn_keyboard_typed(process_t *p, uint8_t key);
thread_t* process_check_unblock_keyboard(process_t *p);
void process_kill_process(process_t *p);
external_hd_file_descriptor* process_allocate_file(process_t *process);
uint32_t process_get_file_id(process_t *process, external_hd_file_descriptor *file);
external_hd_file_descriptor* process_get_file_by_id(process_t *p, uint64_t id);
void process_save_syscall_response(process_t *process, thread_t *thread, uint8_t reg, uint64_t r);
thread_t* process_fork(process_t *dest, process_t *src);
void process_free_resources(process_t *p);
void process_prepare_for_execve(process_t *p);

#endif
