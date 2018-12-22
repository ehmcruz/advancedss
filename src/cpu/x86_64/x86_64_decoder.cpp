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

#include "bochs/bochs.h"
#include "bochs/cpu/fetchdecode_opinfo.h"

// defined in bochs_fetchdecode64.cpp
extern const Bit8u BxOpcodeHasModrm64[512];
extern const BxOpcodeInfo_t BxOpcodeInfo64R[512*3];
extern const BxOpcodeInfo_t BxOpcodeInfo64M[512*3];

#include "../cpu.h"

using namespace cpu;

#include "../decoder_cpp.h"

extern const x8664_dep_rs_num_type_t x8664_rs_num_type[512*2+1];
extern const x8664_dep_call_table_t x8664_dep_call_tb[512*2*3+1];

#define SSE_PREFIX_NONE 0
#define SSE_PREFIX_66   1
#define SSE_PREFIX_F2   2
#define SSE_PREFIX_F3   3

void cpu::decoder_t::initialize_decoder_arch()
{
	this->x86_64_flush_inst_cache();
	this->x86_64_statistic_cache_access = 0;
	this->x86_64_statistic_cache_hit = 0;
	this->x86_64_statistic_cache_flush = 0;
	this->statistic_instructions_invalid = 0;
	this->statistic_instructions = 0;
}

void cpu::decoder_t::x86_64_flush_inst_cache()
{
	uint32_t i;
	
	for (i=0; i<CPU_X86_64_DECODER_INST_CACHE_TABLE_SIZE; i++) {
		this->inst_cache_tb[i].busy = 0;
	}
	
	this->x86_64_statistic_cache_flush++;
}

void cpu::decoder_t::initialize_instruction()
{
	this->is_first_byte = 1;
	this->inst_addr = 0;
	this->inst_length = 0;

	SET_HANDLER(x86_64_decode)
	this->x86_64_decode();
}

/*void cpu::decoder_t::x86_64_decode()
{
	fetch_buffer_el_t b;
	uint8_t opcode;

	while (IS_THERE_BYTE_IN_FETCH_BUFFER) {
		GET_BYTE_FROM_FETCH_BUFFER(&b)
		opcode = b.data;
		CPU_DECODER_ADD_OPCODE_BYTE(opcode)

		if (this->is_first_byte) {
			this->is_first_byte = 0;
			CPU_DECODER_SET_INSTRUCTION_ADDR(b.vaddr)
		}

		if (this->my_cpu->fetchDecode64(GET_OPCODES_BUFFER, &this->bochs_inst, GET_NUMBER_BYTES_IN_BUFFER))
			break;
	}

	CPU_DECODER_SET_INSTRUCTION_LENGTH(this->bochs_inst.ilen())
}*/

void cpu::decoder_t::x86_64_decode()
{
	fetch_buffer_el_t b;
	uint8_t opcode;
	uint8_t prefix_end = 0;

	do {
		GET_BYTE_FROM_FETCH_BUFFER(&b)

		opcode = b.data;
		CPU_DECODER_ADD_OPCODE_BYTE(opcode)

		if (this->is_first_byte) {
			uint32_t index;
			
			this->is_first_byte = 0;
			CPU_DECODER_SET_INSTRUCTION_ADDR(b.vaddr)
			
			index = CPU_X86_64_DECODER_INST_CACHE_TABLE_GET_INDEX(b.vaddr);
			this->state = &this->inst_cache_tb[index];
			//LOG_PRINTF("index=%u, busy=%u, vaddr=0x%llX  wanted=0x%llX\n", index, this->state->busy, this->state->vaddr, b.vaddr);
			this->x86_64_statistic_cache_access++;
					
			if (this->state->busy == 1 && this->state->vaddr == b.vaddr) {
				// cache-hit! yes!
				this->x86_64_statistic_cache_hit++;
				this->bytes_to_get = this->state->length;
				this->bytes_already_got = 1;  // 1 because we already got the first byte
				SET_HANDLER(x86_64_get_bytes)
				this->x86_64_get_bytes();
				return;
			}
			
			this->state->busy = 0;
			
			this->state->vaddr = b.vaddr;
			
			this->state->opcode_bytes = 0;
			this->state->has_b3 = 0;
			this->state->addr_size = 64;
			this->state->bochs_sse_prefix = SSE_PREFIX_NONE;
			this->state->bochs_offset = 512;
			this->state->bochs_rex_prefix = 0;
			this->state->has_modrm = 0;
			this->state->modrm_disp_bytes = 0;
			this->state->has_sib = 0;
			this->state->disp_already_fetched = 0;
			this->state->im_already_fetched = 0;
			this->state->im_bytes = 0;
			this->state->prefix_number = 0;
			this->state->required_rs = 0;

			this->state->modrm_memory_operand = 0;
	
			this->state->modrm_has_dep_base = 0;
			this->state->modrm_has_dep_index = 0;
		}

		switch (opcode) {
			// rex prefix
			case 0x40:
			case 0x41:
			case 0x42:
			case 0x43:
			case 0x44:
			case 0x45:
			case 0x46:
			case 0x47:
			case 0x48:
			case 0x49:
			case 0x4A:
			case 0x4B:
			case 0x4C:
			case 0x4D:
			case 0x4E:
			case 0x4F:
				this->state->bochs_rex_prefix = opcode;
				this->state->prefix_number++;
				DEBUG_PRINTF("found rex prefix\n");
			break;

			// repeat prefix
			case 0xf2: // REPNE/REPNZ
			case 0xf3: // REP/REPE/REPZ
				this->state->bochs_sse_prefix = opcode & 0xf;
				this->state->prefix_number++;
				this->state->bochs_rex_prefix = 0;
				DEBUG_PRINTF("found rep prefix\n");
			break;

			// addr size prefix
			case 0x67:
				this->state->addr_size = 32;
				this->state->prefix_number++;
				this->state->bochs_rex_prefix = 0;
				DEBUG_PRINTF("addr size override prefix\n");
			break;

			// segment override prefix
			case 0x2e: // CS:
			case 0x26: // ES:
			case 0x36: // SS:
			case 0x3e: // DS:

			// lock prefix
			case 0xf0:
				this->state->prefix_number++;
				this->state->bochs_rex_prefix = 0;
				DEBUG_PRINTF("lock prefix\n");
			break;

			// op size prefix
			case 0x66:
				this->state->prefix_number++;
				this->state->bochs_offset = 0;
				this->state->bochs_rex_prefix = 0;
				if(!this->state->bochs_sse_prefix)
					this->state->bochs_sse_prefix = SSE_PREFIX_66;
				DEBUG_PRINTF("opsize override prefix\n");
			break;

			// our simulator doesn't support these
			case 0x64: // FS:
			case 0x65: // GS:
				this->state->prefix_number++;
				this->state->bochs_rex_prefix = 0;
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("found segment fs or gs prefix\n");
				#endif
				if (this->my_cpu->get_cpl() == CPU_X8664_CPL_KERNEL && ENABLED_EXEC) {
					LOG_PRINTF("fs and gs segment prefix still not supported by simulator\n");
					LOG_PRINTF("rip = 0x"PRINTF_INT64X_PREFIX"\n", this->my_cpu->get_pc());
					exit(1);
				}
			break;

			// 2 byte opcode
			case 0x0f:
				prefix_end = 1;
				this->state->opcode_bytes = 2;
				this->state->first_opcode = 0x0f;
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("2 byte opcode\n");
				#endif
				SET_HANDLER(x86_64_get_second_opcode)
				this->x86_64_get_second_opcode();
			break;

			// 1 byte opcode
			default:
				prefix_end = 1;
				this->state->opcode_bytes = 1;
				this->state->first_opcode = opcode;
				this->state->bochs_opcode = opcode;
				#ifdef DEBUG_CPU
					DEBUG_PRINTF("1 byte opcode = 0x%X\n", opcode);
				#endif
				SET_HANDLER(x86_64_after_get_opcodes)
				this->x86_64_after_get_opcodes();
		}
	} while (!prefix_end);
}

void cpu::decoder_t::x86_64_get_second_opcode()
{
	fetch_buffer_el_t b;
	uint8_t opcode;

	GET_BYTE_FROM_FETCH_BUFFER(&b)

	opcode = b.data;
	CPU_DECODER_ADD_OPCODE_BYTE(opcode)

	this->state->second_opcode = opcode;
	this->state->bochs_opcode = 0x0100 | opcode;

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("found second opcode = 0x%X\n", opcode);
	#endif

	SET_HANDLER(x86_64_after_get_opcodes)
	this->x86_64_after_get_opcodes();
}

void cpu::decoder_t::x86_64_after_get_opcodes()
{
	this->state->bochs_offset = (this->state->bochs_rex_prefix & 0x8) ? 512*2 : this->state->bochs_offset;
	this->state->bochs_attr = BxOpcodeInfo64R[this->state->bochs_opcode+this->state->bochs_offset].Attr;

	if (BxOpcodeHasModrm64[this->state->bochs_opcode]) {
		this->state->has_modrm = 1;
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("has modrm, getting it...\n");
		#endif
		SET_HANDLER(x86_64_get_modrm)
		this->x86_64_get_modrm();
	}
	else {
		DEBUG_PRINTF("no modrm\n");
		SET_HANDLER(x86_64_get_im)
		this->x86_64_get_im();
	}
}

void cpu::decoder_t::x86_64_get_modrm()
{
	fetch_buffer_el_t b;
	uint8_t opcode;

	GET_BYTE_FROM_FETCH_BUFFER(&b)

	opcode = b.data;
	CPU_DECODER_ADD_OPCODE_BYTE(opcode)

#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
	// handle 3-byte escape
	if ((this->state->bochs_attr & BxGroupX) == Bx3ByteOp) {
		this->state->bochs_b3 = opcode;
		this->state->has_b3 = 1;
		DEBUG_PRINTF("3 byte escape\n");
		SET_HANDLER(x86_64_get_modrm_3byte)
		this->x86_64_get_modrm_3byte();
	}
	else
#endif
	{
		this->state->modrm = opcode;
		SET_HANDLER(x86_64_after_get_modrm)
		this->x86_64_after_get_modrm();
	}
}

void cpu::decoder_t::x86_64_get_modrm_3byte()
{
	fetch_buffer_el_t b;
	uint8_t opcode;

	GET_BYTE_FROM_FETCH_BUFFER(&b)

	opcode = b.data;
	CPU_DECODER_ADD_OPCODE_BYTE(opcode)

	this->state->modrm = opcode;

	SET_HANDLER(x86_64_after_get_modrm)
	this->x86_64_after_get_modrm();
}

#define BOCHS_MODRM_GET_MOD(V)      (V & 0xC0)
#define BOCHS_MODRM_GET_RM(V)       (V & 0x07)

#define REX_B(R) ((R & 0x1) << 3)
#define REX_X(R) ((R & 0x2) << 2)

void cpu::decoder_t::x86_64_after_get_modrm()
{
	uint8_t mod, rm;

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("modrm got = 0x%X\n", this->state->modrm);
	#endif

	mod = BOCHS_MODRM_GET_MOD(this->state->modrm);
	rm = BOCHS_MODRM_GET_RM(this->state->modrm);

	if ((this->state->bochs_opcode & ~3) == 0x120) {
		mod = 0xc0;
		DEBUG_PRINTF("control register\n");
	}

	if (mod == 0xC0) {
		DEBUG_PRINTF("no memory\n");
		SET_HANDLER(x86_64_modrm_done)
		this->x86_64_modrm_done();
	}
	else {
		DEBUG_PRINTF("memory operand\n");
		if (rm != 4) { // no s-i-b byte
			if (mod == 0x00) {
				if (rm == 5)
					this->state->modrm_disp_bytes = 4;
			}
			else {
				this->state->modrm_has_dep_base = 1;
				this->state->modrm_dep_base = rm | REX_B(this->state->bochs_rex_prefix);
				if (mod == 0x40) // mod == 01b
					this->state->modrm_disp_bytes = 1;
				else
					this->state->modrm_disp_bytes = 4;
			}

			if (this->state->modrm_disp_bytes) {
				SET_HANDLER(x86_64_get_disp)
				this->x86_64_get_disp();
			}
			else {
				SET_HANDLER(x86_64_modrm_done)
				this->x86_64_modrm_done();
			}
		}
		else {
			this->state->has_sib = 1;
			SET_HANDLER(x86_64_get_sib)
			this->x86_64_get_sib();
		}
	}
}

#define BOCHS_SIB_GET_BASE(V)      (V & 0x07)

void cpu::decoder_t::x86_64_get_sib()
{
	fetch_buffer_el_t b;
	uint8_t opcode, mod, base, index;

	GET_BYTE_FROM_FETCH_BUFFER(&b)

	opcode = b.data;
	CPU_DECODER_ADD_OPCODE_BYTE(opcode)

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("sib got = 0x%X\n", opcode);
	#endif

	mod = BOCHS_MODRM_GET_MOD(this->state->modrm);
	this->state->sib = opcode;

	base = BOCHS_SIB_GET_BASE(this->state->sib);
	index = ((this->state->sib >> 3) & 0x7);
	
	if (index != 4) {
		this->state->modrm_has_dep_index = 1;
		this->state->modrm_dep_index = index | REX_X(this->state->bochs_rex_prefix);
	}

	if (mod == 0) {
		if (base == 5)
			this->state->modrm_disp_bytes = 4;
		else {
			this->state->modrm_has_dep_base = 1;
			this->state->modrm_dep_base = base | REX_B(this->state->bochs_rex_prefix);
		}
	}
	else if (mod == 0x40)
		this->state->modrm_disp_bytes = 1;
	else
		this->state->modrm_disp_bytes = 4;

	if (this->state->modrm_disp_bytes) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("has disp of %u bytes, getting it...\n", (uint32_t)this->state->modrm_disp_bytes);
		#endif
		SET_HANDLER(x86_64_get_disp)
		this->x86_64_get_disp();
	}
	else {
		SET_HANDLER(x86_64_modrm_done)
		this->x86_64_modrm_done();
	}
}

void cpu::decoder_t::x86_64_get_disp()
{
	fetch_buffer_el_t b;
	uint8_t opcode;

	while (this->state->disp_already_fetched < this->state->modrm_disp_bytes) {
		GET_BYTE_FROM_FETCH_BUFFER(&b)

		opcode = b.data;
		CPU_DECODER_ADD_OPCODE_BYTE(opcode)

		#ifdef DEBUG_CPU
			DEBUG_PRINTF("disp got byte %u of %u = 0x%X\n", (uint32_t)this->state->disp_already_fetched, this->state->modrm_disp_bytes, opcode);
		#endif

		this->state->disp_already_fetched++;
	}

	SET_HANDLER(x86_64_modrm_done)
	this->x86_64_modrm_done();
}

void cpu::decoder_t::x86_64_modrm_done()
{
	const BxOpcodeInfo_t *OpcodeInfoPtr;
	uint8_t nnn, rm;

	if (BOCHS_MODRM_GET_MOD(this->state->modrm) == 0xc0) {
		OpcodeInfoPtr = &(BxOpcodeInfo64R[this->state->bochs_opcode+this->state->bochs_offset]);
		this->state->bochs_attr = BxOpcodeInfo64R[this->state->bochs_opcode+this->state->bochs_offset].Attr;
	}
	else {
		OpcodeInfoPtr = &(BxOpcodeInfo64M[this->state->bochs_opcode+this->state->bochs_offset]);
		this->state->bochs_attr = BxOpcodeInfo64M[this->state->bochs_opcode+this->state->bochs_offset].Attr;
		this->state->modrm_memory_operand = 1;
	}

	nnn = (this->state->modrm >> 3) & 0x07;
	rm = this->state->modrm & 0x07;

	while(this->state->bochs_attr & BxGroupX) {
		Bit32u Group = this->state->bochs_attr & BxGroupX;
		this->state->bochs_attr &= ~BxGroupX;

		switch(Group) {
			case BxGroupN:
				OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[nnn]);
				break;
			case BxRMGroup:
				OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[rm]);
				break;
			#if (BX_SUPPORT_SSE >= 4) || (BX_SUPPORT_SSE >= 3 && BX_SUPPORT_SSE_EXTENSION > 0)
				case Bx3ByteOp:
					OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[this->state->bochs_b3]);
					break;
			#endif
			case BxOSizeGrp:
				OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[this->state->bochs_offset >> 8]);
				break;
			case BxPrefixSSE:
				/* For SSE opcodes look into another table
				   with the opcode prefixes (NONE, 0x66, 0xF2, 0xF3) */
				if (this->state->bochs_sse_prefix) {
					OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[this->state->bochs_sse_prefix-1]);
					break;
				}
				continue;
			case BxPrefixSSE66:
				/* For SSE opcodes with prefix 66 only */
				if (this->state->bochs_sse_prefix != SSE_PREFIX_66) {
					OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[0]); // BX_IA_ERROR
				}
				continue;
			case BxFPEscape:
				OpcodeInfoPtr = &(OpcodeInfoPtr->AnotherArray[this->state->modrm & 0x3f]); // here... b2
			break;
		}

		/* get additional attributes from group table */
		this->state->bochs_attr |= OpcodeInfoPtr->Attr;
	}

	SET_HANDLER(x86_64_get_im)
	this->x86_64_get_im();
}

void cpu::decoder_t::x86_64_get_im()
{
	uint32_t imm_mode;

	imm_mode = this->state->bochs_attr & BxImmediate;

	if (imm_mode) {
		switch (imm_mode) {
			case BxImmediate_Ib:
			case BxImmediate_Ib_SE: // Sign extend to OS size
			case BxImmediate_BrOff8:
				this->state->im_bytes = 1;
			break;

			case BxImmediate_Iw:
				this->state->im_bytes = 2;
			break;

			case BxImmediate_Id:
				this->state->im_bytes = 4;
			break;

			case BxImmediate_Iq: // MOV Rx,imm64
				this->state->im_bytes = 8;
			break;

			case BxImmediate_IwIb:
				this->state->im_bytes = 3;
			break;

			case BxImmediate_O:
				// For is which embed the address in the opcode.  Note
				// there is only 64/32-bit addressing available in long-mode.
				if (this->state->addr_size == 64)
					this->state->im_bytes = 8;
				else
					this->state->im_bytes = 4;
			break;
		}
	}

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("immediate size is %u\n", (uint32_t)this->state->im_bytes);
	#endif

	if (this->state->im_bytes) {
		SET_HANDLER(x86_64_get_im_)
		this->x86_64_get_im_();
	}
	else {
		SET_HANDLER(x86_64_after_get_inst)
		this->x86_64_after_get_inst();
	}
}

void cpu::decoder_t::x86_64_get_im_()
{
	fetch_buffer_el_t b;
	uint8_t opcode;

	while (this->state->im_already_fetched < this->state->im_bytes) {
		GET_BYTE_FROM_FETCH_BUFFER(&b)

		opcode = b.data;
		CPU_DECODER_ADD_OPCODE_BYTE(opcode)

		#ifdef DEBUG_CPU
			DEBUG_PRINTF("im got byte %u of %u = 0x%X\n", (uint32_t)this->state->im_already_fetched, this->state->im_bytes, opcode);
		#endif

		this->state->im_already_fetched++;
	}

	SET_HANDLER(x86_64_after_get_inst)
	this->x86_64_after_get_inst();
}

void cpu::decoder_t::x86_64_get_bytes()
{
	//fetch_buffer_el_t b;
	//uint8_t opcode;
	uint8_t skip;

	while (this->bytes_already_got < this->bytes_to_get) {
/*		GET_BYTE_FROM_FETCH_BUFFER(&b)

		opcode = b.data;
		CPU_DECODER_ADD_OPCODE_BYTE(opcode)

		#ifdef DEBUG_CPU
			DEBUG_PRINTF("got byte %u of %u = 0x%X\n", this->bytes_already_got, this->bytes_to_get, opcode);
		#endif

		this->bytes_already_got++;*/
		
		skip = this->bytes_to_get - this->bytes_already_got;
		
		SKIP_BYTES_FROM_FETCH_BUFFER(skip, this->bytes_already_got)
	}
	
	//SANITY_ASSERT(GET_NUMBER_BYTES_IN_BUFFER == this->state->length);
	SANITY_ASSERT(this->bytes_already_got == this->bytes_to_get);

	CPU_DECODER_SET_INSTRUCTION_LENGTH(this->state->length)
	SET_HANDLER(x86_64_after_allocate_rs)
	CPU_DECODER_SET_REQUIRED_RS(this->state->dep_rs_num_type->num)
}

void cpu::decoder_t::x86_64_after_get_inst()
{
	uint32_t r;
	
	SANITY_ASSERT((this->state->prefix_number + this->state->opcode_bytes + this->state->has_b3 + this->state->has_modrm + this->state->has_sib + this->state->modrm_disp_bytes + this->state->im_bytes) == GET_NUMBER_BYTES_IN_BUFFER);

	CPU_DECODER_SET_INSTRUCTION_LENGTH(GET_NUMBER_BYTES_IN_BUFFER)
	this->state->length = GET_NUMBER_BYTES_IN_BUFFER;

	r = this->my_cpu->fetchDecode64(GET_OPCODES_BUFFER, &this->state->bochs_inst, GET_NUMBER_BYTES_IN_BUFFER);
	
	DEBUG_PRINTF("nnn = %u    rm = %u\n", this->state->bochs_inst.nnn(), this->state->bochs_inst.rm());

	SANITY_ASSERT(r == GET_NUMBER_BYTES_IN_BUFFER);
	
	this->state->dep_rs_num_type = &x8664_rs_num_type[ (this->state->modrm_memory_operand << 9) + this->state->bochs_opcode ];
	while (this->state->dep_rs_num_type->another != NULL)
		this->state->dep_rs_num_type = this->state->dep_rs_num_type->another;

	this->state->dep_call_tb = &x8664_dep_call_tb[ (this->state->bochs_offset << 1) + (this->state->modrm_memory_operand << 9) + this->state->bochs_opcode ];
	while (this->state->dep_call_tb->another != NULL)
		this->state->dep_call_tb = this->state->dep_call_tb->another;
		
	/* now, we have completed our "state" */
	this->state->busy = 1;

	SET_HANDLER(x86_64_after_allocate_rs)
	CPU_DECODER_SET_REQUIRED_RS(this->state->dep_rs_num_type->num)
}

#include "decoder_utils.h"

#define CPU_X8664_INSTRUCTION_EXCEPTION(CODE) \
	CPU_DECODER_INSTRUCTION_EXCEPTION( CODE	 ) \
	INSTRUCTION_END \
	return;

void cpu::decoder_t::x86_64_after_allocate_rs()
{
	uint32_t i;
/*	if (this->state->bochs_inst.ia_opcode == BX_IA_ERROR) { // undefined opcode
		CPU_X8664_INSTRUCTION_EXCEPTION( CPU_X8664_EXCEPTION_INVALID_OPCODE )
	}*/
	
	for (i=0; i<this->state->dep_rs_num_type->num; i++) {
		CPU_X8664_DECODER_SET_INSTRUCTION_TYPE(i, this->state->dep_rs_num_type->types[i])
	}
	
	(this->*this->state->dep_call_tb->handler)();
	
	this->statistic_instructions++;
	
	if (ENABLED_EXEC) {
		volatile bochs_exec_status_t status;

		//status.is_branch = 0;
		status.has_exception = 0;
		status.is_halt = 0;
		status.is_trap = 0;
		status.must_flush_pipeline = 0;
		status.bochs_inst = &this->state->bochs_inst;

		#ifdef CPU_CHECK_EXECUTION_TRACE
			uint64_t prev_pc = this->my_cpu->get_pc();
		#endif

		this->my_cpu->bochs_exec(&status);
		
/*		if (this->my_cpu->get_pc() >= 0x4001c0) {
			static int64_t a = 0;
			if (a==0) {
				for (a=this->my_cpu->read_gen_reg_64(BX_64BIT_REG_RSP); a<(this->my_cpu->read_gen_reg_64(BX_64BIT_REG_RSP) | 0x0FFF); a+=8) {
					LOG_PRINTF("memory 0x%llX = 0x%llX\n", a, this->my_cpu->bochs_mem_read_8_bytes(a));
				}
			}
			LOG_PRINTF("pc 0x%llX    rsp 0x%llX     rdx 0x%llX      rsi 0x%llX        rax 0x%llX      rdi 0x%llx\n", this->my_cpu->get_pc(), this->my_cpu->read_gen_reg_64(BX_64BIT_REG_RSP), this->my_cpu->read_gen_reg_64(BX_64BIT_REG_RDX), this->my_cpu->read_gen_reg_64(BX_64BIT_REG_RSI), this->my_cpu->read_gen_reg_64(BX_64BIT_REG_RAX), this->my_cpu->read_gen_reg_64(BX_64BIT_REG_RDI));
		}*/

		#ifdef CPU_CHECK_EXECUTION_TRACE
			//if (pc >= 0x400000) {
				if (!advancedss_check_correct_pc(this->my_cpu->get_pc())) {
					LOG_PRINTF("wrong PC 0x%llX    previous pc 0x%llX", this->my_cpu->get_pc(), prev_pc);
					exit(0);
				}
			//}
		#endif

		if (status.has_exception) {
			CPU_X8664_INSTRUCTION_EXCEPTION( status.exception_code )
		}

		/*if (status.is_branch) {
			SET_RESULT_TYPE_BRANCH(status.branch_taken, status.branch_target)
		}*/

		if (status.is_trap) {
			SET_RESULT_TYPE_TRAP(status.trap_code)
		}

		if (status.is_halt) {
			SET_RESULT_TYPE_SYSTEM_HALT
		}

		if (status.must_flush_pipeline) {
			CPU_DECODER_INST_MUST_FLUSH_PIPELINE
		}
	}

	INSTRUCTION_END
}
