#CC = /root/gcc-x86-64/bin/x86_64-elf-gcc
#LD = /root/gcc-x86-64/bin/x86_64-elf-ld
CC = gcc
LD = ld
CFLAGS = -nostdinc -fno-stack-protector
LDFLAGS = -nostdlib -s -T script2.ld
BIN_NAME = kernel.bin
RM = rm

########################################################

SRC_LIST_C_ = interrupt2.c lib.c process.c video.c printk.c start_c.c memory.c syscalls.c external_hd.c keyboard.c
SRC_LIST_S_ = entry.S interrupt.S

########################################################

SRC_LIST_C = $(addprefix src_kernel/, $(SRC_LIST_C_))
SRC_LIST_S = $(addprefix src_kernel/, $(SRC_LIST_S_))

OBJS_C = ${SRC_LIST_C:.c=.o}
OBJS_S = ${SRC_LIST_S:.S=.o}

########################################################

# implicit rules

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@

########################################################

all: c_code asm_code
	$(LD) $(LDFLAGS) -o $(BIN_NAME) src_kernel/*.o
	@echo kernel linked!
	@echo enjoy!

c_code: $(OBJS_C)
	@echo C compiled

asm_code: $(OBJS_S)
	@echo asm compiled

sys_progs_console: sys_progs/console.o
	@echo System process console compiled

clean:
	-$(RM) src_kernel/*.o
	-$(RM) $(BIN_NAME)

