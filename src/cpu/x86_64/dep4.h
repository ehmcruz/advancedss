#define X8664_DEC_PUSH_GPR_HEAD \
	uint8_t reg_src; \
	bx_address vaddr; \
	reg_src = CPU_X8664_DEC_FIRST_OPCODE & 0x07; \
	vaddr = CPU_X8664_READ_GPR_64(BX_64BIT_REG_RSP); \
	\
	CPU_X8664_CREATE_MICROCODE_LINK(1, 0)\
	\
	CPU_X8664_DEC_CHECK_GPR_64_DEP(0, BX_64BIT_REG_RSP)
	
#define X8664_DEC_PUSH_GPR_16BITS \
	vaddr -= 2;\
	CPU_X8664_DEC_CHECK_GPR_16_DEP(1, reg_src) \
	CPU_X8664_SET_RESULT_TYPE_MEMORY_STORE(1, vaddr, 2)

#define X8664_DEC_PUSH_GPR_32BITS \
	vaddr -= 8;\
	CPU_X8664_DEC_CHECK_GPR_64_DEP(1, reg_src) \
	CPU_X8664_SET_RESULT_TYPE_MEMORY_STORE(1, vaddr, 8)

#define X8664_DEC_PUSH_GPR_64BITS \
	vaddr -= 8;\
	CPU_X8664_DEC_CHECK_GPR_64_DEP(1, reg_src) \
	CPU_X8664_SET_RESULT_TYPE_MEMORY_STORE(1, vaddr, 8)

#define X8664_DEC_PUSH_GPR_TAIL \
	CPU_X8664_DEC_SET_GPR_64_DEP(0, BX_64BIT_REG_RSP)

/******************************************************************/

#define X8664_DEC_POP_GPR_HEAD \
	uint8_t reg_dst; \
	bx_address vaddr; \
	reg_dst = CPU_X8664_DEC_FIRST_OPCODE & 0x07; \
	vaddr = CPU_X8664_READ_GPR_64(BX_64BIT_REG_RSP); \
	\
	CPU_X8664_CREATE_MICROCODE_LINK(1, 0)\
	\
	CPU_X8664_DEC_CHECK_GPR_64_DEP(0, BX_64BIT_REG_RSP)

#define X8664_DEC_POP_GPR_16BITS \
	CPU_X8664_SET_RESULT_TYPE_MEMORY_LOAD(1, vaddr, 2) \
	CPU_X8664_DEC_SET_GPR_16_DEP(1, reg_dst)

#define X8664_DEC_POP_GPR_32BITS \
	CPU_X8664_SET_RESULT_TYPE_MEMORY_LOAD(1, vaddr, 8)\
	CPU_X8664_DEC_SET_GPR_64_DEP(1, reg_dst)

#define X8664_DEC_POP_GPR_64BITS \
	CPU_X8664_SET_RESULT_TYPE_MEMORY_LOAD(1, vaddr, 8)\
	CPU_X8664_DEC_SET_GPR_64_DEP(1, reg_dst)

#define X8664_DEC_POP_GPR_TAIL \
	CPU_X8664_DEC_SET_GPR_64_DEP(0, BX_64BIT_REG_RSP)

/******************************************************************/
#define X8664_B0_TO_B7_HEAD \
	uint8_t reg_dst; \
	reg_dst = BOCHS_INST.opcodeReg();\
	CPU_X8664_DEC_SET_GPR_8_DEP(0, reg_dst)

#define X8664_B0_TO_B7_16BITS

#define X8664_B0_TO_B7_32BITS

#define X8664_B0_TO_B7_64BITS

#define X8664_B0_TO_B7_TAIL
/******************************************************************/
#define X8664_B8_TO_BF_HEAD \
	uint8_t reg_dst; \
	reg_dst = BOCHS_INST.opcodeReg();

#define X8664_B8_TO_BF_16BITS \
	CPU_X8664_DEC_SET_GPR_16_DEP(0, reg_dst)

#define X8664_B8_TO_BF_32BITS \
	CPU_X8664_DEC_SET_GPR_32_DEP(0, reg_dst)

#define X8664_B8_TO_BF_64BITS \
	CPU_X8664_DEC_SET_GPR_64_DEP(0, reg_dst)

#define X8664_B8_TO_BF_TAIL
/******************************************************************/

