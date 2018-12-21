/*
    ADVANCEDSS (Advanced Superscalar Simulator).

    Copyright (C) 2009  Eduardo Henrique Molina da Cruz

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


    **************************************************************************************

    ADVANCEDSS (Advanced Superscalar Simulator)

    Coded by Eduardo Henrique Molina da Cruz
    email: eduardohmdacruz@gmail.com
    MSN: eduardohmcruz@hotmail.com

    Oriented by Ronaldo Augusto de Lara Gonçalves
    Doctor in Computer Science
    State University of Maringa
    Parana - Brazil

    January 2009
*/

#ifndef _ADVANCEDSS_CPU_HEADER_
#define _ADVANCEDSS_CPU_HEADER_

#include <setjmp.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../data/double_linked_static.h"
#include "../config.h"
#include "../config_file.h"
#include "../main.h"

namespace io {
	class memory_t;
};

#include "arch_out_of_namespace.h"

namespace cpu {
	class processor_t;
	class decoder_t;
	class rs_policy_t;
	class rs_instruction_t;

	#include "arch_general_start.h"

	typedef data_dblinked_static_list<rs_instruction_t, uint64_t> rs_instruction_static_list_t;
	typedef data_dblinked_static_el<rs_instruction_t, uint64_t> rs_instruction_static_list_el_t;

	void load_config();

	#ifdef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
		#define CPU_GET_CYCLE(CPU)           advancedss_get_system_cycle()
	#else
		#define CPU_GET_CYCLE(CPU)           CPU->get_ncycle()
	#endif

	/* Instruction types.
	   maximum is 32 types
	*/
	enum {
		CPU_INSTRUCTION_TYPE_NONE, // must be the first one
		CPU_INSTRUCTION_TYPE_INT_ARITH,
		CPU_INSTRUCTION_TYPE_INT_LOGIC,
		CPU_INSTRUCTION_TYPE_INT_MUL,
		CPU_INSTRUCTION_TYPE_INT_DIV,
		CPU_INSTRUCTION_TYPE_MOV,
		CPU_INSTRUCTION_TYPE_LOAD,
		CPU_INSTRUCTION_TYPE_STORE,
		CPU_INSTRUCTION_TYPE_BRANCH,
		CPU_INSTRUCTION_TYPE_FPU_ARITH,
		CPU_INSTRUCTION_TYPE_FPU_MUL,
		CPU_INSTRUCTION_TYPE_FPU_DIV,
		CPU_INSTRUCTION_TYPE_IO_IN,
		CPU_INSTRUCTION_TYPE_IO_OUT,
		CPU_INSTRUCTION_TYPE_NUMBER // must be the last one
	};
	/*#define CPU_INSTRUCTION_TYPE_INT_ARITH          0
	#define CPU_INSTRUCTION_TYPE_INT_LOGIC          1
	#define CPU_INSTRUCTION_TYPE_INT_MUL            2
	#define CPU_INSTRUCTION_TYPE_INT_DIV            3
	#define CPU_INSTRUCTION_TYPE_MOV                4
	#define CPU_INSTRUCTION_TYPE_LOAD               5
	#define CPU_INSTRUCTION_TYPE_STORE              6
	#define CPU_INSTRUCTION_TYPE_BRANCH             7
	#define CPU_INSTRUCTION_TYPE_FPU_ARITH          8
	#define CPU_INSTRUCTION_TYPE_FPU_MUL            9
	#define CPU_INSTRUCTION_TYPE_FPU_DIV            10
	#define CPU_INSTRUCTION_TYPE_IO_IN              11
	#define CPU_INSTRUCTION_TYPE_IO_OUT             12*/

	#define CPU_INST_TYPE_MASK(T)                   (1 << (T))
	
	enum {
		CPU_BRANCH_TYPE_CONDITIONAL,
		CPU_BRANCH_TYPE_UNCONDITIONAL,
		CPU_BRANCH_TYPE_RAS_PUSH,
		CPU_BRANCH_TYPE_RAS_POP
	};

	char* get_inst_type_str(uint32_t type);

	#ifdef DEBUG_CPU
		void print_instruction_type(uint32_t type);
		char* get_instruction_status_str(uint32_t status);
	#endif

	#define CPU_INSTRUCTION_STATUS_DECODING        0x00000000
	#define CPU_INSTRUCTION_STATUS_WAIT_DEP        0x00000001
	#define CPU_INSTRUCTION_STATUS_DEP_READY       0x00000002
	#define CPU_INSTRUCTION_STATUS_EXECUTING       0x00000003
	#define CPU_INSTRUCTION_STATUS_READY           0x00000004

	#define CPU_EXCEPTION_INT_OVERFLOW             0

	#define CPU_RESULT_TYPE_NONE            0xFFFFFFFF
	#define CPU_RESULT_TYPE_GPR             0
	#define CPU_RESULT_TYPE_BRANCH          1

	/* class instruction_t contains the "decoded" instruction
	   information such as type, dependency list, functional unit, cycles to execute, etc...
	   all information needed to simulate the pipeline
	*/

	enum rs_instruction_pipeline_state_t {
		RS_INSTRUCTION_PIPELINE_VALID,
		RS_INSTRUCTION_PIPELINE_INVALID,
		RS_INSTRUCTION_PIPELINE_VALID_CAN_FLUSH_PIPELINE_IN_WRITEBACK
	};

	enum rs_instruction_microcode_state_t {
		RS_INSTRUCTION_MICROCODE_FIRST,
		RS_INSTRUCTION_MICROCODE_MIDLE,
		RS_INSTRUCTION_MICROCODE_LAST
	};

	class rs_instruction_t
	{
		protected:
			/* the processor
			   needed in a SMT processor to distinguish between different virtual processors
			*/
			processor_t *my_cpu;

			/* id of the instruction
			*/
			uint64_t id;

			/* Number of the cycle that the instruction status become ready
			   It is used to simulate the cycle delay of the write-back stage
			*/
//			uint64_t cycle_ready;

			/* Vector that indexes to the instructions that will generate the results that I need
			   dep_in_n contain the number of positions actualy used in the vector
			*/
			rs_instruction_t *dep_in[CPU_INSTRUCTION_MAX_DEP_IN];
			uint8_t dep_in_n;
			uint8_t dep_in_resolved; // número de dependências já resolvidas
			uint32_t dep_in_index[CPU_INSTRUCTION_MAX_DEP_IN];
			
		public:
			inline uint8_t get_dep_in_n() {
				return this->dep_in_n;
			}

			inline uint8_t get_dep_in_resolved() {
				return this->dep_in_resolved;
			}
		
		protected:

			/* Vector that indexes to the instructions that depends on the results that I'll generate
			   dep_out_n contain the number of positions actualy used in the vector
			   The memory is allocated when the program starts because this values equals to the maximum number of the
			   instructions in the pipeline
			*/
			rs_instruction_t **dep_out;
			uint32_t dep_out_n;
			uint32_t dep_out_max;
			uint32_t *dep_out_index;

			/*
				results that the instruction generates
				set by decoder
				used by the writeback
			*/
			rs_instruction_t **out_deps_regs[CPU_INSTRUCTION_MAX_DEP_OUT];
			uint8_t out_deps_regs_n; // number of used slots in the above vector

			/* type of the instruction
			*/
			uint32_t type;

			/* status of instruction: waiting dependency, dependencies ready, eecuting, ...
			*/
			uint32_t status;

			uint32_t cycles_to_exec;

			#ifdef DEBUG_CPU
				char unmounted_inst[100];
			#endif

			uint8_t is_branch;
			uint8_t branch_taken;
			uint8_t branch_foresight; // prevision correct or not
			target_addr_t branch_target;
			uint32_t branch_type;

			/* If the instruction generated an exception, then exception=1 and exception_code
			   contains the identifier
			*/
			uint8_t exception;
			exception_code_t exception_code;

			/* If the instruction is a trap, then is_trap=1 and trap_code
			   contains the identifier
			*/
			uint8_t is_trap;
			trap_code_t trap_code;

			target_addr_t vaddr; // virtual address of the instruction
			uint8_t length; // length of the instruction in bytes

			uint8_t memory_result;
			target_addr_t memory_operand_addr;
			uint8_t memory_operand_length;

			/* if is a halt instruction */
			uint8_t is_halt;

			uint8_t inst_must_flush_pipeline;

			rs_policy_t *rs_policy;
			rs_instruction_static_list_el_t *rs_policy_list_el, *rb_list_el;

			rs_instruction_pipeline_state_t pipeline_state;

			rs_instruction_microcode_state_t microcode_state;

			rs_instruction_t *first_microcode, *last_microcode;

		public:
			inline rs_instruction_pipeline_state_t get_pipeline_state() {
				return this->pipeline_state;
			}

			inline void set_pipeline_state(rs_instruction_pipeline_state_t state) {
				this->pipeline_state = state;
			}

			inline rs_instruction_microcode_state_t get_microcode_state() {
				return this->microcode_state;
			}

			inline void set_microcode_state(rs_instruction_microcode_state_t state) {
				this->microcode_state = state;
			}

			inline rs_instruction_t* get_first_microcode() {
				return this->first_microcode;
			}

			inline void set_first_microcode(rs_instruction_t *first_microcode) {
				this->first_microcode = first_microcode;
			}

			inline rs_instruction_t* get_last_microcode() {
				return this->last_microcode;
			}

			inline void set_last_microcode(rs_instruction_t *last_microcode) {
				this->last_microcode = last_microcode;
			}

			void reset(processor_t *c, uint64_t id);

			inline void set_rs_policy(rs_policy_t *rs_policy) {
				this->rs_policy = rs_policy;
			}

			inline rs_policy_t* get_rs_policy() {
				return this->rs_policy;
			}

			inline void set_rs_policy_list_el(rs_instruction_static_list_el_t *rs_policy_list_el) {
				this->rs_policy_list_el = rs_policy_list_el;
			}

			inline rs_instruction_static_list_el_t* get_rs_policy_list_el() {
				return this->rs_policy_list_el;
			}

			inline void set_rb_list_el(rs_instruction_static_list_el_t *rb_list_el) {
				this->rb_list_el = rb_list_el;
			}

			inline rs_instruction_static_list_el_t* get_rb_list_el() {
				return this->rb_list_el;
			}

			inline void set_as_must_flush_pipeline() {
				this->inst_must_flush_pipeline = 1;
			}

			inline uint8_t must_flush_pipeline() {
				return this->inst_must_flush_pipeline;
			}

			inline void set_as_halt() {
				this->is_halt = 1;
			}

			inline uint8_t get_is_halt() {
				return this->is_halt;
			}

			inline void set_memory_result(uint8_t type, target_addr_t memory_operand_addr, uint8_t memory_operand_length) {
				this->memory_result = type;
				this->memory_operand_addr = memory_operand_addr;
				this->memory_operand_length = memory_operand_length;
			}

			inline uint8_t get_memory_result() {
				return this->memory_result;
			}
			
			inline target_addr_t get_memory_operand_addr() {
				return this->memory_operand_addr;
			}
			
			inline uint8_t get_memory_operand_length() {
				return this->memory_operand_length;
			}

			inline void add_out_deps_regs(rs_instruction_t **r) {
				#ifdef SANITY_CHECK
					SANITY_ASSERT(this->out_deps_regs_n < CPU_INSTRUCTION_MAX_DEP_OUT);
				#endif
				this->out_deps_regs[ this->out_deps_regs_n++ ] = r;
			}

			inline uint8_t branch_mis_predicted() {
				return !this->branch_foresight;
			}

			inline void setup_for_dispatch(target_addr_t vaddr,
			                               uint8_t len
			                               ) {
				this->vaddr = vaddr;
				this->length = len;
			}

			inline void set_branch(uint8_t branch_taken,
			                       target_addr_t branch_target,
			                       uint32_t branch_type
			                       ) {
				this->is_branch = 1;
				this->branch_taken = branch_taken;
				this->branch_target = branch_target;
				this->branch_type = branch_type;
			}

			inline void set_branch_foresight(uint8_t branch_foresight) {
				this->branch_foresight = branch_foresight;
			}

			inline target_addr_t get_branch_target() {
				return this->branch_target;
			}

			inline uint8_t get_branch_taken() {
				return this->branch_taken;
			}

			inline uint8_t get_length() {
				return this->length;
			}

			inline void set_trap(trap_code_t c) {
				this->is_trap = 1;
				this->trap_code = c;
			}

			inline uint8_t get_is_trap() {
				return this->is_trap;
			}

			inline trap_code_t get_trap_code() {
				return this->trap_code;
			}

			inline uint8_t get_is_branch() {
				return this->is_branch;
			}

			static void create_dep(rs_instruction_t *dest, rs_instruction_t *src);

			inline uint64_t get_vaddr() {
				return this->vaddr;
			}

			inline uint8_t has_exception() {
				return this->exception;
			}

			inline exception_code_t get_exception_code() {
				return this->exception_code;
			}

			inline void set_exception(exception_code_t c) {
				this->exception = 1;
				this->exception_code = c;
			}

			#ifdef DEBUG_CPU
				inline char* get_unmounted_inst() {
					return this->unmounted_inst;
				}

				void print_dependencies();
			#endif

			inline uint64_t get_id() {
				return this->id;
			}

			inline void set_id(uint64_t id) {
				this->id = id;
			}

			void write_back();

			void set_dep_out_buffer(uint32_t dep_out_max);

			inline uint32_t get_cycles_to_exec() {
				return this->cycles_to_exec;
			}

			inline void set_cycles_to_exec(uint32_t cycles) {
//				if (cycles > 500){DEBUG_PRINTF("ohaaaaaaaaaa %u\n",cycles);}
				this->cycles_to_exec = cycles;
			}

			inline void set_cpu(processor_t *c) {
				this->my_cpu = c;
			}

			inline processor_t* get_cpu() {
				return this->my_cpu;
			}

			inline void set_status(uint32_t s) {
				this->status = s;
			}

			inline uint32_t get_status() {
				return this->status;
			}

			inline uint8_t are_all_dep_resolved() {
				return (this->dep_in_resolved == this->dep_in_n);
			}

	/*		inline void set_cycle_ready(uint64_t c) {
				this->cycle_ready = c;
			}

			inline uint64_t get_cycle_ready() {
				return this->cycle_ready;
			}*/

			inline uint32_t get_type() {
				return this->type;
			}

			inline void set_type(uint32_t type) {
				this->type = type;
			}

			void flush_on_write_back();
	};

	struct fetch_buffer_el_t {
		target_addr_t vaddr;
		uint8_t is_branch;
		uint8_t data;
		target_addr_t next_inst;
		uint8_t is_exception;
		exception_code_t exception_code;
		uint32_t paddr;
	};

	#define CPU_DECODER_STATUS_IDLE              0x00
	#define CPU_DECODER_STATUS_BUSY              0x01

	class fetch_buffer_t;

	extern uint32_t cfg_max_bytes_decoded_per_cycle;

	class mmu_t;

	/* decodes (and executes) instructions
	*/

	class decoder_t
	{
		protected:
			processor_t *my_cpu; // the procesor i belong

			fetch_buffer_t *fb; // fetch buffer

//			mmu_t *mmu;

			uint64_t inst_id; // next instrution id

			/* disables/enables execution
			   when the execution is disabled, just the dependency checking is done
			   this is used to simulate branch mispredition and interruptions
			*/
			uint8_t mis_speculation;

			/* The structure where the decoded instructions will be put in
			   before the dispatch
			*/
			rs_instruction_t *buffered_inst[CPU_DECODER_MAX_ALLOCATED_INST];
			uint8_t allocated_rs_slots, required_rs_slots, dispatched_rs_slots;

			uint8_t status; // idle or busy

			/*
				The number of bytes already decoded in current cycle
			*/
			uint32_t bytes_decoded_in_cycle;

			void (decoder_t::*handler)(); // handler for when status != idle

			void (decoder_t::*handler_alocate_inst)(); // handler backup for when inside alocate_inst_

			uint8_t inst_opcodes[CPU_DECODER_MAX_OPCODE_BYTES];
			uint8_t inst_opcodes_i; // index the above array

			target_addr_t inst_addr; // virtual address of the instruction, set by arch
			uint8_t inst_length; // length of the instruction in bytes, set by arch

			/*
				virtual address of the next instruction, got fgrom fetch buffer
				used to check branch misprediction
			*/
			target_addr_t next_inst_addr;

			uint8_t is_branch;
			uint8_t branch_taken;
			uint8_t branch_foresight; // prevision correct or not
			target_addr_t branch_target;
			uint32_t branch_type;
			uint8_t branch_automatic_detect;

			/* If the instruction generated an exception, then exception=1 and exception_code
			   contains the identifier
			*/
			uint8_t exception;
			exception_code_t exception_code;

			exception_code_t buffer_exception_code;

			/* If the instruction is a trap, then is_trap=1 and trap_code
			   contains the identifier
			*/
			uint8_t is_trap;
			trap_code_t trap_code;

			uint8_t stop_decoding_in_cycle;

			/* if is a halt instruction */
			uint8_t is_halt;

			uint8_t inst_must_flush_pipeline;

			uint8_t inst_unexpected_behavior;

			uint8_t check_if_branch_instruction[CPU_INSTRUCTION_TYPE_NUMBER];

		public:
			decoder_t(processor_t *my_cpu);
			#ifdef CPU_CHECK_EXECUTION_TRACE
				void decode(volatile uint64_t v);
			#else
				void decode();
			#endif

			void initialize_decoder_arch(); // defined by the archtecture

			void alocate_inst_();
			void alocate_inst(uint32_t n);
			void dispatch();
			void flush();
			uint8_t get_buffered_rs(rs_instruction_t **s); // return the number of buffered instruction (0...2)
			void initialize_instruction(); // defined by the archtecture

			void fetch_buffer_exception();

			inline void new_cycle() {
				this->bytes_decoded_in_cycle = 0;
				this->stop_decoding_in_cycle = 0;
			}

			inline uint8_t must_stop_decoding_in_cycle() {
				return this->stop_decoding_in_cycle;
			}

			inline uint8_t get_mis_speculation() {
				return this->mis_speculation;
			}

			#include "arch_class_decoder.h"
	};

	#define CPU_FUNCTIONAL_UNIT_STATE_IDLE         0x00
	#define CPU_FUNCTIONAL_UNIT_STATE_BUSY         0x01

	class cpu_shared_component_t
	{
		private:
			processor_t **my_cpus;
			uint32_t number_processors_shared;

		public:
			cpu_shared_component_t(uint32_t number_processors_shared);

			inline void set_cpu(uint32_t i, processor_t *cpu) {
				this->my_cpus[i] = cpu;
			}

			inline uint32_t get_number_processors_shared() {
				return this->number_processors_shared;
			}

			inline processor_t* get_cpu(uint32_t i) {
				return this->my_cpus[i];
			}
	};

	struct functional_units_response_t {
		uint32_t vid; // processors virtual id
		rs_instruction_t **instructions;
		uint32_t n_instructions;
	};

	/* Controlls the execution of the functional units
	   Can be shared among virtual processors
	*/
	class functional_units_t: public cpu_shared_component_t
	{
		struct func_unit_t {
			rs_instruction_t *inst; // instruction being executed. if NULL, the functional unit is idle
			// inst_type: bitmask, perform an AND operation with this field and the type of desired instruction to
			//            check if the functional unit can execute it
			uint32_t inst_type;
			uint32_t cycles; // number of cycles alread spent wih actual instruction
		};

		protected:
			func_unit_t *units;
			func_unit_t **units_access[32];
			uint32_t n_units; // number of functinal units
			uint32_t n_units_ls;
			functional_units_response_t *response;

		public:
			functional_units_t(uint32_t number_processors_shared);

			inline uint32_t get_n_units() {
				return this->n_units;
			}

			inline uint32_t get_n_units_ls() {
				return this->n_units_ls;
			}

			inline uint32_t get_funct_unit_type(uint32_t n) {
				return this->units[n].inst_type;
			}

			inline uint8_t is_func_unit_idle(uint32_t n) {
				return (this->units[n].inst == NULL);
			}

			functional_units_response_t* exec();
			void flush(processor_t *c);
			void flush_on_write_back(rs_instruction_t *s);

			uint8_t issue(rs_instruction_t *inst);
	};

	/* Controlls the topology of the reservation station queues,
	   as well perform the issue to the functional units
	   Can be shared among virtual processors

	   rs_policy_t specify the abstract class
	   the implentations are in rs_policy.h and rs_policy.cpp
	*/
	class rs_policy_t: public cpu_shared_component_t
	{
		protected:
			functional_units_t *f_units;

		public:
			rs_policy_t(functional_units_t *f_units, uint32_t number_processors_shared);

			virtual uint8_t add(rs_instruction_t *s) = 0;
			virtual uint8_t del(rs_instruction_t *s) = 0;
			virtual void supply() = 0;
			virtual void flush(processor_t *c) = 0;
			virtual uint32_t get_capacity() = 0;
			virtual void arrived_to_commit(rs_instruction_t *s) = 0; // called when an instruction arrived at the end of the reorder buffer
			virtual void flush_on_write_back(rs_instruction_t *s) = 0;
	};

	// add here the names of all the classes specified in rs_policy.h
	class rs_policy_just_one_t;

	class reorder_buffer_t: public cpu_shared_component_t
	{
		protected:
			/* List of unused rs_instruction_t
			   Avids mallocs, making simulation much faster
			*/
			rs_instruction_static_list_t list_inst_supplier;

			/* The instruction list of the rerder buffer, in the dispatch order
			*/
			rs_instruction_static_list_t list;

			uint8_t *must_flush;

		public:
			reorder_buffer_t(uint32_t size, uint32_t number_processors_shared);
			rs_instruction_t* get_inst_from_supply();
			void add(rs_instruction_t *s);

			void commit();
			void flush(processor_t *c);

			inline uint32_t get_size() {
				return this->list.get_capacity();
			}

			void flush_on_write_back(rs_instruction_t *s);
	};

	class branch_pred_t;

	class fetch_buffer_t: public cpu_shared_component_t
	{
		protected:
			branch_pred_t *bp;
			fetch_buffer_el_t **buffer;
			uint32_t size, *read_pos, *write_pos, *available, *available_before, *last_branch;
			uint32_t width;
			uint8_t *disable_until_flush;
			uint32_t *disable_due_to_cache_miss;

		public:	
			fetch_buffer_t(uint32_t number_processors_shared, branch_pred_t *bp);

			inline uint8_t has_bytes(uint32_t n, uint32_t vid) {
				return ((this->size - this->available[vid]) >= n);
			}

			inline uint32_t bytes_in_buffer(uint32_t vid) {
				return (this->size - this->available[vid]);
			}

			/* try to skip n bytes from buffer
			   returns the number of bytes actually skipped
			*/
			uint32_t skip_bytes(fetch_buffer_el_t *r, uint32_t n, uint32_t vid);
			uint32_t skip_bytes(target_addr_t *vaddr, uint32_t n, uint32_t vid);

			void get_byte(fetch_buffer_el_t *r, processor_t *c);
			void fetch();
			void fetch(uint32_t vid);
			void flush(processor_t *c);
			void before_decoder();
			void after_decoder();
	};

	/* MMU, handles virtual memory
	*/
	class mmu_t
	{
		protected:
			uint32_t number_processors_shared; // am i shared between virtual processors?
			pthread_mutex_t mutex; // controlls access when shared

		public:
			mmu_t(uint32_t number_processors_shared);
	};

	struct branch_response_t {
		uint8_t is_branch;
		uint8_t take; // must take or not the branch
		target_addr_t target; // branch target address
	};

	struct branch_el_t {
		target_addr_t mem_addr; // memory address of the branch
		target_addr_t target_addr; // branch target address
		uint8_t taken; // was the branch taken?
		uint8_t is_call;
		uint8_t is_ret;
	};

	/* Branch prediction abstract class
	*/
	class branch_pred_t: public cpu_shared_component_t
	{
		protected:

		public:
			uint64_t statistic_hash_hit, statistic_hash_miss;
		
			branch_pred_t(uint32_t number_processors_shared);

			// warn about a new branch
			virtual void add_entry(branch_el_t *branch, processor_t *proc) = 0;

			// check if some address is a branch, and if it is, warn about what path to take
			virtual void check (target_addr_t vaddr, branch_response_t *r, processor_t *proc) = 0;

			// clear the table
			virtual void flush(processor_t *proc) = 0;
	};

	// list here all the classes of branch_pred.h
	class branch_pred_take_none_t;

	struct cache_pre_slot_t {
		uint64_t cycle_ready;
	};

	struct cache_response_t {
		uint8_t hit; // if found or not
		uint32_t blocks; // number of blocks involved
		uint32_t latency;
	};
	
	#define CPU_CACHE_READ 0
	#define CPU_CACHE_WRITE 1

	class cache_memory_t
	{
		protected:
			uint32_t latency_cycles, id;

		public:
			uint64_t statistic_hits, statistic_misses, statistic_copy_back;
			
			#ifdef CPU_CACHE_LOCAL_STATISTIC
				uint64_t statistic_local_hits, statistic_local_misses;
			#endif

			cache_memory_t(uint32_t latency_cycles, uint32_t id);

			virtual void access(phy_addr_t address, uint8_t length, cache_response_t *response, uint8_t write) = 0;
			virtual uint32_t insert(phy_addr_t address, uint8_t length, uint32_t time_to_be_ready) = 0;
			
			inline uint32_t get_latency_cycles() {
				return this->latency_cycles;
			}
			
			inline uint32_t get_id() {
				return this->id;
			}
	};

	class cache_memory_struct_t
	{
		protected:
			cache_memory_t **caches; // points to the caches
			cache_memory_t **links; // vector that contains witch cache must be access if one fails to hit
			uint32_t memory_penalty;

			#ifdef CPU_CACHE_LOCAL_STATISTIC
				uint64_t statistic_local_hits[3][CPU_CACHE_LOCAL_STATISTIC];
				uint64_t statistic_local_misses[3][CPU_CACHE_LOCAL_STATISTIC];
				uint32_t statistic_local_i, statistic_local_n;
			#endif
			
		public:
			
			#ifdef CPU_CACHE_LOCAL_STATISTIC			
				void event_contex_switch(processor_t *c);
				void event_cycle();
			#endif
			
			uint64_t statistic_memory_latency_medium, statistic_memory_latency_medium_i;

			cache_memory_struct_t();

			uint32_t access(uint32_t cache_id, phy_addr_t address, uint8_t length, uint8_t write);
			
			inline cache_memory_t* get_cache(uint32_t id) {
				return this->caches[id];
			}
			
			uint32_t copy_back(cache_memory_t *cache, uint32_t addr, uint32_t len, uint32_t time_to_be_ready);
	};
	
	extern cache_memory_struct_t *cache_memory_struct;
	
	void cache_initialize();

	// most of the statistics where copied from simplescalar
	// to facilitate the use o advancedss
	struct cpu_statistics {
		uint64_t committed_uops[32]; // one counter for each type
		uint64_t dispatched_uops[32]; // one counter for each type
		uint64_t executed_uops[32]; // one counter for each type
		uint64_t fetch_queue_cumulative_occupancy;
		uint64_t fetch_width_used_cumulative;
		uint64_t decode_width_used_cumulative;
		uint64_t rs_queue_cumulative_occupancy;
		uint64_t ls_queue_cumulative_occupancy;
		uint64_t io_queue_cumulative_occupancy;
		uint64_t rb_cumulative_occupancy;
		uint64_t exec_units_cumulative_occupancy;
		uint64_t pipeline_flushes_on_commit;
		uint64_t pipeline_flushes_on_wb;
		uint64_t branch_pred_total;
		uint64_t branch_pred_correct;
		uint64_t branch_missing;
		uint64_t average_inst_size, average_inst_size_i;
		uint64_t average_bytes_between_branchs, average_bytes_between_branchs_i;
		uint64_t average_uops_per_inst, average_uops_per_inst_i;
		uint64_t dispatched_inst;
		uint64_t commited_inst;

		/*
			sum ( (occupied_1 / size) + (occupied_2 / size) + ... (occupied_n / size) ) / sum_n
			= sum ( occupied_1 + occupied_2 + ... occupied_n ) / (size * sum_n)
		*/
	};

	void clear_cpu_statistics(cpu_statistics *s);

	struct phy_processor_t {
		uint32_t id;

		/* my reservation station queue/queues
		   shared among virtual processors
		*/
		rs_policy_t *rs_policy, *ls_queue, *io_queue;

		/* my functional units
		   shared among virtual processors
		*/
		functional_units_t *f_units;

		/* my reorder buffer
		   shared among virtual processors
		*/
		reorder_buffer_t *rb;

		/* my fetch buffer
		   shared among virtual processors
		*/
		fetch_buffer_t *fb;

		branch_pred_t *bp;

		io::memory_t *memory;

		uint32_t cache_l1_data, cache_l1_inst;

		processor_t **virtual_cpus;
	};

	/* The out-of-order simulation is done in a simpler way: the instruction is
	   executed by the decoder, and the instruction just go throw all the steps
	   of the pipeline simulating just the time and depedency.
	   This makes my life easier, and the final resut is the same... except when interrupts
	   occur. But an interrupt happens in a low frequency, so this mechanism won't
	   produce wrong results.
	*/

	class processor_t
	{
		protected:
			struct dependency_vector_t {
				rs_instruction_t **deps;
				uint32_t number;
			};

			dependency_vector_t *dep_vector;
			uint32_t dep_vector_number;

			/* spec_pc: updated by the fetch
			*/
			target_addr_t spec_pc;

			/* my decoder
			*/
			decoder_t *decoder;

			/* id of cpu to be used in a multi-processor environment
			   virtual processors in a physical processor uses different id too
			*/
			uint32_t id;

			/* ID inside of the virtual processor inside a physical processor
			*/
			uint32_t virtual_id;

			/* my reservation station queue/queues
			   can be shared among virtual processors
			*/
			rs_policy_t *rs_policy, *ls_queue, *io_queue;

			/* my functional units
			   can be shared among virtual processors
			*/
			functional_units_t *f_units;

			/* my reorder buffer
			   can be shared among virtual processors
			*/
			reorder_buffer_t *rb;

			/* my fetch buffer
			*/
			fetch_buffer_t *fb;

			//mmu_t *mmu;

			io::memory_t *memory;

			branch_pred_t *bp;

			/*
				1 if i am locking the memory
				otherwise 0
			*/
			uint8_t i_locked_memory;

			uint8_t has_received_external_interrupt;
			uint8_t external_interrupt_code;

			/* when cpu's are assyncronous, each one has your own cycle counter
			*/
			#ifndef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
				uint64_t ncycle;
			#endif

			#ifdef CPU_DETECT_PIPELINE_DEADLOCK_CYCLES
				uint32_t number_locked_cycles;
			#endif

			uint32_t num_of_branches[CPU_INSTRUCTION_TYPE_NUMBER];
			rs_instruction_t *wb_flusher;

		public:
			processor_t(phy_processor_t *phy_cpu, uint32_t virtual_id);

			void initialize_arch(uint32_t boot_size); // defined by the archtecture
			void process_exception(exception_code_t c, target_addr_t inst_addr, uint8_t inst_length); // defined by the archtecture
			void process_trap(trap_code_t c, target_addr_t inst_addr, uint8_t inst_length); // defined by the archtecture
			void process_instruction_that_flushes_pipeline(); // defined by the archtecture
			void pipeline_before_decode_inst(); // defined by the archtecture
			void pipeline_after_decode_inst(); // defined by the archtecture
			void pipeline_before_fetch_inst(); // defined by the archtecture
			void pipeline_after_fetch_inst(); // defined by the archtecture

			 // defined by the archtecture
			 // return 1 if external interrupt processed, otherwise 0
			uint8_t process_external_interruption(uint8_t c);

			// defined by the archtecture
			uint32_t memory_read_inst_byte(target_addr_t vaddr, uint8_t& has_exception, exception_code_t& exc);

			// return 1 if the cpu has not an external interrupt pending
			inline uint8_t set_external_interrupt(uint8_t code) {
				uint8_t r;
				if (this->has_received_external_interrupt == 1)
					r = 0;
				else {
					this->has_received_external_interrupt = 1;
					this->external_interrupt_code = code;
					r = 1;
				}
				return r;
			}

			inline uint8_t get_has_received_external_interrupt() {
				return this->has_received_external_interrupt;
			}

			inline void clear_has_received_external_interrupt() {
				this->has_received_external_interrupt = 0;
			}

			inline uint8_t get_external_interrupt_code() {
				return this->external_interrupt_code;
			}

			void set_number_dep_vectors(uint32_t n);
			void setup_dep_vector(uint32_t vector_index, uint32_t size);

			void io_space_read(void *data, uint8_t size, uint32_t port);
			void io_space_write(void *data, uint8_t size, uint32_t port);

			void memory_lock();
			void memory_unlock();

			void memory_check_lock();
			void memory_check_unlock();

			uint8_t memory_read_1_bytes(uint32_t addr);
			uint16_t memory_read_2_bytes(uint32_t addr);
			uint32_t memory_read_4_bytes(uint32_t addr);
			uint64_t memory_read_8_bytes(uint32_t addr);
			void memory_read_n_bytes(uint32_t addr, void *data, uint32_t len);

			uint8_t memory_read_1_bytes_no_lock(uint32_t addr);

			void memory_write_1_bytes(uint32_t addr, uint8_t data);
			void memory_write_2_bytes(uint32_t addr, uint16_t data);
			void memory_write_4_bytes(uint32_t addr, uint32_t data);
			void memory_write_8_bytes(uint32_t addr, uint64_t data);
			void memory_write_n_bytes(uint32_t addr, void *data, uint32_t len);

			inline rs_instruction_t** get_dep_vector_handler(uint32_t vector, uint32_t code) {
				#ifdef SANITY_CHECK
					SANITY_ASSERT(vector < this->dep_vector_number);
					SANITY_ASSERT(code < this->dep_vector[vector].number);
				#endif
				return this->dep_vector[vector].deps + code;
			}

			inline branch_pred_t* get_bp() {
				return this->bp;
			}

			inline fetch_buffer_t* get_fetch_buffer() {
				return this->fb;
			}

/*			inline mmu_t* get_mmu() {
				return this->mmu;
			}*/

			inline rs_instruction_t* get_inst_from_buffer() {
				return this->rb->get_inst_from_supply();
			}

			uint8_t dispatch(rs_instruction_t *s);

			void write_back(functional_units_response_t *r);

			void arrived_to_commit(rs_instruction_t *s);

			void cycle();

			#ifndef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
				inline uint64_t get_ncycle() {
					return this->ncycle;
				}
			#endif

			inline void set_pc(target_addr_t pc);
			inline target_addr_t get_pc();

			inline target_addr_t get_spec_pc() {
				return this->spec_pc;
			}

			inline void set_spec_pc(target_addr_t pc) {
				this->spec_pc = pc;
			}

			uint8_t flush_pipeline_simple(rs_instruction_t **decoder_buffer); // return the number of buffered instructions
			void flush_pipeline_others();
			uint8_t flush_pipeline_on_wb(rs_instruction_t **decoder_buffer, rs_instruction_t *s);

			void print_registers();

			void system_halted();

			inline uint32_t get_id() {
				return this->id;
			}

			inline uint32_t get_virtual_id() {
				return this->virtual_id;
			}

			inline void warn_branch_predictor(branch_el_t *branch) {
				this->bp->add_entry(branch, this);
			}

			// this is the only public atributte, because
			// it is used by lots of other classes
			cpu_statistics statistics;

			void print_statistics(char *fname);

		#include "arch_class_processor.h"
	};

	#include "arch_general_end.h"
}; // end namespace cpu

// defined in main.cpp
cpu::processor_t* advancedss_get_processor_obj(uint32_t id);

#endif
