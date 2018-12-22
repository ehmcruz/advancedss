#include "lib.h"
#include "vars.h"
#include "printk.h"

uint8_t inportb(uint16_t port)
{
	uint8_t ret_val;

	__asm__ __volatile__("inb %1,%0"
		: "=a"(ret_val)
		: "d"(port));
	return ret_val;
}

void outportb(uint16_t port, uint8_t val)
{
	__asm__ __volatile__("outb %b0,%w1"
		:
		: "a"(val), "d"(port));
}

uint32_t inportd(uint16_t port)
{
	uint32_t ret_val;

	__asm__ __volatile__("in %1,%0"
		: "=a"(ret_val)
		: "d"(port));
	return ret_val;
}

void outportd(uint16_t port, uint32_t val)
{
	__asm__ __volatile__("out %0,%w1"
		:
		: "a"(val), "d"(port));
}

uint32_t get_cpuid()
{
	return read_msr_32(CONFIG_KERNEL_MSR_CPUID);
}

uint64_t get_virtual_address_fault()
{
	return read_msr_64(CONFIG_KERNEL_MSR_VIRTUAL_MEMORY_PAGE_FAULT_ADDR);
}

uint64_t read_msr_64(uint32_t code)
{
	uint64_t ret;
	__asm__ __volatile__(
		"rdmsr\n"
		"shl $32, %%rdx\n"
		"xorq %%rdx, %q0"
		: "=a"(ret)
		: "c" (code)
		: "%rdx");
	return ret;
}

uint32_t read_msr_32(uint32_t code)
{
	uint32_t ret;
	__asm__ __volatile__(
		"rdmsr\n"
		: "=a"(ret)
		: "c" (code)
		: "%rdx");
	return ret;
}

void write_msr_64(uint32_t code, uint64_t value)
{
	__asm__ __volatile__(
		"mov %%rax, %%rdx\n"
		"shr $32, %%rdx\n"
		"wrmsr\n"
		:
		: "c" (code), "a" (value)
		: "%rdx");
}

void write_msr_32(uint32_t code, uint32_t value)
{
	__asm__ __volatile__(
		"mov $0, %%edx\n"
		"wrmsr\n"
		:
		: "c" (code), "a" (value)
		: "%rdx");
}

uint32_t get_number_of_cpus()
{
	return read_msr_32(CONFIG_KERNEL_MSR_NUMBER_PHY_CPUS) * read_msr_32(CONFIG_KERNEL_MSR_NUMBER_VIRTUAL_CPUS_PER_PHY_CPU);
}

void* get_kernel_stack_base_for_current_cpu()
{
	return read_msr_64(CONFIG_KERNEL_MSR_KERNEL_STACK);
}
/*
void halt()
{
	__asm__ __volatile__(
		"hlt\n");
}*/

void panic()
{
	uint32_t i = 10000;
	video_set_focus_window_kernel();
	printk("\nkernel panic");
	while (i-- > 0);
	halt();
}

void memcpy(void *dest, void *src, int64_t len)
{
	uint8_t *d, *s;

	d = (uint8_t*)dest;
	s = (uint8_t*)src;

	while (len >= 8) {
		*((uint64_t*)d) = *((uint64_t*)s);

		s += 8;
		d += 8;
		len -= 8;
	}

	while (len > 0) {
		*d = *s;
		s++;
		d++;
		len--;
	}
}

void strcpy(char *dest, char *src)
{
	while(*src) {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = 0;
}

int strcmp(char *src1, char *src2)
{
	char *s1 = src1;
	char *s2 = src2;
	int r = 0;

	while (*s1 && *s2 && r == 0) {
		if (*s1 < *s2) {
			r = -1;
		}
		else if (*s1 > *s2) {
			r = 1;
		}
		s1++;
		s2++;
	}

	if (r == 0) {
		if (*s1 == 0) {
			if (*s2 != 0) {
				r = -1;
			}
		}
		else {
			r = 1;
		}
	}
	
	return r;
}

uint32_t strlen(char *s)
{
	uint32_t i = 0;
	
	while (*s) {
		i++;
		s++;
	}
	
	return i;
}


void interrupt_enable(void)
{
	__asm__ __volatile__("sti"
		:
		:
		);
}

void interrupt_disable(void)
{
	__asm__ __volatile__("cli"
		:
		:
		);
}

