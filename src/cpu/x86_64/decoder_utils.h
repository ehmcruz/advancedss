/* get mod:
 *     bbaa aaaa
 *     return bb
 *
 * get rm:
 *     aaaa abbb
 *     return bbb
 *
 * get regop
 *     aabb baaa
 *     return bbb
 */

#define CPU_X8664_FLAGS_POS_CF           0
#define CPU_X8664_FLAGS_POS_PF           2
#define CPU_X8664_FLAGS_POS_AF           4
#define CPU_X8664_FLAGS_POS_ZF           6
#define CPU_X8664_FLAGS_POS_SF           7
#define CPU_X8664_FLAGS_POS_DF           10
#define CPU_X8664_FLAGS_POS_OF           11

#define MODRM_GET_G (this->state->bochs_inst.nnn())
#define MODRM_GET_E (this->state->bochs_inst.rm())

#define CPU_X8664_DECODER_GET_RS(N)       CPU_DECODER_GET_RS(N)

#define CPU_X8664_DECODER_SET_INSTRUCTION_TYPE(N, TYPE)   CPU_DECODER_SET_INSTRUCTION_TYPE(CPU_X8664_DECODER_GET_RS(N), TYPE )

#define MODRM_GET_CALCULATED_MEMORY_ADDR       (BX_CPU_CALL_METHODR_(this->my_cpu, (this->state->bochs_inst.ResolveModrm), (&this->state->bochs_inst)))

#define BOCHS_INST (this->state->bochs_inst)

#define CPU_X8664_DEC_FIRST_OPCODE  \
	this->state->first_opcode

#define CPU_X8664_DEC_SECOND_OPCODE  \
	this->state->second_opcode

/*******************************************************************/

#define CPU_X8664_READ_GPR_64(R) \
	this->my_cpu->read_gen_reg_64(R)

#define CPU_X8664_READ_GPR_32(R) \
	this->my_cpu->read_gen_reg_32(R)

#define CPU_X8664_READ_GPR_16(R) \
	this->my_cpu->read_gen_reg_16(R)

/*******************************************************************/

#define CPU_X8664_SET_RESULT_TYPE_MEMORY_STORE(N, ADDR, LENGTH) {   \
		x86_64_virtual_memory_data vm;\
		vm.vaddr = vaddr;\
		this->my_cpu->get_physical_memory_address(&vm, this->my_cpu->x86_64_vm_cache);\
		SET_RESULT_TYPE_MEMORY_STORE(CPU_X8664_DECODER_GET_RS(N), vm.paddr, LENGTH)\
	}

#define CPU_X8664_SET_RESULT_TYPE_MEMORY_LOAD(N, ADDR, LENGTH)  {  \
		x86_64_virtual_memory_data vm;\
		vm.vaddr = vaddr;\
		this->my_cpu->get_physical_memory_address(&vm, this->my_cpu->x86_64_vm_cache);\
		SET_RESULT_TYPE_MEMORY_LOAD(CPU_X8664_DECODER_GET_RS(N), vm.paddr, LENGTH)\
	}

/*******************************************************************/

#define CPU_X8664_CREATE_MICROCODE_LINK(DEST, SRC)   \
	CREATE_DEP(CPU_X8664_DECODER_GET_RS(DEST), CPU_X8664_DECODER_GET_RS(SRC))

/*******************************************************************/

#define CPU_X8664_DEC_CHECK_RFLAGS_DEP(N, CODE)     \
	{ \
		rs_instruction_t *s; \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_RFLAGS, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
	}

#define CPU_X8664_DEC_SET_RFLAGS_DEP(N, CODE)     \
		SET_DEP(CPU_X8664_DEP_VECTOR_RFLAGS, CODE, CPU_X8664_DECODER_GET_RS(N)); \

/*******************************************************************/

#define CPU_X8664_DEC_CHECK_MODRM_MEMORY_DEP(N)    \
	if (this->state->addr_size == 64) {  \
		if (this->state->modrm_has_dep_base) {  \
			CPU_X8664_DEC_CHECK_GPR_64_DEP(N, this->state->modrm_dep_base)   \
		}  \
		if (this->state->modrm_has_dep_index) {  \
			CPU_X8664_DEC_CHECK_GPR_64_DEP(N, this->state->modrm_dep_index)   \
		}  \
	}  \
	else {  \
		if (this->state->modrm_has_dep_base) {  \
			CPU_X8664_DEC_CHECK_GPR_32_DEP(N, this->state->modrm_dep_base)   \
		}  \
		if (this->state->modrm_has_dep_index) {  \
			CPU_X8664_DEC_CHECK_GPR_32_DEP(N, this->state->modrm_dep_index)   \
		}  \
	}

/*******************************************************************/

#define CPU_X8664_DEC_CHECK_MSR_DEP(N, CODE)     \
	if (CODE < CPU_X8664_NUMBER_MSR) { \
		rs_instruction_t *s; \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_MSR, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
	}

#define CPU_X8664_DEC_SET_MSR_DEP(N, CODE)     \
	if (CODE < CPU_X8664_NUMBER_MSR) { \
		SET_DEP(CPU_X8664_DEP_VECTOR_MSR, CODE, CPU_X8664_DECODER_GET_RS(N)); \
	}

/*******************************************************************/

#define CPU_X8664_DEC_CHECK_GPR_64_DEP(N, CODE)     \
	{ \
		rs_instruction_t *s; \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_0_7, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_8_15, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_16_63, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
	}

#define CPU_X8664_DEC_CHECK_GPR_16_TO_63_DEP(N, CODE)     \
	{ \
		rs_instruction_t *s; \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_16_63, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
	}

#define CPU_X8664_DEC_CHECK_GPR_32_DEP(N, CODE)     \
	CPU_X8664_DEC_CHECK_GPR_64_DEP(N, CODE)

#define CPU_X8664_DEC_CHECK_GPR_16_DEP(N, CODE)     \
	{ \
		rs_instruction_t *s; \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_0_7, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
		s = GET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_8_15, CODE); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
	}

#define CPU_X8664_DEC_CHECK_GPR_8_DEP(N, CODE)     \
	{ \
		rs_instruction_t *s; \
		uint8_t vector, c;  \
		vector = ((this->state->bochs_rex_prefix & 0xF0) == 0x40 || (CODE & 0x04) == 0)  \
			? CPU_X8664_DEP_VECTOR_GPR_BITS_0_7  \
			: CPU_X8664_DEP_VECTOR_GPR_BITS_8_15;  \
		c = (vector == CPU_X8664_DEP_VECTOR_GPR_BITS_0_7) ? CODE : CODE & 0x03; \
		s = GET_DEP(vector, c); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
	}

#define CPU_X8664_DEC_CHECK_GPR_8_DEP_NOREX(N, CODE, HIGH_BITS)     \
	{ \
		rs_instruction_t *s; \
		uint8_t vector, c;  \
		vector = (!HIGH_BITS)  \
			? CPU_X8664_DEP_VECTOR_GPR_BITS_0_7  \
			: CPU_X8664_DEP_VECTOR_GPR_BITS_8_15;  \
		c = (vector == CPU_X8664_DEP_VECTOR_GPR_BITS_0_7) ? CODE : CODE & 0x03; \
		s = GET_DEP(vector, c); \
		if (s != NULL) { \
			CREATE_DEP(CPU_X8664_DECODER_GET_RS(N), s);              \
		} \
	}


/*******************************************************************/

#define CPU_X8664_DEC_SET_GPR_64_DEP(N, CODE)     \
	SET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_0_7, CODE, CPU_X8664_DECODER_GET_RS(N)); \
	SET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_8_15, CODE, CPU_X8664_DECODER_GET_RS(N)); \
	SET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_16_63, CODE, CPU_X8664_DECODER_GET_RS(N));

#define CPU_X8664_DEC_SET_GPR_32_DEP(N, CODE)     \
	CPU_X8664_DEC_SET_GPR_64_DEP(N, CODE)

#define CPU_X8664_DEC_SET_GPR_16_DEP(N, CODE)     \
	SET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_0_7, CODE, CPU_X8664_DECODER_GET_RS(N)); \
	SET_DEP(CPU_X8664_DEP_VECTOR_GPR_BITS_8_15, CODE, CPU_X8664_DECODER_GET_RS(N));

#define CPU_X8664_DEC_SET_GPR_8_DEP(N, CODE)  {   \
		uint8_t vector, c;  \
		vector = ((this->state->bochs_rex_prefix & 0xF0) == 0x40 || (CODE & 0x04) == 0)  \
			? CPU_X8664_DEP_VECTOR_GPR_BITS_0_7  \
			: CPU_X8664_DEP_VECTOR_GPR_BITS_8_15;  \
		c = (vector == CPU_X8664_DEP_VECTOR_GPR_BITS_0_7) ? CODE : CODE & 0x03; \
		SET_DEP(vector, c, CPU_X8664_DECODER_GET_RS(N)); \
	}

#define CPU_X8664_DEC_SET_GPR_8_DEP_NOREX(N, CODE, HIGH_BITS)  {   \
		uint8_t vector, c;  \
		vector = (!HIGH_BITS)  \
			? CPU_X8664_DEP_VECTOR_GPR_BITS_0_7  \
			: CPU_X8664_DEP_VECTOR_GPR_BITS_8_15;  \
		c = (vector == CPU_X8664_DEP_VECTOR_GPR_BITS_0_7) ? CODE : CODE & 0x03; \
		SET_DEP(vector, c, CPU_X8664_DECODER_GET_RS(N)); \
	}

/*************************************************************/

#define CPU_X8664_CONDITIONAL_FLAG_MAKEDEP(OP, RS) \
	switch (OP) { \
		case 0x00: \
		case 0x01: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_OF )  \
		break; \
		\
		case 0x02: \
		case 0x03: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_CF )  \
		break; \
		\
		case 0x04: \
		case 0x05: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_ZF )  \
		break; \
		\
		case 0x06: \
		case 0x07: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_CF )  \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_ZF )  \
		break; \
		\
		case 0x08: \
		case 0x09: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_SF )  \
		break; \
		\
		case 0x0A: \
		case 0x0B: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_PF )  \
		break; \
		\
		case 0x0C: \
		case 0x0D: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_SF )  \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_OF )  \
		break; \
		\
		case 0x0E: \
		case 0x0F: \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_SF )  \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_OF )  \
			CPU_X8664_DEC_CHECK_RFLAGS_DEP( RS, CPU_X8664_FLAGS_POS_ZF )  \
	}

