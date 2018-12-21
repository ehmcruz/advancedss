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

#include <stdlib.h>
#include <stdio.h>
#include "cpu.h"
#include "../io/io.h"

#ifdef DEBUG_CPU
	extern int64_t debug_console_go;
#endif

namespace cpu {
	uint32_t cfg_max_bytes_decoded_per_cycle;
	uint32_t cfg_instructions_cycles_to_exec[32];
	uint8_t cfg_detect_branch_misprediction_on_wb;
};

using namespace cpu;

void cpu::clear_cpu_statistics(cpu_statistics *s)
{
	uint8_t i;

	for (i=0; i<32; i++) {
		s->committed_uops[i] = 0;
		s->dispatched_uops[i] = 0;
		s->executed_uops[i] = 0;
	}

	s->fetch_queue_cumulative_occupancy = 0;
	s->rs_queue_cumulative_occupancy = 0;
	s->ls_queue_cumulative_occupancy = 0;
	s->io_queue_cumulative_occupancy = 0;
	s->rb_cumulative_occupancy = 0;
	s->exec_units_cumulative_occupancy = 0;
	s->fetch_width_used_cumulative = 0;
	s->branch_pred_correct = 0;
	s->pipeline_flushes_on_commit = 0;
	s->pipeline_flushes_on_wb = 0;
	s->branch_missing = 0;
	s->branch_pred_total = 0;
	s->average_inst_size = 0;
	s->average_inst_size_i = 0;
	s->average_bytes_between_branchs = 0;
	s->average_bytes_between_branchs_i = 0;
	s->average_uops_per_inst = 0;
	s->average_uops_per_inst_i = 0;
	s->dispatched_inst = 0;
	s->commited_inst = 0;
	s->decode_width_used_cumulative = 0;
}

char* cpu::get_inst_type_str(uint32_t type)
{
	static char *list[] = {
		"none",
		"int-arith",
		"int-logic",
		"int-mul",
		"int-div",
		"data-transfer",
		"load",
		"store",
		"branch",
		"fpu-arith",
		"fpu-mul",
		"fpu-div",
		"io-in",
		"io-out",
		"invalid"
	};

	return (type < CPU_INSTRUCTION_TYPE_NUMBER) ? list[type] : list[CPU_INSTRUCTION_TYPE_NUMBER];
}

#ifdef DEBUG_CPU
	void cpu::print_instruction_type(uint32_t type)
	{
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_ARITH))
			DEBUG_PRINTF("Int-Arith, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_LOGIC))
			DEBUG_PRINTF("Int-Logic, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_MUL))
			DEBUG_PRINTF("Int-Mul, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_INT_DIV))
			DEBUG_PRINTF("Int-Div, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_MOV))
			DEBUG_PRINTF("Mov, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_LOAD))
			DEBUG_PRINTF("Load, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_STORE))
			DEBUG_PRINTF("Store, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_BRANCH))
			DEBUG_PRINTF("Branch, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_FPU_ARITH))
			DEBUG_PRINTF("Fpu-Arith, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_FPU_MUL))
			DEBUG_PRINTF("Fpu-Mul, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_FPU_DIV))
			DEBUG_PRINTF("Fpu-Div, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_IO_IN))
			DEBUG_PRINTF("IO-in, ");
		if (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_IO_OUT))
			DEBUG_PRINTF("IO-out, ");
	}

	char* cpu::get_instruction_status_str(uint32_t status)
	{
		static char *tb[] = {
			"DECODING",
			"WAIT_DEP",
			"DEP_READY",
			"EXECUTING",
			"READY"
		};

		return tb[status];
	}
#endif

/***********************************************************************/

void cpu::load_config()
{
	advancedss_config *cfg = get_advancedss_main_config();
	advancedss_config cfg_inst;
	char *s;
	uint32_t i;

	cfg_max_bytes_decoded_per_cycle = cfg->read_int32("decoder_max_bytes_per_cycle");
	
	s = cfg->read_str("detect_branch_misprediction_on_wb");
	if (!strcmp(s, "true"))
		cfg_detect_branch_misprediction_on_wb = 1;
	else
		cfg_detect_branch_misprediction_on_wb = 0;

	for (i=0; i<32; i++) {
		cfg_instructions_cycles_to_exec[i] = 1;
	}

	cfg_inst.open("inst_cycles.conf");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_INT_ARITH] =
		cfg_inst.read_int32("int_arith");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_INT_LOGIC] =
		cfg_inst.read_int32("int_logic");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_INT_MUL] =
		cfg_inst.read_int32("int_mul");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_INT_DIV] =
		cfg_inst.read_int32("int_div");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_MOV] =
		cfg_inst.read_int32("mov");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_BRANCH] =
		cfg_inst.read_int32("branch");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_LOAD] =
		cfg_inst.read_int32("load");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_STORE] =
		cfg_inst.read_int32("store");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_IO_IN] =
		cfg_inst.read_int32("io_in");
	cfg_instructions_cycles_to_exec[CPU_INSTRUCTION_TYPE_IO_OUT] =
		cfg_inst.read_int32("io_out");
}

cpu::cpu_shared_component_t::cpu_shared_component_t(uint32_t number_processors_shared)
{
	this->my_cpus = (processor_t**)malloc(sizeof(processor_t*)*number_processors_shared);
	SANITY_ASSERT(this->my_cpus != NULL);
	this->number_processors_shared = number_processors_shared;
}

cpu::processor_t::processor_t(phy_processor_t *phy_cpu, uint32_t virtual_id)
{
	advancedss_config *cfg = get_advancedss_main_config();
	uint32_t i, j;

	clear_cpu_statistics(&this->statistics);

	this->id = phy_cpu->id;
	this->virtual_id = virtual_id;
	this->rs_policy = phy_cpu->rs_policy;
	this->ls_queue = phy_cpu->ls_queue;
	this->io_queue = phy_cpu->io_queue;
	this->f_units = phy_cpu->f_units;
	this->rb = phy_cpu->rb;
	this->bp = phy_cpu->bp;
	//this->mmu = mmu;
	this->fb = phy_cpu->fb;
	this->decoder = new decoder_t(this); // decoder needs fetch buffer
	this->memory = phy_cpu->memory;

	this->set_pc(0);
	this->spec_pc = 0;

	#ifdef CPU_DETECT_PIPELINE_DEADLOCK_CYCLES
		this->number_locked_cycles = 0;
	#endif

	this->i_locked_memory = 0;

	this->has_received_external_interrupt = 0;

	#ifndef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
		this->ncycle = 0;
	#endif

	this->initialize_arch( advancedss_get_boot_size() );

	for (i=0; i<this->dep_vector_number; i++) {
		for (j=0; j<this->dep_vector[i].number; j++) {
			this->dep_vector[i].deps[j] = NULL;
		}
	}
	
	SANITY_ASSERT(CPU_INSTRUCTION_TYPE_NONE == 0)
	
	this->num_of_branches[CPU_INSTRUCTION_TYPE_BRANCH] = 0;
	this->wb_flusher = NULL;
}

void cpu::processor_t::set_number_dep_vectors(uint32_t n)
{
	this->dep_vector = (dependency_vector_t*)malloc( sizeof(dependency_vector_t) * n );
	if (!this->dep_vector) {
		LOG_PRINTF("error malloc cpu::processor_t::set_number_dep_vectors\n");
		exit(1);
	}
	this->dep_vector_number = n;
}

void cpu::processor_t::setup_dep_vector(uint32_t vector_index, uint32_t size)
{
	this->dep_vector[vector_index].deps = (rs_instruction_t**)malloc( sizeof(rs_instruction_t*) * size );
	if (!this->dep_vector[vector_index].deps) {
		LOG_PRINTF("error malloc cpu::processor_t::setup_dep_vector\n");
		exit(1);
	}
	this->dep_vector[vector_index].number = size;
}

uint8_t cpu::processor_t::dispatch(rs_instruction_t *s)
{
	uint8_t success;
	rs_policy_t *rs_policy;
	uint32_t type = CPU_INST_TYPE_MASK( s->get_type() );

//	DEBUG_PRINTF("1-inst id=%llu vaddr=0x%llX cycles=%u\n", s->get_id(), s->get_vaddr(), s->get_cycles_to_exec());
//	DEBUG_PRINTF("2-inst id=%llu vaddr=0x%llX cycles=%u\n", s->get_id(), s->get_vaddr(), s->get_cycles_to_exec());
	
	if (this->wb_flusher == NULL) {
		this->num_of_branches[s->get_type()]++;
		this->wb_flusher = (s->get_pipeline_state() != RS_INSTRUCTION_PIPELINE_VALID_CAN_FLUSH_PIPELINE_IN_WRITEBACK) ? NULL : s;
	}

	/* chose what queue it will be inserted in depending on
	   instruction type
	*/

	if (type & (CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_LOAD) | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_STORE))) {
//		if (s->get_cycles_to_exec() > 500){DEBUG_PRINTF("4-inst id=%llu vaddr=0x%llX cycles=%u\n", s->get_id(), s->get_vaddr(), s->get_cycles_to_exec()); exit(1);}
		success = this->ls_queue->add(s);
		rs_policy = this->ls_queue;
	}
	else if (type & (CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_IO_IN) | CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_IO_OUT))) {
		success = this->io_queue->add(s);
		rs_policy = this->io_queue;
	}
	else if (s->get_type() != 0) {
		success = this->rs_policy->add(s);
		rs_policy = this->rs_policy;
	}
	else  { // no need to execute
		success = 1;
		rs_policy = NULL;
		s->set_status(CPU_INSTRUCTION_STATUS_READY);
	}

	/* now add to the reorder buffer
	   there will always be space available in the RB, since
	   the slot was previously allocated by the decoder */

	if (success) {
		this->statistics.dispatched_uops[ s->get_type() ]++;
		s->set_cycles_to_exec(s->get_cycles_to_exec() + cfg_instructions_cycles_to_exec[s->get_type()]);
		if (s->get_memory_result()) {
			s->set_cycles_to_exec( s->get_cycles_to_exec() + cache_memory_struct->access(1, s->get_memory_operand_addr(), s->get_memory_operand_length(), (type & CPU_INST_TYPE_MASK(CPU_INSTRUCTION_TYPE_LOAD)) ? CPU_CACHE_READ : CPU_CACHE_WRITE ));
		}
		
		if (s->get_microcode_state() == RS_INSTRUCTION_MICROCODE_FIRST) {
			this->statistics.dispatched_inst++;
			if (s->get_pipeline_state() != RS_INSTRUCTION_PIPELINE_INVALID) {
				this->statistics.average_inst_size += s->get_length();
				this->statistics.average_inst_size_i++;
			}
		}
		s->set_rs_policy(rs_policy);
		this->rb->add(s);
		return 1;
	}
	else {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("cpu::processor_t::dispatch FAIL\n");
		#endif
		return 0;
	}
}

void cpu::processor_t::arrived_to_commit(rs_instruction_t *s)
{
	if (s->get_rs_policy() != NULL) {
		s->get_rs_policy()->arrived_to_commit(s);
	}
}

uint8_t cpu::processor_t::flush_pipeline_simple(rs_instruction_t **decoder_buffer)
{
	uint8_t decoder_buffer_n;
	uint32_t i, j;

//LOG_PRINTF("pipeline flush at cycle %llu\n", advancedss_get_system_cycle());
	this->rs_policy->flush(this);
	this->ls_queue->flush(this);
	this->io_queue->flush(this);
	this->f_units->flush(this);
	decoder_buffer_n = this->decoder->get_buffered_rs(decoder_buffer);
	this->decoder->flush();
	this->fb->flush(this);

	#ifdef CPU_DETECT_PIPELINE_DEADLOCK_CYCLES
		this->number_locked_cycles = 0;
	#endif

	for (i=0; i<this->dep_vector_number; i++) {
		for (j=0; j<this->dep_vector[i].number; j++) {
			this->dep_vector[i].deps[j] = NULL;
		}
	}

	this->statistics.pipeline_flushes_on_commit++;
	
	this->num_of_branches[CPU_INSTRUCTION_TYPE_BRANCH] = 0;
	this->wb_flusher = NULL;

	return decoder_buffer_n;
}

uint8_t cpu::processor_t::flush_pipeline_on_wb(rs_instruction_t **decoder_buffer, rs_instruction_t *s)
{
	uint8_t decoder_buffer_n;
	uint32_t i, j;

DEBUG_PRINTF("pipeline flush on wb at cycle %llu inst_id=%llu\n", advancedss_get_system_cycle(), s->get_id());
	this->rs_policy->flush_on_write_back(s);
	this->ls_queue->flush_on_write_back(s);
	this->io_queue->flush_on_write_back(s);
	this->f_units->flush_on_write_back(s);
	
	if (s->get_status() != CPU_INSTRUCTION_STATUS_DECODING) {
		decoder_buffer_n = this->decoder->get_buffered_rs(decoder_buffer);
		this->decoder->flush();
	}
	else
		decoder_buffer_n = 0;

	this->fb->flush(this);

	#ifdef CPU_DETECT_PIPELINE_DEADLOCK_CYCLES
		this->number_locked_cycles = 0;
	#endif

	for (i=0; i<this->dep_vector_number; i++) {
		for (j=0; j<this->dep_vector[i].number; j++) {
			if (this->dep_vector[i].deps[j] != NULL && this->dep_vector[i].deps[j]->get_id() > s->get_id()) {
				this->dep_vector[i].deps[j] = NULL;
			}
		}
	}

	this->statistics.pipeline_flushes_on_wb++;
	
	this->num_of_branches[CPU_INSTRUCTION_TYPE_BRANCH] = 0;
	this->wb_flusher = NULL;
	
	s->set_branch_foresight(1);

	return decoder_buffer_n;
}

void cpu::processor_t::flush_pipeline_others()
{
	this->bp->flush(this);
}

void cpu::processor_t::write_back(functional_units_response_t *r)
{
	uint32_t j;
	rs_instruction_t *s;
	
	for (j=0; j<r->n_instructions; j++) {
		s = r->instructions[j];
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("\tInstruction (%s) id=%u wb\n", s->get_unmounted_inst(), CONVERT64TOPRINT(s->get_id()));
		#endif
		s->write_back();
		s->get_rs_policy()->del( s );
		if (s->get_pipeline_state() != RS_INSTRUCTION_PIPELINE_INVALID) {
			this->num_of_branches[s->get_type()]--;
			if (s->get_is_branch() && cfg_detect_branch_misprediction_on_wb) {
				branch_el_t branch;
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("\tis a branch\n");
				#endif

				branch.mem_addr = s->get_vaddr() + s->get_length() - 1;
				branch.target_addr = s->get_branch_target();
				branch.taken = s->get_branch_taken();
				branch.is_call = 0;
				branch.is_ret = 0;

				this->warn_branch_predictor(&branch);

				this->statistics.branch_pred_total++;
				if (!s->branch_mis_predicted()) {
					this->statistics.branch_pred_correct++;
				}
			}
		}
	}

	if (cfg_detect_branch_misprediction_on_wb && this->wb_flusher != NULL && this->num_of_branches[CPU_INSTRUCTION_TYPE_BRANCH] == 0) {
		// i can flush on wb, there is no need to wait commit
		this->rb->flush_on_write_back(this->wb_flusher->get_last_microcode());
	}
}

void cpu::processor_t::io_space_read(void *data, uint8_t size, uint32_t port)
{
	io::call_io_handler_read(port, data, size);
}

void cpu::processor_t::io_space_write(void *data, uint8_t size, uint32_t port)
{
	io::call_io_handler_write(port, data, size);
}

void cpu::processor_t::memory_lock()
{
	if (advancedss_get_number_phy_cpu() > 1) {
		this->memory->lock();
	}
	this->i_locked_memory = 1;
}

void cpu::processor_t::memory_unlock()
{
	if (advancedss_get_number_phy_cpu() > 1) {
		this->memory->unlock();
	}
	this->i_locked_memory = 0;
}

void cpu::processor_t::memory_check_lock()
{
	if (this->i_locked_memory == 0 && advancedss_get_number_phy_cpu() > 1) { // if i dont do this if a deadlock occurs
		this->memory->check_lock();
	}
}

void cpu::processor_t::memory_check_unlock()
{
	if (this->i_locked_memory == 0 && advancedss_get_number_phy_cpu() > 1) { // if i dont do this if a deadlock occurs
		this->memory->check_unlock();
	}
}

uint8_t cpu::processor_t::memory_read_1_bytes(uint32_t addr)
{
	uint8_t data;
	this->memory_check_lock();
	data = this->memory->read1(addr);
	this->memory_check_unlock();
	return data;
}

uint8_t cpu::processor_t::memory_read_1_bytes_no_lock(uint32_t addr)
{
	return this->memory->read1(addr);
}

uint16_t cpu::processor_t::memory_read_2_bytes(uint32_t addr)
{
	uint16_t data;
	this->memory_check_lock();
	data = this->memory->read2(addr);
	this->memory_check_unlock();
	return data;
}

uint32_t cpu::processor_t::memory_read_4_bytes(uint32_t addr)
{
	uint32_t data;
	this->memory_check_lock();
	data = this->memory->read4(addr);
	this->memory_check_unlock();
	return data;
}

uint64_t cpu::processor_t::memory_read_8_bytes(uint32_t addr)
{
	uint64_t data;
	this->memory_check_lock();
	data = this->memory->read8(addr);
	this->memory_check_unlock();
	return data;
}

void cpu::processor_t::memory_read_n_bytes(uint32_t addr, void *data, uint32_t len)
{
	this->memory_check_lock();
	this->memory->read(data, len, addr);
	this->memory_check_unlock();
}

void cpu::processor_t::memory_write_1_bytes(uint32_t addr, uint8_t data)
{
	this->memory_check_lock();
	this->memory->write1(addr, data);
	this->memory_check_unlock();
}

void cpu::processor_t::memory_write_2_bytes(uint32_t addr, uint16_t data)
{
	this->memory_check_lock();
	this->memory->write2(addr, data);
	this->memory_check_unlock();
}

void cpu::processor_t::memory_write_4_bytes(uint32_t addr, uint32_t data)
{
	this->memory_check_lock();
	this->memory->write4(addr, data);
	this->memory_check_unlock();
}

void cpu::processor_t::memory_write_8_bytes(uint32_t addr, uint64_t data)
{
	this->memory_check_lock();
	this->memory->write8(addr, data);
	this->memory_check_unlock();
}

void cpu::processor_t::memory_write_n_bytes(uint32_t addr, void *data, uint32_t len)
{
	this->memory_check_lock();
	this->memory->write(data, len, addr);
	this->memory_check_unlock();
}

void cpu::processor_t::system_halted()
{
	DEBUG_PRINTF("\n\n\n************************* SYSTEM HALTED *************************\n");
	DEBUG_PRINTF("CPU %u registers:\n\n", this->get_id());
	this->print_registers();
	DEBUG_PRINTF("\n\n\n************************* SYSTEM HALTED *************************\n\n\n");
}

void cpu::processor_t::cycle()
{
	#ifdef CPU_DETECT_PIPELINE_DEADLOCK_CYCLES
		if (this->decoder->get_mis_speculation())
			this->number_locked_cycles++;
		if (this->number_locked_cycles >= CPU_DETECT_PIPELINE_DEADLOCK_CYCLES) {
			this->print_registers();
			LOG_PRINTF("execution halted due to pipeline deadlock\n");
			SANITY_ASSERT(0)
		}
	#endif

	#ifdef DEBUG_CPU
//		if (debug_console_go == -1)
//			getchar();
		DEBUG_PRINTF("**********************\ncpu::processor_t::cycle   DECODE\n\n");
	#endif
	this->decoder->new_cycle();
	while (!this->decoder->must_stop_decoding_in_cycle()) {
		this->pipeline_before_decode_inst();
		#ifdef CPU_CHECK_EXECUTION_TRACE
			this->decoder->decode(this->get_pc());
		#else
			this->decoder->decode();
		#endif
		#ifdef DEBUG_CPU
			//this->print_registers();
		#endif
		this->pipeline_after_decode_inst();
	#ifdef DEBUG_CPU
		if (debug_console_go == -1)
			getchar();
	#endif
	}

/*if (this->get_cpl()==3 && this->get_pc() >= 0x400198) {
	exception_code_t exc;
	uint8_t has_exception;
	mem_check_exc_read(0x7FFFFFFFDE20, 8, has_exception, exc);
	if (this->mem_read_8_bytes(0x7FFFFFFFDE20) == 0x7FFFFFFFDE20) {
		LOG_PRINTF("ahhhhhhhh ....... pc 0x"PRINTF_INT64X_PREFIX"\n",this->get_pc());
	}
	LOG_PRINTF("/*cycle end\n");
}*/
//if (this->memory->read8(0x3e008) == 0x40  && this->get_cpl()==0) {LOG_PRINTF("ahhhhhhhh ....... pc 0x"PRINTF_INT64X_PREFIX"\n",this->get_pc());}
//		if (this->read_gen_reg_64(4)==0x0000008000002F88){LOG_PRINTF("pc 0x%llX GPR 64 bits   %u    value=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(),  4, this->read_gen_reg_64(4), this->read_gen_reg_64(4)); SANITY_ASSERT(0);}
//if (this->memory->read8(0xc120) != 0x00) {LOG_PRINTF("ahhhhhhhh ....... pc 0x"PRINTF_INT64X_PREFIX"\n",this->get_pc());}
//		if (this->read_gen_reg_64(4)==0x0000008000002F88){LOG_PRINTF("pc 0x%llX GPR 64 bits   %u    value=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(),  4, this->read_gen_reg_64(4), this->read_gen_reg_64(4)); SANITY_ASSERT(0);}
	#ifndef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
		this->ncycle++;
	#endif
}

void cpu::processor_t::print_statistics(char *fname)
{
	FILE *fp;
	uint8_t i;
	uint64_t commited_total = 0, dispatched_total = 0, executed_total = 0, cache_total;
	cpu_statistics *s = &statistics;
	#define PI(VAR, NAME...) fprintf(fp, NAME); fprintf(fp, "#%llu\n", VAR);
	#define PF(VAR, NAME...) fprintf(fp, NAME); fprintf(fp, "#%.3f\n", VAR);

	// backup
	fp = fopen(fname, "r");
	if (fp) {
		FILE *fp_backup;
		char fn[50];
		sprintf(fn, "%s.bak", fname);
		fp_backup = fopen(fn, "w");
		if (fp_backup) {
			static char *backup = NULL;
			static int allocated = 0;
			int size;
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			rewind(fp);
			if (allocated < size) {
				backup = (char*)realloc(backup, size);
				if (backup != NULL)
					allocated = size;
			}
			if (backup) {
				fread(backup, sizeof(char), size, fp);
				fwrite(backup, sizeof(char), size, fp_backup);
			}
			fclose(fp_backup);
		}
		fclose(fp);
	}

	fp = fopen(fname, "w");

	PI(advancedss_get_system_cycle(), "cycles");

	for (i=0; i<32; i++) {
		commited_total += s->committed_uops[i];
		dispatched_total += s->dispatched_uops[i];
		executed_total += s->executed_uops[i];
		PI(s->committed_uops[i], "committed_%s", get_inst_type_str(i));
		PI(s->dispatched_uops[i], "dispatched_%s", get_inst_type_str(i));
		PI(s->executed_uops[i], "executed_%s", get_inst_type_str(i));
	}

	PI(commited_total, "commited_total");
	PI(dispatched_total, "dispatched_total");
	PI(executed_total, "executed_total");

	PI(s->fetch_queue_cumulative_occupancy, "fetch_queue_cumulative_occupancy");
	PI(s->rs_queue_cumulative_occupancy, "rs_queue_cumulative_occupancy");
	PI(s->ls_queue_cumulative_occupancy, "ls_queue_cumulative_occupancy");
	PI(s->io_queue_cumulative_occupancy, "io_queue_cumulative_occupancy");
	PI(s->rb_cumulative_occupancy, "rb_cumulative_occupancy");
	PI(s->exec_units_cumulative_occupancy, "exec_units_cumulative_occupancy");
	PI(s->fetch_width_used_cumulative, "fetch_width_used_cumulative");
	PI(s->decode_width_used_cumulative, "decode_width_used_cumulative");

	PF((double)s->fetch_queue_cumulative_occupancy/(double)advancedss_get_system_cycle(), "fetch_queue_occupancy_rate");
	PF((double)s->rs_queue_cumulative_occupancy/(double)advancedss_get_system_cycle(), "rs_queue_occupancy_rate");
	PF((double)s->ls_queue_cumulative_occupancy/(double)advancedss_get_system_cycle(), "ls_queue_occupancy_rate");
	PF((double)s->io_queue_cumulative_occupancy/(double)advancedss_get_system_cycle(), "io_queue_occupancy_rate");
	PF((double)s->rb_cumulative_occupancy/(double)advancedss_get_system_cycle(), "rb_occupancy_rate");
	PF((double)s->exec_units_cumulative_occupancy/(double)advancedss_get_system_cycle(), "exec_units_occupancy_rate");
	PF((double)s->fetch_width_used_cumulative/(double)advancedss_get_system_cycle(), "fetch_width_used_rate");
	PF((double)s->decode_width_used_cumulative/(double)advancedss_get_system_cycle(), "decode_width_used_rate");

	PF((double)advancedss_get_system_cycle()/(double)s->pipeline_flushes_on_wb, "cycles_per_pipeline_flush_wb");
	
	PF((double)advancedss_get_system_cycle()/(double)s->pipeline_flushes_on_commit, "cycles_per_pipeline_flush_commit");

	PF(host_sim_get_running_time(), "simulation_time");
	PI((uint64_t)((double)advancedss_get_system_cycle()/host_sim_get_running_time()), "cycles/second");
	
	PI(this->decoder->x86_64_statistic_cache_hit, "x86_64_decoder_internal_cache_hit");
	PI(this->decoder->x86_64_statistic_cache_flush, "x86_64_decoder_internal_cache_flush");
	PF((double)this->decoder->x86_64_statistic_cache_hit/(double)this->decoder->x86_64_statistic_cache_access, "x86_64_decoder_internal_cache_hit_rate");

	PI(this->x86_64_statistic_vm_cache_hit, "x86_64_vm_internal_cache_hit");
	PI(this->x86_64_statistic_vm_cache_flush, "x86_64_vm_internal_cache_flush");
	PF((double)this->x86_64_statistic_vm_cache_hit/(double)this->x86_64_statistic_vm_cache_access, "x86_64_vm_internal_cache_hit_rate");
	
	PI(s->pipeline_flushes_on_commit, "pipeline_flushes_on_commit");
	PI(s->pipeline_flushes_on_wb, "pipeline_flushes_on_wb");
	
	PI(s->branch_missing, "branch_missing");

	PF((double)s->dispatched_inst/(double)advancedss_get_system_cycle(), "disp_inst_rate");
	PF((double)commited_total/(double)advancedss_get_system_cycle(), "ipc_uops");
	PF((double)s->commited_inst/(double)advancedss_get_system_cycle(), "ipc");

	PI(cache_memory_struct->get_cache(0)->statistic_hits, "l1_inst_cache_hits");
	PI(cache_memory_struct->get_cache(0)->statistic_misses, "l1_inst_cache_misses");
	cache_total = cache_memory_struct->get_cache(0)->statistic_hits + cache_memory_struct->get_cache(0)->statistic_misses;
	PF((double)cache_memory_struct->get_cache(0)->statistic_hits/(double)cache_total, "l1_inst_cache_hit_rate");
	PI(cache_memory_struct->get_cache(0)->statistic_copy_back, "l1_inst_copy_back");
	
	PI(cache_memory_struct->get_cache(1)->statistic_hits, "l1_data_cache_hits");
	PI(cache_memory_struct->get_cache(1)->statistic_misses, "l1_data_cache_misses");
	cache_total = cache_memory_struct->get_cache(1)->statistic_hits + cache_memory_struct->get_cache(1)->statistic_misses;
	PF((double)cache_memory_struct->get_cache(1)->statistic_hits/(double)cache_total, "l1_data_cache_hit_rate");
	PI(cache_memory_struct->get_cache(1)->statistic_copy_back, "l1_data_copy_back");
	
	PI(cache_memory_struct->get_cache(2)->statistic_hits, "l2_cache_hits");
	PI(cache_memory_struct->get_cache(2)->statistic_misses, "l2_cache_misses");
	cache_total = cache_memory_struct->get_cache(2)->statistic_hits + cache_memory_struct->get_cache(2)->statistic_misses;
	PF((double)cache_memory_struct->get_cache(2)->statistic_hits/(double)cache_total, "l2_cache_hit_rate");
	PI(cache_memory_struct->get_cache(2)->statistic_copy_back, "l2_copy_back");
	
	PF((double)cache_memory_struct->statistic_memory_latency_medium/(double)cache_memory_struct->statistic_memory_latency_medium_i, "average_memory_latency");
	
	PF((double)(this->decoder->statistic_instructions - this->decoder->statistic_instructions_invalid)/(double)this->decoder->statistic_instructions, "implemented_instructions");

	PI(s->branch_pred_total, "total_branchs");
	PI(s->branch_pred_correct, "branch_pred_correct");

	PF((double)s->branch_pred_correct/(double)s->branch_pred_total, "branch_pred_hit_rate");
	
	PF((double)this->x86_64_statistic_cycle_user/(double)(this->x86_64_statistic_cycle_user + this->x86_64_statistic_cycle_kernel), "time_rate_user");
	
	PF((double)this->bp->statistic_hash_hit/(double)(this->bp->statistic_hash_hit + this->bp->statistic_hash_miss), "branch_hash_hit");
	
	PF((double)s->average_inst_size/(double)s->average_inst_size_i, "average_inst_size");
	PF((double)s->average_bytes_between_branchs/(double)s->average_bytes_between_branchs_i, "average_bytes_between_branchs");
	PF((double)s->average_uops_per_inst/(double)s->average_uops_per_inst_i, "average_uops_per_inst");

	fclose(fp);
}

