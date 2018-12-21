#CC = /root/gcc-x86-64/bin/x86_64-elf-gcc
#LD = /root/gcc-x86-64/bin/x86_64-elf-ld
CC =  /home/eduardo/advanced-ss-compiler/bin/x86_64-pc-elf-gcc
LD = /home/eduardo/advanced-ss-compiler/bin/x86_64-pc-elf-gcc
CFLAGS = 
LDFLAGS = -static -T ../script2.ld
RM = rm
CP = cp

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.S
	$(CC) $(CFLAGS) -c $< -o $@

########################################################

all: console.o crt0.o libgloss.o
	$(CP) crt0.o /home/eduardo/advanced-ss-compiler-debug/x86_64-pc-elf/lib
	$(LD) $(LDFLAGS) -o ../console.bin console.o libgloss.o
	@echo kernel linked!
	@echo enjoy!

clean:
	-$(RM) *.o
	-$(RM) ../console.bin


