#include "process.h"
#include "interrupt.h"
#include "config.h"
#include "lib.h"
#include "printk.h"
#include "vars.h"
#include "memory.h"
#include "keyboard.h"
#include "syscalls.h"

static void start_create_idle_process();

static void start_callback_loaded_console_process(uint64_t r, void *d);
static void start_load_console();

static process_t *console_process[ CONFIG_KERNEL_NUMBER_OF_WINDOWS ];
static uint8_t number_console_loaded = 0;

void start_c()
{
	kernel_physical_memory_size = read_msr_64(CONFIG_KERNEL_MSR_MEM_SIZE);
	kernel_stack_base_last_addr = get_number_of_cpus() * CONFIG_KERNEL_STACK_SIZE + kernel_binary_image_size;
	kernel_physical_memory_usable_size = kernel_physical_memory_size & 0xFFFFFFFFFFFFF000LLU;

	video_init();
	
	printk("kernel video initialized\n");
	printk("physical memory = %llu bytes\n", kernel_physical_memory_size);
	printk("physical memory usable = %llu bytes\n", kernel_physical_memory_usable_size);
	printk("kernel image size = %u bytes\n", kernel_binary_image_size);
	printk("%u cpus detected\n", get_number_of_cpus());
	printk("kernel stack limit address = %llu (0x%llX)\n", kernel_stack_base_last_addr, kernel_stack_base_last_addr);

	idt_initialize();
	printk("IDT initialized\n");

	initialize_process_table();
	printk("process table initialized\n");

	initialize_memory();
	printk("memory initialized\n");

	initialize_external_hd();
	printk("external hd initialized\n");
	
	keyboard_initialize();
	printk("keyboard initialized\n");

	initialize_syscalls();

	start_create_idle_process();
	printk("idle process created\n");
	
	start_load_console();
	
	printk("...\n");
}

static void start_callback_loaded_console_process(uint64_t r, void *d)
{
	if (!r) {
		printk("error loading console binary file!");
		panic();
	}
	printk("console %u process loaded!\n", (uint32_t)number_console_loaded);
	console_process[number_console_loaded]->video_buffer = video_get_frame_buffer(1);
	
	for (number_console_loaded=1; number_console_loaded<CONFIG_KERNEL_NUMBER_OF_WINDOWS; number_console_loaded++) {
		console_process[number_console_loaded] = process_allocate(NULL);
		if (console_process[number_console_loaded] == NULL) {
			printk("Could not allocate console process %u\n", (uint32_t)number_console_loaded);
			panic();
		}
		
		process_fork(console_process[number_console_loaded], console_process[0]);
		console_process[number_console_loaded]->video_buffer = video_get_frame_buffer(number_console_loaded+1);
		printk("console %u process loaded!\n", (uint32_t)number_console_loaded);
	}
	
	video_set_focus_window(1);
}

static process_t *initial_process[ 2 ];

static void start_callback_loaded_initial_process(uint64_t r, void *d)
{
	uint64_t id = (uint64_t)d;
	if (!r) {
		printk("error loading binary file %llu!", id);
		panic();
	}
	printk("%llu process loaded!\n", id);
	initial_process[id]->video_buffer = video_get_frame_buffer(1);

	id++;
	if (id < 5) {
		initial_process[id] = process_allocate(NULL);
		if (initial_process[id] == NULL) {
			printk("Could not allocate process %u\n", id);
			panic();
		}
		if (id == 1) {
			process_load(initial_process[id], "crafty", start_callback_loaded_initial_process, (void*)id);
		}
		else if (id == 2) {
			process_load(initial_process[id], "gzip", start_callback_loaded_initial_process, (void*)id);
		}
		else if (id == 3) {
			process_load(initial_process[id], "mcf", start_callback_loaded_initial_process, (void*)id);
		}
		else if (id == 4) {
			process_load(initial_process[id], "parser", start_callback_loaded_initial_process, (void*)id);
		}
		else {
			printk("something really bad happenned!");
			panic();
		}
	}
	else {
		video_set_focus_window(1);
	}
}

static void start_load_console()
{
//	int i;
	
//	for (i=0; i<1; i++) {
		initial_process[0] = process_allocate(NULL);
		if (initial_process[0] == NULL) {
			printk("Could not allocate process %u\n", 0);
			panic();
		}
//	}

	process_load(initial_process[0], "bzip2", start_callback_loaded_initial_process, (void*)0);
//	process_load(console_process[number_console_loaded], "t", start_callback_loaded_console_process, (void*)number_console_loaded);
}

void setup_cpu()
{
	printk("setting idt to address %llu\n", interrupt_idt_tb);
	write_msr_64(CONFIG_KERNEL_MSR_INTERRUPT_VECTOR_ADDR, (uint64_t)interrupt_idt_tb);
	interrupt_enable();
	memory_enable_paging();
	process_schedule_thread(process_idle_process[ get_cpuid() ], process_idle_thread[ get_cpuid() ]);
	printk("kernel boot complete\n");
}

static void start_create_idle_process()
{
	process_t *idle_process;
	thread_t *thread;
	uint8_t *ptr;
	uint32_t i, j;
	uint32_t jmp_disp;

	for (j=0; j<CONFIG_KERNEL_MAX_CPUS; j++) {
		idle_process = process_allocate(NULL);
		if (idle_process == NULL) {
			printk("Could not allocate IDLE process\n");
			panic();
		}

		ptr = (uint8_t*)memory_create_page(idle_process, 0, MEMORY_FLAG_EXECUTE);
		KERNEL_ASSERT(ptr != NULL);

		for (i=0; i<CONFIG_KERNEL_PAGE_SIZE; i++) {
			ptr[i] = 0x90; // nop
		}

		jmp_disp = -4000;
		i = CONFIG_KERNEL_PAGE_SIZE - 16;
		ptr[i++] = 0xE9;
		*( (uint32_t*)(ptr+i) ) = jmp_disp;

		thread = process_allocate_thread(idle_process, 0);
		KERNEL_ASSERT(thread != NULL);
		
		thread->state = THREAD_STATE_READY;
		
		strcpy(idle_process->fname, "system idle process");
		
		idle_process->heap_end = 0x0FFF;
		idle_process->static_heap_end = 0x0FFF;
		idle_process->heap_allocated = 0x0FFF;
	
		process_idle_process[j] = idle_process;
		process_idle_thread[j] = thread;
	}
}
