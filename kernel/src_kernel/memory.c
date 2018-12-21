#include "memory.h"
#include "vars.h"
#include "process.h"
#include "printk.h"

phy_memory_frame_t *phy_frames;
uint64_t *begin_of_page_tables;
uint32_t n_phy_frames;
uint32_t n_phy_frames_kernel;
uint32_t n_phy_frames_virtual_tables;

static uint32_t phy_last_allocated;

uint64_t kernel_memory;

void initialize_memory()
{
	uint32_t i;

	KERNEL_ASSERT(sizeof(x86_64_virtual_memory_pg_el) == 8);

	phy_frames = (phy_memory_frame_t*)(kernel_stack_base_last_addr + 1);
	n_phy_frames = kernel_physical_memory_usable_size >> 12;

	kernel_memory = (uint64_t)phy_frames + n_phy_frames*sizeof(phy_memory_frame_t) + 4096;
	kernel_memory &= 0xFFFFFFFFFFFFF000LLU;

	n_phy_frames_kernel = kernel_memory >> 12;

	printk("There are %u physical memory frames, the first %u belongs to kernel\n", n_phy_frames, n_phy_frames_kernel);
	printk("Memory used by kernel is %llu bytes\n", kernel_memory);

	for (i=0; i<n_phy_frames_kernel; i++) {
		phy_frames[i].pid = 0;
		phy_frames[i].phy_addr = (void*)(i << 12);
	}

	for (i=n_phy_frames_kernel; i<n_phy_frames; i++) {
		phy_frames[i].pid = 1;
		phy_frames[i].phy_addr = (void*)(i << 12);
	}
	
	phy_last_allocated = n_phy_frames_kernel - 1;

	printk("Physical Frames table address from %llu to %llu\n", phy_frames, (uint64_t)(phy_frames + n_phy_frames) - 1);

	begin_of_page_tables = (uint64_t*)kernel_memory;
	n_phy_frames_virtual_tables = 0;
}

phy_memory_frame_t* memory_allocate_page(uint32_t pid)
{
	uint32_t i, j;

	j = phy_last_allocated + 1;
	for (i=0; i<n_phy_frames; i++) {
		if (phy_frames[j].pid == 1) {
			phy_frames[j].pid = pid;
			phy_last_allocated = j;
			//printk("Physical Frame %u (address= %u) allocated to process %u\n", i, i << 12, pid);
			return phy_frames+j;
		}
		j++;
		if (j == n_phy_frames)
			j = 0;
	}

	return NULL;
}

void memory_copy_virtual_memory_space(struct process_t *p_dest, struct process_t *p_src)
{
	uint32_t i;
	void *ptr;
	phy_memory_frame_t *frame;

	for (i=0; i<n_phy_frames; i++) {
		frame = phy_frames+i;
		if (frame->pid == p_src->pid && frame->type == MEMORY_PHY_FRAME_TYPE_USER_SPACE) {
			ptr = memory_create_page(p_dest, frame->data.user_space.vaddr, frame->data.user_space.flags);
			memcpy(ptr, frame->phy_addr, CONFIG_KERNEL_PAGE_SIZE);
		}
	}
}

void memory_free_process(struct process_t *process)
{
	uint32_t i;

	phy_memory_frame_t *frame;

	for (i=0; i<n_phy_frames; i++) {
		frame = phy_frames+i;
		if (frame->pid == process->pid) {
			frame->pid = 1;
		}
	}
}

void* memory_allocate_page_to_virtual_table(uint32_t pid)
{
	phy_memory_frame_t *frame;

	frame = memory_allocate_page(pid);
	KERNEL_ASSERT(frame != NULL);

	frame->type = MEMORY_PHY_FRAME_TYPE_VM_TABLE;
	return frame->phy_addr;
}

void* memory_allocate_page_to_user_space(uint32_t pid, uint64_t vaddr, uint8_t flags)
{
	phy_memory_frame_t *frame;

	frame = memory_allocate_page(pid);
	KERNEL_ASSERT(frame != NULL);

	frame->type = MEMORY_PHY_FRAME_TYPE_USER_SPACE;
	frame->data.user_space.vaddr = vaddr;
	frame->data.user_space.flags = flags;

	return frame->phy_addr;
}

x86_64_virtual_memory_pg_el* memory_initialize_table(uint32_t pid)
{
	x86_64_virtual_memory_pg_el *tb;

	tb = memory_allocate_page_to_virtual_table(pid);
	KERNEL_ASSERT(tb != NULL);

	memory_page_table_initialize(tb);

	return tb;
}

void memory_page_table_initialize(x86_64_virtual_memory_pg_el *tb)
{
	uint16_t i;

	for (i=0; i<CONFIG_KERNEL_VIRTUAL_MEMORY_ENTRIES_PER_TB; i++) {
		tb[i].data = 0;
		tb[i].mask.nx = 1; // no execution
	}
}

void* memory_create_page(struct process_t *process, uint64_t vaddr, uint8_t flags)
{
	uint16_t index;
	x86_64_virtual_memory_pg_el *tb1, *tb2, *tb3, *tb4;
	void *page_phy_addr;

	vaddr &= 0xFFFFFFFFFFFFF000LLU;

	tb1 = process->virtual_memory_table;

	// level 1

	index = (vaddr >> 39) & 0x01FF;
	tb1 += index;

	if (tb1->mask.p) {
		tb2 = (x86_64_virtual_memory_pg_el*)(tb1->mask.addr << 12);
	}
	else {
		tb2 = (x86_64_virtual_memory_pg_el*)memory_allocate_page_to_virtual_table(process->pid);
		KERNEL_ASSERT(tb2 != NULL);
		tb1->mask.addr = (uint64_t)tb2 >> 12;
		tb1->mask.p = 1;
		tb1->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;
		memory_page_table_initialize(tb2);
	}

	// level 2

	index = (vaddr >> 30) & 0x01FF;
	tb2 += index;

	if (tb2->mask.p) {
		tb3 = (x86_64_virtual_memory_pg_el*)(tb2->mask.addr << 12);
	}
	else {
		tb3 = (x86_64_virtual_memory_pg_el*)memory_allocate_page_to_virtual_table(process->pid);
		KERNEL_ASSERT(tb3 != NULL);
		tb2->mask.addr = (uint64_t)tb3 >> 12;
		tb2->mask.p = 1;
		tb2->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;
		memory_page_table_initialize(tb3);
	}

	// level 3

	index = (vaddr >> 21) & 0x01FF;
	tb3 += index;

	if (tb3->mask.p) {
		tb4 = (x86_64_virtual_memory_pg_el*)(tb3->mask.addr << 12);
	}
	else {
		tb4 = (x86_64_virtual_memory_pg_el*)memory_allocate_page_to_virtual_table(process->pid);
		KERNEL_ASSERT(tb4 != NULL);
		tb3->mask.addr = (uint64_t)tb4 >> 12;
		tb3->mask.p = 1;
		tb3->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;
		memory_page_table_initialize(tb4);
	}

	// level 4

	index = (vaddr >> 12) & 0x01FF;
	tb4 += index;

	// here, the page can't exist, since we are trying to create it
	KERNEL_ASSERT(tb4->mask.p == 0);

	page_phy_addr = memory_allocate_page_to_user_space(process->pid, vaddr, flags);
	KERNEL_ASSERT(page_phy_addr != NULL);
	tb4->mask.addr = (uint64_t)page_phy_addr >> 12;
	tb4->mask.p = 1;
	tb4->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;

	if (flags & MEMORY_FLAG_WRITE)
		memory_setup_page_for_write(tb4);
	if (flags & MEMORY_FLAG_EXECUTE)
		memory_setup_page_for_execute(tb4);

	printk("Page at address %llu (vaddr=0x%llX) allocated to pid %u\n", page_phy_addr, vaddr, process->pid);
	
	return page_phy_addr;
}

uint8_t memory_create_n_pages(struct process_t *process, uint64_t vaddr_start, uint64_t vaddr_end, uint8_t flags)
{
	uint16_t index;
	x86_64_virtual_memory_pg_el *tb1, *tb2, *tb3, *tb4;
	void *page_phy_addr;
	uint64_t vaddr;
	
	vaddr_start &= 0xFFFFFFFFFFFFF000LLU;
	vaddr_end |= 0x0FFF;
	vaddr = vaddr_start;

	while (vaddr < vaddr_end) {
		tb1 = process->virtual_memory_table;

		// level 1

		index = (vaddr >> 39) & 0x01FF;
		tb1 += index;

		if (tb1->mask.p) {
			tb2 = (x86_64_virtual_memory_pg_el*)(tb1->mask.addr << 12);
		}
		else {
			tb2 = (x86_64_virtual_memory_pg_el*)memory_allocate_page_to_virtual_table(process->pid);
			KERNEL_ASSERT(tb2 != NULL);
			tb1->mask.addr = (uint64_t)tb2 >> 12;
			tb1->mask.p = 1;
			tb1->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;
			memory_page_table_initialize(tb2);
		}

		// level 2

		index = (vaddr >> 30) & 0x01FF;
		tb2 += index;

		if (tb2->mask.p) {
			tb3 = (x86_64_virtual_memory_pg_el*)(tb2->mask.addr << 12);
		}
		else {
			tb3 = (x86_64_virtual_memory_pg_el*)memory_allocate_page_to_virtual_table(process->pid);
			KERNEL_ASSERT(tb3 != NULL);
			tb2->mask.addr = (uint64_t)tb3 >> 12;
			tb2->mask.p = 1;
			tb2->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;
			memory_page_table_initialize(tb3);
		}

		// level 3

		index = (vaddr >> 21) & 0x01FF;
		tb3 += index;

		if (tb3->mask.p) {
			tb4 = (x86_64_virtual_memory_pg_el*)(tb3->mask.addr << 12);
		}
		else {
			tb4 = (x86_64_virtual_memory_pg_el*)memory_allocate_page_to_virtual_table(process->pid);
			KERNEL_ASSERT(tb4 != NULL);
			tb3->mask.addr = (uint64_t)tb4 >> 12;
			tb3->mask.p = 1;
			tb3->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;
			memory_page_table_initialize(tb4);
		}

		// level 4

		index = (vaddr >> 12) & 0x01FF;
		tb4 += index;
		
		while (vaddr < vaddr_end && index <= 0x01FF) {
			// here, the page can't exist, since we are trying to create it
			KERNEL_ASSERT(tb4->mask.p == 0);

			page_phy_addr = memory_allocate_page_to_user_space(process->pid, vaddr, flags);
			KERNEL_ASSERT(page_phy_addr != NULL);
			tb4->mask.addr = (uint64_t)page_phy_addr >> 12;
			tb4->mask.p = 1;
			tb4->mask.avl |= VIRTUAL_MEMORY_TB_EL_AVL_PROCESS_GREEN_CARD;

			if (flags & MEMORY_FLAG_WRITE)
				memory_setup_page_for_write(tb4);
			if (flags & MEMORY_FLAG_EXECUTE)
				memory_setup_page_for_execute(tb4);

//			printk("Page at address %llu (vaddr=0x%llX) allocated to pid %u\n", page_phy_addr, vaddr, process->pid);
			
			vaddr += 4096;
			index++;
			tb4++;
		}
	}
	
	return 1;
}

void memory_setup_page_for_write(x86_64_virtual_memory_pg_el *tb)
{
	tb->mask.rw = 1;
}

void memory_setup_page_for_execute(x86_64_virtual_memory_pg_el *tb)
{
	tb->mask.nx = 0;
}

union x86_64_virtual_memory_cfg {
	struct {
		uint64_t base_addr: 40; // address is aligned to 12 bits
		uint64_t paging_enable: 1;
	} mask;

	uint64_t data;
};

void memory_enable_paging()
{
	union x86_64_virtual_memory_cfg mem_cfg;

	mem_cfg.data = read_msr_64(CONFIG_KERNEL_MSR_VIRTUAL_MEMORY_CFG);
	mem_cfg.mask.paging_enable = 1;
	write_msr_64(CONFIG_KERNEL_MSR_VIRTUAL_MEMORY_CFG, mem_cfg.data);
}

void memory_setup_page_base_addr(process_t *process)
{
	union x86_64_virtual_memory_cfg mem_cfg;

	mem_cfg.data = read_msr_64(CONFIG_KERNEL_MSR_VIRTUAL_MEMORY_CFG);
	mem_cfg.mask.base_addr = (uint64_t)process->virtual_memory_table >> 12;
	write_msr_64(CONFIG_KERNEL_MSR_VIRTUAL_MEMORY_CFG, mem_cfg.data);
}

void* memory_get_physical_addr_from_user_space(struct process_t *process, uint64_t vaddr)
{
	uint16_t index;
	x86_64_virtual_memory_pg_el *tb1, *tb2, *tb3, *tb4;
	
	//printk("translating vaddr 0x%llX of process %s\n", vaddr, process->fname);

	tb1 = process->virtual_memory_table;

	// level 1

	index = (vaddr >> 39) & 0x01FF;
	tb1 += index;

	if (tb1->mask.p) {
		tb2 = (x86_64_virtual_memory_pg_el*)(tb1->mask.addr << 12);
	}
	else {
		return NULL;
	}

	// level 2

	index = (vaddr >> 30) & 0x01FF;
	tb2 += index;

	if (tb2->mask.p) {
		tb3 = (x86_64_virtual_memory_pg_el*)(tb2->mask.addr << 12);
	}
	else {
		return NULL;
	}

	// level 3

	index = (vaddr >> 21) & 0x01FF;
	tb3 += index;

	if (tb3->mask.p) {
		tb4 = (x86_64_virtual_memory_pg_el*)(tb3->mask.addr << 12);
	}
	else {
		return NULL;
	}

	// level 4

	index = (vaddr >> 12) & 0x01FF;
	tb4 += index;

	if (tb4->mask.p) {
		uint64_t r;
		r = tb4->mask.addr << 12;
		r |= vaddr & 0x0FFF;
		return (void*)r;
	}
	else {
		return NULL;
	}
}

uint8_t memory_copy_data_from_user_space(struct process_t *process, uint64_t vaddr, uint64_t len, void *buffer)
{
	uint8_t *kb;
	uint64_t va, last_va;
	uint8_t *pb;
	uint64_t i;
	
	va= vaddr;
	last_va = va;
	kb = (uint8_t*)buffer;
	
	pb = memory_get_physical_addr_from_user_space(process, va);
	if (pb == NULL)
		return 1;
	
	for (i=0; i<len; i++) {
		// check if the page changed
		// if it does changed, re-calculate physical address
		if ((last_va & 0xFFFFFFFFFFFFF000LLU) != (va & 0xFFFFFFFFFFFFF000LLU)) {
			pb = memory_get_physical_addr_from_user_space(process, va);
			if (pb == NULL)
				return 1;
		}
		
		*kb = *pb;
		
		last_va = va;
		va++;
		
		kb++;
		pb++;
	}
	
	return 0;
}

uint8_t memory_copy_data_to_user_space(struct process_t *process, uint64_t vaddr, uint64_t len, void *buffer)
{
	uint8_t *kb;
	uint64_t va, last_va;
	uint8_t *pb;
	uint64_t i;
	
	va= vaddr;
	last_va = va;
	kb = (uint8_t*)buffer;
	
	pb = memory_get_physical_addr_from_user_space(process, va);
	if (pb == NULL)
		return 1;
	
	for (i=0; i<len; i++) {
		// check if the page changed
		// if it does changed, re-calculate physical address
		if ((last_va & 0xFFFFFFFFFFFFF000LLU) != (va & 0xFFFFFFFFFFFFF000LLU)) {
			pb = memory_get_physical_addr_from_user_space(process, va);
			if (pb == NULL)
				return 1;
		}
		
		*pb = *kb;
		
		last_va = va;
		va++;
		
		kb++;
		pb++;
	}
	
	return 0;
}









