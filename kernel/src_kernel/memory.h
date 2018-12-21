#ifndef _KERNEL_HEADER_MEMORY_H_
#define _KERNEL_HEADER_MEMORY_H_

#include "config.h"
#include "lib.h"

#define MEMORY_PHY_FRAME_TYPE_USER_SPACE     0
#define MEMORY_PHY_FRAME_TYPE_VM_TABLE       1

struct phy_memory_frame_t
{
	/*
		 pid: 0 if kernel, 1 if free, otherwise the pid of the process
	*/
	uint32_t pid;
	uint8_t type;
	void *phy_addr;

	union {
		struct {
			uint64_t vaddr;
			uint8_t flags;
		} user_space;
	} data;
};

typedef struct phy_memory_frame_t phy_memory_frame_t;

/*******************************/

#define VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_DENIED          0x00
#define VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD      0x01

union x86_64_virtual_memory_pg_el {
	struct {
		uint64_t p: 1;
		uint64_t rw: 1;
		uint64_t us: 1;
		uint64_t pwt: 1;
		uint64_t pcd: 1;
		uint64_t a: 1;
		uint64_t d: 1;
		uint64_t pat: 1;
		uint64_t g: 1;
		uint64_t avl: 3;
		uint64_t addr: 40;
		uint64_t available: 11;
		uint64_t nx: 1;
	} mask;

	uint64_t data;
};

typedef union x86_64_virtual_memory_pg_el x86_64_virtual_memory_pg_el;

/*******************************/

#define MEMORY_FLAG_WRITE       0x01
#define MEMORY_FLAG_EXECUTE     0x02

struct process_t;

void initialize_memory();
phy_memory_frame_t* memory_allocate_page(uint32_t pid);
void* memory_allocate_page_to_virtual_table(uint32_t pid);
void* memory_allocate_page_to_user_space(uint32_t pid, uint64_t vaddr, uint8_t flags);
x86_64_virtual_memory_pg_el* memory_initialize_table(uint32_t pid);
void memory_copy_virtual_memory_space(struct process_t *p_dest, struct process_t *p_src);
void memory_page_table_initialize(x86_64_virtual_memory_pg_el *tb);
void* memory_create_page(struct process_t *process, uint64_t vaddr, uint8_t flags);
uint8_t memory_create_n_pages(struct process_t *process, uint64_t vaddr_start, uint64_t vaddr_end, uint8_t flags);
void memory_setup_page_for_write(x86_64_virtual_memory_pg_el *tb);
void memory_setup_page_for_execute(x86_64_virtual_memory_pg_el *tb);
void memory_enable_paging();
void* memory_get_physical_addr_from_user_space(struct process_t *process, uint64_t vaddr); // return NULL if address can't be translated
uint8_t memory_copy_data_from_user_space(struct process_t *process, uint64_t vaddr, uint64_t len, void *buffer); // returns 0 if ok, otherwise anything else
uint8_t memory_copy_data_to_user_space(struct process_t *process, uint64_t vaddr, uint64_t len, void *buffer); // returns 0 if ok, otherwise anything else
void memory_copy_virtual_memory_space(struct process_t *p_dest, struct process_t *p_src);
void memory_free_process(struct process_t *process);

#endif
