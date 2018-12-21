#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "config.h"
#include "config_file.h"
#include "cpu/cpu.h"
#include "cpu/rs_policy.h"
#include "cpu/branch_pred.h"
#include "io/io.h"
#include "host_code/host.h"

#ifdef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
	static volatile uint64_t system_cycle = 0;

	uint64_t advancedss_get_system_cycle()
	{
		return system_cycle;
	}

	static volatile uint32_t processors_finished_cycle = 0;
	static pthread_mutex_t mutex_processors_finished_cycle = PTHREAD_MUTEX_INITIALIZER;
	static pthread_cond_t cond_start_cycle = PTHREAD_COND_INITIALIZER;
#endif

#ifdef DEBUG_CPU
	#ifdef CONFIG_LOG_TO_FILE
		int64_t debug_console_go = 0xEFFFFFFFFFFFFFFFLL;
	#else
		int64_t debug_console_go = -1;
	#endif
#endif

#ifdef CONFIG_LOG_TO_FILE
	FILE *output_debug_fp_ = NULL;
#endif

#ifdef CPU_CHECK_EXECUTION_TRACE
	static uint64_t *possible_pc_values;
	static uint32_t possible_pc_values_size;
#endif

struct cpu_thread_t {
	cpu::phy_processor_t proc;
	pthread_t thread;
};

static cpu::processor_t **all_cpus;

static uint32_t boot_size;

static cpu_thread_t *cpus;

static uint32_t cpu_number_phy;
static uint32_t cpu_number_total;
static advancedss_config *cfg;

static uint64_t start_pc;

static pthread_mutex_t mutex_simulation_runnig = PTHREAD_MUTEX_INITIALIZER;

static volatile uint8_t simulation_runnig = 1;

static uint32_t virtual_per_physical;

static char **statistic_fnames;

/*****************************************************/

void advancedss_stop_simulation();
void load_simulator_cpu();
void setup_boot_code(char *fname);
void* cpu_thread_handler(void *param);

/*****************************************************/

#ifdef CPU_CHECK_EXECUTION_TRACE
	static int advancedss_check_correct_pc_(uint64_t vaddr, uint32_t start, uint32_t end)
	{
		uint32_t m;

		m = (start + end) >> 1;

		if (start > end)
			return 0;
		else if (vaddr == possible_pc_values[m])
			return 1;
		else if (vaddr < possible_pc_values[m])
			return advancedss_check_correct_pc_(vaddr, start, m-1);
		else
			return advancedss_check_correct_pc_(vaddr, m+1, end);
	}

	int advancedss_check_correct_pc(uint64_t vaddr)
	{
		return advancedss_check_correct_pc_(vaddr, 0, possible_pc_values_size-1);
	}
#endif

#ifdef DEBUG_CPU
	void get_param_str(char *cmd, char *dest, int n)
	{
		char *c = cmd;
		char *d = dest;
		int i = 0;

		*d = 0;

		if (n == 0)
			goto bbb;

		for (c=cmd; *c; c++) {
			if (*c == ' ') {
				if (i < (n-1)) {
					i++;
					continue;
				}

				c++;

				bbb:

				while (*c && *c != ' ') {
					*d = *c;
					d++;
					c++;
				}
				*d = 0;
				return;
			}
			else if (*c == 0)
				return;
		}
	}

	int get_param_nh(char *cmd, int n)
	{
		char cm[20];
		int a;

		get_param_str(cmd, cm, n);//DEBUG_PRINTF("====%s====",cm);
		sscanf(cm, "%X", &a);

		return a;
	}

	int get_param_n(char *cmd, int n)
	{
		char cm[20];
		int a;

		get_param_str(cmd, cm, n);//DEBUG_PRINTF("====%s====",cm);
		sscanf(cm, "%i", &a);

		return a;
	}
#endif

/*****************************************************/

uint8_t advancedss_sim_running()
{
	uint8_t d;
	pthread_mutex_lock(&mutex_simulation_runnig);
	d = simulation_runnig;
	pthread_mutex_unlock(&mutex_simulation_runnig);
	return d;
}

void advancedss_stop_simulation()
{
	pthread_mutex_lock(&mutex_simulation_runnig);
	simulation_runnig = 0;
	pthread_mutex_unlock(&mutex_simulation_runnig);
}

void* cpu_thread_handler(void *param)
{
	cpu_thread_t *cpu_thread = (cpu_thread_t*)param;
	cpu::functional_units_response_t *f_units_response;
	uint32_t i, j;
	cpu::rs_policy_t *rs_policy, *ls_queue, *io_queue;
	cpu::functional_units_t *f_units;
	cpu::reorder_buffer_t *rb;
	cpu::fetch_buffer_t *fb;
	uint8_t external_int_processed, external_interrupt_code;
	cpu::processor_t **virtual_cpus;
	uint32_t cycle_counter = 0;
	#ifdef DEBUG_CPU
		char cmd[20], cm[20];
	#endif
	
	for (i=0; i<virtual_per_physical; i++) {
		cpu_thread->proc.virtual_cpus[i]->set_pc(start_pc);
		cpu_thread->proc.virtual_cpus[i]->set_spec_pc(start_pc);
	}

	f_units = cpu_thread->proc.f_units;
	rs_policy = cpu_thread->proc.rs_policy;
	ls_queue = cpu_thread->proc.ls_queue;
	io_queue = cpu_thread->proc.io_queue;
	rb = cpu_thread->proc.rb;
	fb = cpu_thread->proc.fb;
	virtual_cpus = cpu_thread->proc.virtual_cpus;
	
	while (advancedss_sim_running()) {
		//if (advancedss_get_system_cycle() >= 10000000) exit(0);
		if (cycle_counter++ >= 5000000) {
			cycle_counter = 0;
			
			for (i=0; i<virtual_per_physical; i++) {
				virtual_cpus[i]->print_statistics(statistic_fnames[ virtual_cpus[i]->get_id() ]);
			}
		}
		
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("********************************************************\ncpu::processor_t::cycle   begin of cycle "PRINTF_UINT64_PREFIX"\n", advancedss_get_system_cycle());
		#endif

		// commit
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("**********************\ncpu::processor_t::cycle   COMMIT\n\n");
		#endif

		for (i=0; i<virtual_per_physical; i++) {
			if (virtual_cpus[i]->get_has_received_external_interrupt()) {
				//LOG_PRINTF("timer interrupt cycle %llu\n",advancedss_get_system_cycle());
				external_interrupt_code = virtual_cpus[i]->get_external_interrupt_code();
				external_int_processed = virtual_cpus[i]->process_external_interruption( external_interrupt_code );
				if (external_int_processed) {
					virtual_cpus[i]->flush_pipeline_others();
					rb->flush(virtual_cpus[i]);
				}
				else if (external_interrupt_code == IO_INTERRUPT_CODE_TIMER) { // skip timer interruptions
					external_int_processed = 1;//LOG_PRINTF("skipping timer interrupt cycle %llu\n",advancedss_get_system_cycle());
				}

				if (external_int_processed)
					virtual_cpus[i]->clear_has_received_external_interrupt();
			}
		}

		rb->commit();

		/* write-back is implicity done in the execution
		*/

		#ifdef DEBUG_CPU
			DEBUG_PRINTF("**********************\ncpu::processor_t::cycle   EXECUTION\n\n");
		#endif
		f_units_response = f_units->exec();

		// write back
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("cpu write_back\n");
		#endif
		for (i=0; i<virtual_per_physical; i++) {
			virtual_cpus[i]->write_back(f_units_response+i);
		}
		
/*	for (i=0; i<this->get_number_processors_shared(); i++) {
		for (j=0; j<r[i].n_instructions; j++) {
			this->list.del( r[i].instructions[j] );
		}
	}*/
		
		/*rs_policy->del(f_units_response);
		ls_queue->del(f_units_response);
		io_queue->del(f_units_response);*/

		// issue
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("**********************\ncpu::processor_t::cycle   ISSUE\n\n");
		#endif
		rs_policy->supply(); // issue
		ls_queue->supply(); // issue
		io_queue->supply(); // issue

		// decode
		
		fb->before_decoder();

		for (i=0; i<virtual_per_physical; i++) {
			virtual_cpus[i]->cycle();
		}
		
		fb->after_decoder();

		// fetch

		#ifdef DEBUG_CPU
			DEBUG_PRINTF("**********************\ncpu::processor_t::cycle   FETCH\n\n");
		#endif
		for (i=0; i<virtual_per_physical; i++) {
			virtual_cpus[i]->pipeline_before_fetch_inst();
		}

		fb->fetch();

		for (i=0; i<virtual_per_physical; i++) {
			virtual_cpus[i]->pipeline_after_fetch_inst();
		}

		#ifdef CPU_CACHE_LOCAL_STATISTIC
			cpu::cache_memory_struct->event_cycle();
		#endif

		#ifdef DEBUG_CPU
			//c->print_registers();
			faz:
			if (debug_console_go == -1) {
				gets(cmd);
				get_param_str(cmd, cm, 0);

				if (!strcmp(cm, "m")) {
					int start, end, i, l;
					uint8_t k;

					start = get_param_nh(cmd, 1);
					end = get_param_nh(cmd, 2);

					DEBUG_PRINTF("DISPLAY MEMORY FROM 0x%X to 0x%X\n\n", start, end);

					l = 0;
					for (i=start; i<=end; i++) {
						k = io::get_memory_obj()->read1(i);
						if (k <= 0x0F){
							DEBUG_PRINTF("0%X", k);}
						else{
							DEBUG_PRINTF("%X", k);}
						DEBUG_PRINTF("  ");
						if ((l++ % 10) == 9)
							DEBUG_PRINTF("\n");
					}
					DEBUG_PRINTF("\n\n--------------------------------------------------\n\n");

					goto faz;
				}
				else if (!strcmp(cm, "g")) {
					debug_console_go = get_param_n(cmd, 1);
				}
				else if (!strcmp(cm, "d")) {
					debug_console_go = 0x7FFFFFFF;
				}
			}
			else if (debug_console_go == (system_cycle+1))
				debug_console_go = -1;
		#endif

		#ifdef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
/*			system_cycle++;
			io::cycle();*/
			pthread_mutex_lock(&mutex_processors_finished_cycle);
			processors_finished_cycle++;
			if (processors_finished_cycle == cpu_number_phy) {
				processors_finished_cycle = 0;
				system_cycle++;
				if (cpu_number_phy > 1) {
					pthread_cond_broadcast(&cond_start_cycle);
				}
				io::cycle();
			}
			else {
				pthread_cond_wait( &cond_start_cycle, &mutex_processors_finished_cycle );
			}
			pthread_mutex_unlock(&mutex_processors_finished_cycle);
		#endif
	}

	return NULL;
}

/*****************************************************/

void setup_boot_code(char *fname)
{
	FILE *fp;
	char *buffer;
	uint32_t size;
	io::memory_t *memory = io::get_memory_obj();

	// carrega o execut�vel na mem�ria
	fp = fopen(fname, "rb");
	if (!fp) {
		LOG_PRINTF("error opening %s\n", fname);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	boot_size = size;
	rewind(fp);
	buffer = (char*) malloc( size );
	if (!buffer) {
		LOG_PRINTF("memory fault\n");
		exit(1);
	}
	fread(buffer, 1, size, fp);
	fclose(fp);

	memory->write(buffer, size, 0);

	free(buffer);
}

uint32_t advancedss_get_boot_size()
{
	return boot_size;
}

/*****************************************************/

cpu::processor_t* advancedss_get_processor_obj(uint32_t id)
{
	return ((id < cpu_number_total) ? all_cpus[id] : NULL);
}

uint32_t advancedss_get_total_number_of_cpus()
{
	return cpu_number_total;
}

uint32_t advancedss_get_number_phy_cpu()
{
	return cpu_number_phy;
}

uint32_t advancedss_get_number_virtual_cpu_per_phy_cpu()
{
	return virtual_per_physical;
}

static void load_general_config()
{
	cpu_number_phy = 1;
	virtual_per_physical = 1;
	cpu_number_total = cpu_number_phy * virtual_per_physical;
}

void load_simulator_cpu()
{
	uint32_t cpu_id, cpu_vid, i;
	cpu::rs_policy_t *rs_policy, *ls_queue, *io_queue;
	cpu::functional_units_t *f_units;
	cpu::reorder_buffer_t *rb;
	cpu::processor_t *vproc;
	cpu::mmu_t *mmu;
	cpu::branch_pred_t *bp;
	cpu::fetch_buffer_t *fb;
	advancedss_config *cfg = get_advancedss_main_config();
	char *str;

	cpu::load_config();
	cpu::cache_initialize();

	cpu_id = 0;
	cpu_vid = 0;
	cpus = (cpu_thread_t*)malloc(cpu_number_phy * sizeof(cpu_thread_t));
	if (!cpus) {
		LOG_PRINTF("load_simulator fail\n");
		exit(1);
	}
	all_cpus = (cpu::processor_t**)malloc(sizeof(cpu::processor_t*) * cpu_number_total);
	SANITY_ASSERT(all_cpus != NULL);
	
	f_units = new cpu::functional_units_t(virtual_per_physical);
	rs_policy = new cpu::rs_policy_just_one_t(f_units, virtual_per_physical, cfg->read_int32("rs_policy_just_one_max"));
	ls_queue = new cpu::rs_policy_ls_queue_simple_t(f_units, virtual_per_physical, cfg->read_int32("ls_queue_max"));
	io_queue = new cpu::rs_policy_io_queue_t(f_units, virtual_per_physical, cfg->read_int32("io_queue_max"));
	rb = new cpu::reorder_buffer_t(rs_policy->get_capacity() + ls_queue->get_capacity() + io_queue->get_capacity() + cfg->read_int32("reorder_bufferthreshold"), virtual_per_physical);

	mmu = new cpu::mmu_t(virtual_per_physical);

	str = cfg->read_str("branch_pred_type");
	if (!strcmp(str, "none")) {
		bp = new cpu::branch_pred_take_none_t(virtual_per_physical);
	}
	else if (!strcmp(str, "all")) {
		bp = new cpu::branch_pred_take_all_t(virtual_per_physical);
	}
	else if (!strcmp(str, "hist")) {
		bp = new cpu::branch_pred_hist_tb_t(virtual_per_physical);
	}
	else {
		LOG_PRINTF("branch_pred_type misconfigured\n");
		exit(1);
	}

	fb = new cpu::fetch_buffer_t(virtual_per_physical, bp);

	cpus[0].proc.id = cpu_id;
	cpus[0].proc.rs_policy = rs_policy;
	cpus[0].proc.ls_queue = ls_queue;
	cpus[0].proc.io_queue = io_queue;
	cpus[0].proc.f_units = f_units;
	cpus[0].proc.rb = rb;
	cpus[0].proc.fb = fb;
	cpus[0].proc.bp = bp;
	cpus[0].proc.memory = io::get_memory_obj();

	vproc = new cpu::processor_t(&(cpus[0].proc), cpu_vid);
	all_cpus[0] = vproc;

	f_units->set_cpu(cpu_vid, vproc);
	rs_policy->set_cpu(cpu_vid, vproc);
	ls_queue->set_cpu(cpu_vid, vproc);
	io_queue->set_cpu(cpu_vid, vproc);
	rb->set_cpu(cpu_vid, vproc);
	fb->set_cpu(cpu_vid, vproc);
	bp->set_cpu(cpu_vid, vproc);

	cpus[0].proc.virtual_cpus = (cpu::processor_t**)malloc(virtual_per_physical*sizeof(cpu::processor_t*));
	SANITY_ASSERT(cpus[0].proc.virtual_cpus != NULL);

	cpus[0].proc.virtual_cpus[0] = vproc;
	
	/*************************************/
	
	statistic_fnames = (char**)malloc(sizeof(char*) * cpu_number_total);
	SANITY_ASSERT(statistic_fnames != NULL);
	for (i=0; i<cpu_number_total; i++) {
		statistic_fnames[i] = (char*)malloc(sizeof(char)*100);
		SANITY_ASSERT(statistic_fnames[i] != NULL);
		sprintf(statistic_fnames[i], "statistic/ssa_statistic_cpu_%u.txt", i);
	}
}

void advancedss_print_status()
{
	uint32_t i;

	for (i=0; i<cpu_number_total; i++) {
		all_cpus[i]->system_halted();
	}
}

/*****************************************************/

int main(int argc, char **argv)
{
	uint32_t i;
	double time;
	#ifdef CPU_CHECK_EXECUTION_TRACE
		FILE *pc_trace;
		uint32_t pc_trace_fsize, j, min;
		char *pc_trace_buffer, *b;
		char pc_trace_line[50], pc_trace_tmp[2];
		uint64_t tmp;
	#endif

	#ifdef CONFIG_LOG_TO_FILE
		output_debug_fp_ = fopen(CONFIG_LOG_TO_FILE, "w");
		if (!output_debug_fp_) {
			printf("could not open log file %s\n", CONFIG_LOG_TO_FILE);
			exit(1);
		}
	#endif

	if (argc != 3) {
		printf("use: %s file_name_hex start_pc\n", argv[0]);
		return 1;
	}

	cfg = get_advancedss_main_config();
	cfg->open("main_config.cfg");

	load_general_config();

	#ifdef CPU_CHECK_EXECUTION_TRACE
		pc_trace = fopen("trace.txt", "r");
		if (!pc_trace) {
			printf("error openning pc trace file!\n");
			exit(1);
		}
		fseek(pc_trace, 0, SEEK_END);
		pc_trace_fsize = ftell(pc_trace);
		rewind(pc_trace);
		pc_trace_buffer = (char*)malloc(pc_trace_fsize+2);
		if (!pc_trace_buffer) {
			printf("error malloc main\n");
			exit(1);
		}
		fread(pc_trace_buffer, 1, pc_trace_fsize, pc_trace);
		fclose(pc_trace);

		pc_trace_buffer[pc_trace_fsize++] = 10;
		pc_trace_buffer[pc_trace_fsize] = 0;

		possible_pc_values_size = 0;
		for (b=pc_trace_buffer; *b; b++) {
			if (*b == 10) {
				possible_pc_values_size++;
			}
		}

		possible_pc_values = (uint64_t*)malloc(possible_pc_values_size * sizeof(uint64_t));
		if (!possible_pc_values) {
			printf("error malloc main 2\n");
			exit(1);
		}

		*pc_trace_line = 0;
		pc_trace_tmp[1] = 0;
		i = 0;
		b = pc_trace_buffer;
		while (*b && !((*b >= '0' && *b <= '9') || (*b >= 'a' && *b <= 'f') || (*b >= 'A' && *b <= 'F')))
			b++;
		while (*b) {
			if ((*b >= '0' && *b <= '9') || (*b >= 'a' && *b <= 'f') || (*b >= 'A' && *b <= 'F')) {
				*pc_trace_tmp = *b;
				strcat(pc_trace_line, pc_trace_tmp);
				b++;
			}
			else {
				if (i >= possible_pc_values_size) {
					printf("something is wrong with the trace file! i=%u\n", i);
					exit(1);
				}
				else {
					uint64_t value;

					sscanf(pc_trace_line, "%llX", &value);

					//printf("rastro 0x%llX\n", value);
					possible_pc_values[i] = value;

					i++;
					*pc_trace_line = 0;

					while (*b && !((*b >= '0' && *b <= '9') || (*b >= 'a' && *b <= 'f') || (*b >= 'A' && *b <= 'F')))
						b++;
				}
			}
		}

//		printf("rastro 0x%llX\n", possible_pc_values[i-1]);exit(0);

		possible_pc_values_size = i;

		// order the array, so we can use binary search to look for positions
		// selection sort sucks... i know... but i'm in a hurry here

		for (i=0; i<possible_pc_values_size-1; i++) {
			min = i;
			for (j=i+1; j<possible_pc_values_size; j++) {
				if (possible_pc_values[j] < possible_pc_values[min])
					min = j;
			}

			tmp = possible_pc_values[min];
			possible_pc_values[min] = possible_pc_values[i];
			possible_pc_values[i] = tmp;
		}
	#endif

	// io must be loaded before cpu
	io::advancedss_io_initialize();

	setup_boot_code(argv[1]);
	sscanf(argv[2], "%X", &start_pc);

	load_simulator_cpu();

	host_sim_initialize();

	for (i=0; i<cpu_number_phy; i++) {
		pthread_create(&cpus[i].thread, NULL, cpu_thread_handler, (void*)(cpus+i));
	}

	for (i=0; i<cpu_number_phy; i++) {
		pthread_join(cpus[i].thread, NULL);
	}

	LOG_PRINTF("/*");

	for (i=0; i<cpu_number_total; i++) {
		all_cpus[i]->system_halted();
	}

	time = host_sim_get_running_time();
	LOG_PRINTF("Simulation stopped at cycle "PRINTF_UINT64_PREFIX" due to halt instruction (time = %f seconds... %f cycles/sec)\n", advancedss_get_system_cycle(), time, (double)advancedss_get_system_cycle() / time);
	LOG_PRINTF("*/");

	return 0;
}
