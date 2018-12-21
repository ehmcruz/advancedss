CC = gcc
CPP = g++
LD = g++
FLAGS = -ggdb -O2
CFLAGS = $(FLAGS)
CPPFLAGS = $(FLAGS)
LDFLAGS = -lpthread -lncurses -ggdb
SIM_TARGET_ARCH = x86_64
SIM_HOST_CODE = ncurses
BIN_NAME = advancedss
RM = rm

# -fprofile-arcs -ftest-coverage

########################################################

SRC_CPU_ = branch_pred.cpp cpu.cpp decoder.cpp fetch_buffer.cpp func_units.cpp mmu.cpp reorder_buffer.cpp rs_instruction.cpp rs_policy.cpp cache.cpp

SRC_IO_ = io.cpp keyboard.cpp memory.cpp video.cpp external_hd.cpp int_controller.cpp timer.cpp

SRC_GENERAL_ = config_file.cpp main.cpp

SRC_ARCH_x86_64 = dependency.cpp x86_64.cpp x86_64_decoder.cpp bochs/cpu/arith16.cpp bochs/cpu/cpuid.cpp bochs/cpu/flag_ctrl.cpp bochs/cpu/msr.cpp bochs/cpu/shift8.cpp bochs/cpu/arith32.cpp bochs/cpu/ctrl_xfer16.cpp bochs/cpu/flag_ctrl_pro.cpp bochs/cpu/mult16.cpp bochs/cpu/soft_int.cpp bochs/cpu/arith64.cpp bochs/cpu/ctrl_xfer32.cpp bochs/cpu/fpu_emu.cpp bochs/cpu/mult32.cpp bochs/cpu/stack16.cpp bochs/cpu/arith8.cpp bochs/cpu/ctrl_xfer64.cpp bochs/cpu/io.cpp bochs/cpu/mult64.cpp bochs/cpu/stack32.cpp bochs/cpu/bcd.cpp bochs/cpu/data_xfer16.cpp bochs/cpu/lazy_flags.cpp bochs/cpu/mult8.cpp bochs/cpu/stack64.cpp bochs/cpu/bit16.cpp bochs/cpu/data_xfer32.cpp bochs/cpu/load32.cpp bochs/cpu/proc_ctrl.cpp bochs/cpu/string.cpp bochs/cpu/bit32.cpp bochs/cpu/data_xfer64.cpp bochs/cpu/logical16.cpp bochs/cpu/resolver.cpp bochs/cpu/bit64.cpp bochs/cpu/data_xfer8.cpp bochs/cpu/logical32.cpp bochs/cpu/shift16.cpp bochs/cpu/bit.cpp bochs/cpu/fetchdecode64.cpp bochs/cpu/logical64.cpp bochs/cpu/shift32.cpp bochs/cpu/cpu.cpp bochs/cpu/fetchdecode.cpp bochs/cpu/logical8.cpp bochs/cpu/shift64.cpp bochs/cpu/sse.cpp bochs/cpu/sse_move.cpp bochs/cpu/sse_pfp.cpp bochs/cpu/sse_rcp.cpp bochs/cpu/sse_string.cpp bochs/cpu/mmx.cpp bochs/fpu/f2xm1.cpp bochs/fpu/fpu_compare.cpp bochs/fpu/fpu_tags.cpp bochs/fpu/softfloat.cpp bochs/fpu/ferr.cpp bochs/fpu/fpu_const.cpp bochs/fpu/fpu_trans.cpp bochs/fpu/softfloat-round-pack.cpp bochs/fpu/fpatan.cpp bochs/fpu/fpu.cpp bochs/fpu/fsincos.cpp bochs/fpu/softfloat-specialize.cpp bochs/fpu/fprem.cpp bochs/fpu/fpu_load_store.cpp bochs/fpu/fyl2x.cpp bochs/fpu/softfloatx80.cpp bochs/fpu/fpu_arith.cpp bochs/fpu/fpu_misc.cpp bochs/fpu/poly.cpp


# x86_64_decoder.cpp

SRC_HOST_CODE_compatible = console.cpp

SRC_HOST_CODE_ncurses = console.cpp

########################################################

SIM_TARGET_ARCH_DIR = src/cpu/$(SIM_TARGET_ARCH)

SRC_CPU = $(addprefix src/cpu/, $(SRC_CPU_))
SRC_IO = $(addprefix src/io/, $(SRC_IO_))
SRC_GENERAL = $(addprefix src/, $(SRC_GENERAL_))
SRC_ARCH = $(addprefix src/cpu/$(SIM_TARGET_ARCH)/, $(SRC_ARCH_$(SIM_TARGET_ARCH)))
SRC_HOST = $(addprefix src/host_code/$(SIM_HOST_CODE)/, $(SRC_HOST_CODE_$(SIM_HOST_CODE)))

########################################################

OBJS_CPU_ = ${SRC_CPU:.cpp=.o}
OBJS_CPU = ${OBJS_CPU_:.c=.o}

OBJS_IO_ = ${SRC_IO:.cpp=.o}
OBJS_IO = ${OBJS_IO_:.c=.o}

OBJS_GENERAL_ = ${SRC_GENERAL:.cpp=.o}
OBJS_GENERAL = ${OBJS_GENERAL_:.c=.o}

OBJS_ARCH_ = ${SRC_ARCH:.cpp=.o}
OBJS_ARCH = ${OBJS_ARCH_:.c=.o}

OBJS_HOST_ = ${SRC_HOST:.cpp=.o}
OBJS_HOST = ${OBJS_HOST_:.c=.o}

########################################################

# implicit rules

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@

########################################################

all: sanity_test cpu io general arch host
	$(LD) $(LDFLAGS) -o $(BIN_NAME) src/*.o src/cpu/*.o $(OBJS_ARCH) src/io/*.o src/host_code/$(SIM_HOST_CODE)/*.o
	@echo program linked!
	@echo enjoy!

cpu: $(OBJS_CPU)
	@echo CPU compiled

io: $(OBJS_IO)
	@echo IO compiled

general: $(OBJS_GENERAL)
	@echo general compiled

arch: $(OBJS_ARCH)
	@echo arch $(SIM_TARGET_ARCH) compiled

host: $(OBJS_HOST)
	@echo host $(SIM_HOST_CODE) compiled

sanity_test:     \
                 $(SIM_TARGET_ARCH_DIR)/$(SIM_TARGET_ARCH)_general_start.h  \
                 $(SIM_TARGET_ARCH_DIR)/$(SIM_TARGET_ARCH)_general_end.h  \
                 $(SIM_TARGET_ARCH_DIR)/$(SIM_TARGET_ARCH)_class_processor.h \
                 $(SIM_TARGET_ARCH_DIR)/$(SIM_TARGET_ARCH)_class_decoder.h
	@echo SANITY OK

clean:
	-$(RM) src/*.o
	-$(RM) src/cpu/*.o
	-$(RM) src/io/*.o
	-$(RM) src/host_code/$(SIM_HOST_CODE)/*.o
	-$(RM) $(OBJS_ARCH)
	-$(RM) $(BIN_NAME)

