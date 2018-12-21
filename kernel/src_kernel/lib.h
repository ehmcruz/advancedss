#ifndef _KERNEL_HEADER_LIBRARY_H_
#define _KERNEL_HEADER_LIBRARY_H_

#include "config.h"

typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;

#define NULL 0

uint8_t inportb(uint16_t port);
void outportb(uint16_t port, uint8_t val);
uint32_t inportd(uint16_t port);
void outportd(uint16_t port, uint32_t val);
uint32_t get_cpuid();

void* get_kernel_stack_base_for_current_cpu();

uint64_t get_virtual_address_fault();

uint64_t read_msr_64(uint32_t code);
uint32_t read_msr_32(uint32_t code);

void write_msr_64(uint32_t code, uint64_t value);
void write_msr_32(uint32_t code, uint32_t value);

uint32_t get_number_of_cpus();

void interrupt_disable(void);
void interrupt_enable(void);

int strcmp(char *src1, char *src2);
void strcpy(char *dest, char *src);
uint32_t strlen(char *s);

void memcpy(void *dest, void *src, int64_t len);

//void halt();

#define halt() 	__asm__ __volatile__("hlt\n")

void panic();

#define KERNEL_ASSERT(V)  \
	if (!(V)) { \
		printk("File %s at line %u assertion failed!\n%s\n", __FILE__, __LINE__, #V); \
		panic(); \
	}

#endif
