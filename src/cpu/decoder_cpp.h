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

#ifdef DEBUG_CPU
	#define DECODER_H_CPP_DEBUG(...)   DEBUG_PRINTF(__VA_ARGS__);
#else
	#define DECODER_H_CPP_DEBUG(...)
#endif

#define CPU_DECODER_GET_RS(N)  this->buffered_inst[N]

#define CPU_DECODER_ADD_OPCODE_BYTE(B)         this->inst_opcodes[ this->inst_opcodes_i++ ] = B;

#define GET_NUMBER_BYTES_IN_BUFFER \
	this->inst_opcodes_i

#define GET_OPCODES_BUFFER \
	this->inst_opcodes

#define CPU_DECODER_INST_DEBUG(...)   { \
		uint8_t i; \
		for (i=0; i<this->required_rs_slots; i++) \
			sprintf(this->buffered_inst[i]->get_unmounted_inst(), __VA_ARGS__); \
		DEBUG_PRINTF(__VA_ARGS__); \
		DEBUG_PRINTF("\n"); \
	}

#define CPU_DECODER_INSTRUCTION_EXCEPTION(CODE)       \
	this->exception = 1;  \
	this->exception_code = CODE; \
	this->inst_unexpected_behavior = 1;

#define CPU_DECODER_SET_INSTRUCTION_ADDR(V)       this->inst_addr = V;
#define CPU_DECODER_SET_INSTRUCTION_LENGTH(V)     this->inst_length = V;

#define CPU_DECODER_GET_NEXT_INSTRUCTION_ADDR      (this->inst_addr + this->inst_length)

#define CPU_DECODER_GET_INSTRUCTION_ADDR      this->inst_addr

#define CPU_DECODER_GET_INSTRUCTION_LENGTH      this->inst_length

#define CPU_DECODER_SET_INSTRUCTION_TYPE(S, TYPE)        (S)->set_type( TYPE ); this->check_if_branch_instruction[TYPE] = 1;

#define SKIP_BYTES_FROM_FETCH_BUFFER(N, TO_ADD)   {  \
		uint32_t to_skip__ = cfg_max_bytes_decoded_per_cycle - this->bytes_decoded_in_cycle; \
		uint32_t r__; \
		\
		to_skip__ = ((N) < to_skip__) ? (N) : to_skip__; \
		r__ = this->fb->skip_bytes(&(this->next_inst_addr), to_skip__, this->my_cpu->get_virtual_id());  \
		TO_ADD += r__; \
		if (r__ < (N)) { \
			this->stop_decoding_in_cycle = 1;  \
			return;  \
		} \
	}

//			if (ENABLED_EXEC) {LOG_PRINTF("fetch byte 0x%X from vddr=0x%llX   next=0x%llX\n", (B)->data, (uint64_t)((B)->vaddr), (uint64_t)((B)->next_inst));} \

#define GET_BYTE_FROM_FETCH_BUFFER(B)     \
	if (this->bytes_decoded_in_cycle < cfg_max_bytes_decoded_per_cycle && this->fb->has_bytes(1, this->my_cpu->get_virtual_id())) {  \
		this->fb->get_byte(B, this->my_cpu);  \
		if ((B)->is_exception == 0) { \
			(B)->data = this->my_cpu->memory_read_1_bytes_no_lock((B)->paddr); \
			DEBUG_PRINTF("fetch byte 0x%X from vddr=0x%llX   next=0x%llX\n", (B)->data, (uint64_t)((B)->vaddr), (uint64_t)((B)->next_inst)); \
			this->next_inst_addr = (B)->next_inst;  \
			this->bytes_decoded_in_cycle++;  \
		} \
		else { \
			this->buffer_exception_code = (B)->exception_code; \
			SET_HANDLER(fetch_buffer_exception)  \
			this->alocate_inst(1); \
		} \
	}  \
	else {  \
		this->stop_decoding_in_cycle = 1;  \
		return;  \
	}

#define IS_THERE_BYTE_IN_FETCH_BUFFER     \
	(this->bytes_decoded_in_cycle < cfg_max_bytes_decoded_per_cycle && this->fb->has_bytes(1))

#define CPU_DECODER_SET_REQUIRED_RS(N)         this->alocate_inst(N);

//			LOG_PRINTF("inst 0x%llX   foresight=%u    rip=0x%llX     next=0x%llX\n", this->inst_addr, this->branch_foresight, this->my_cpu->get_pc(), this->next_inst_addr);\

#define INSTRUCTION_END       \
	{  \
		uint8_t i, can_flush_on_wb = 0;  \
		rs_instruction_pipeline_state_t pipeline_state; \
		\
		this->branch_foresight = (this->next_inst_addr == this->my_cpu->get_pc());  \
		\
		if (ENABLED_EXEC) {\
			if (this->inst_unexpected_behavior) {\
				DISABLE_EXEC;\
			}\
			else if (!this->branch_foresight) {\
				DISABLE_EXEC  \
				can_flush_on_wb = this->check_if_branch_instruction[CPU_INSTRUCTION_TYPE_BRANCH];\
			}\
			pipeline_state = (!can_flush_on_wb) ? RS_INSTRUCTION_PIPELINE_VALID : RS_INSTRUCTION_PIPELINE_VALID_CAN_FLUSH_PIPELINE_IN_WRITEBACK;\
		}\
		else {\
			pipeline_state = RS_INSTRUCTION_PIPELINE_INVALID;\
		}\
		\
		for (i=0; i<this->required_rs_slots; i++) {  \
			this->buffered_inst[i]->set_branch_foresight(1);  \
			this->buffered_inst[i]->set_pipeline_state(pipeline_state);  \
			this->buffered_inst[i]->set_microcode_state(RS_INSTRUCTION_MICROCODE_MIDLE);  \
			this->buffered_inst[i]->set_first_microcode(this->buffered_inst[0]);  \
			this->buffered_inst[i]->set_last_microcode(this->buffered_inst[this->required_rs_slots-1]);  \
		}  \
		\
		if (this->is_branch) {  \
			if (this->branch_automatic_detect) {\
				if (this->my_cpu->get_pc() == this->branch_target) {\
					this->branch_taken = 1;\
				}\
				else{\
					this->branch_taken = 0;\
				}\
			}\
			for (i=0; i<this->required_rs_slots; i++) {  \
				this->buffered_inst[i]->set_branch(this->branch_taken, this->branch_target, this->branch_type);  \
			}\
		}  \
		\
		if (this->required_rs_slots > 0) { \
			this->buffered_inst[this->required_rs_slots-1]->set_branch_foresight(this->branch_foresight);  \
			\
			if (this->exception) {  \
				this->buffered_inst[0]->set_exception(this->exception_code);  \
			}  \
			if (this->is_trap) {  \
				this->buffered_inst[0]->set_trap(this->trap_code);  \
			}  \
			if (this->is_halt) {  \
				this->buffered_inst[0]->set_as_halt();  \
			}  \
			if (this->inst_must_flush_pipeline == 1) { \
				this->buffered_inst[0]->set_as_must_flush_pipeline();  \
			} \
			\
			this->buffered_inst[ this->required_rs_slots - 1 ]->set_microcode_state(RS_INSTRUCTION_MICROCODE_LAST);  \
			this->buffered_inst[ 0 ]->set_microcode_state(RS_INSTRUCTION_MICROCODE_FIRST);  \
		} \
		DECODER_H_CPP_DEBUG("INSTRUCTION ENDED\n\tforesight = %u\n", (uint32_t)this->branch_foresight) \
		\
		DEBUG_PRINTF("branch foresight = %u\n", (uint32_t)this->branch_foresight); \
		SET_HANDLER(dispatch)  \
		this->dispatch();  \
	}

#define ENABLED_EXEC       (!this->mis_speculation)

#define DISABLE_EXEC       this->mis_speculation = 1;

#define SET_HANDLER(H)     this->handler = &decoder_t::H;

#define EXEC_HANDLER      (this->*handler)();

#define CREATE_DEP(DEST, SRC)           rs_instruction_t::create_dep(DEST, SRC);

#define GET_DEP(VECTOR, OFFSET)         *this->my_cpu->get_dep_vector_handler(VECTOR, OFFSET);

#define SET_DEP(VECTOR, OFFSET, RS)   { \
		rs_instruction_t **h; \
		DECODER_H_CPP_DEBUG("adding dep to vector=%u  offset=%u\n", (uint32_t)VECTOR, (uint32_t)OFFSET)   \
		h = this->my_cpu->get_dep_vector_handler(VECTOR, OFFSET); \
		*h = RS	;  \
		RS->add_out_deps_regs(h);  \
	}

#define SET_RESULT_TYPE_BRANCH(TAKEN, TARGET, TYPE)   \
	this->is_branch = 1;  \
	this->branch_taken = TAKEN;  \
	this->branch_target = TARGET;\
	this->branch_type = TYPE;

#define SET_RESULT_TYPE_BRANCH_AUTOMATIC(TARGET, TYPE)   \
	this->branch_automatic_detect = 1;\
	SET_RESULT_TYPE_BRANCH(0, TARGET, TYPE)

#define SET_RESULT_TYPE_TRAP(CODE)       \
	this->is_trap = 1;  \
	this->trap_code = CODE;\
	this->inst_unexpected_behavior = 1;
	
#define CPU_DECODER_INST_MUST_FLUSH_PIPELINE \
	this->inst_must_flush_pipeline = 1;\
	this->inst_unexpected_behavior = 1;

#define SET_RESULT_TYPE_MEMORY_STORE(RS, ADDR, LENGTH) {  \
		if (ENABLED_EXEC) {\
			RS->set_memory_result(CPU_INSTRUCTION_TYPE_STORE, ADDR, LENGTH);\
		}\
	}

#define SET_RESULT_TYPE_MEMORY_LOAD(RS, ADDR, LENGTH) {  \
		if (ENABLED_EXEC) {\
			RS->set_memory_result(CPU_INSTRUCTION_TYPE_LOAD, ADDR, LENGTH);\
		}\
	}

#define UPDATE_PC(PC)      \
	this->my_cpu->set_pc(PC);

#define SET_RESULT_TYPE_SYSTEM_HALT           \
	this->is_halt = 1;\
	this->inst_unexpected_behavior = 1;

#define CPU_DECODER_MEMORY_LOCK  \
	this->my_cpu->memory_lock();

#define CPU_DECODER_MEMORY_UNLOCK  \
	this->my_cpu->memory_unlock();














