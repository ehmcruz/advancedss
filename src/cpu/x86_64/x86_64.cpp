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

#include "../cpu.h"
#include "../../io/io.h"
#include "../../host_code/host.h"

//#define X86_64_COMPARE_RESULTS

#ifdef X86_64_COMPARE_RESULTS
	FILE *fp_x86_64_reg_data;
#endif

using namespace cpu;

/*

*/

#ifdef DEBUG_CPU
	char* cpu::x86_64_get_gpr64_name(uint8_t code)
	{
		static char *list[] = {
			"rax",
			"rcx",
			"rdx",
			"rbx",
			"rsp",
			"rbp",
			"rsi",
			"rdi",
			"r8",
			"r9",
			"r10",
			"r11",
			"r12",
			"r13",
			"r14",
			"r15"
		};

		return list[code];
	}

	char* cpu::x86_64_get_gpr32_name(uint8_t code)
	{
		static char *list[] = {
			"eax",
			"ecx",
			"edx",
			"ebx",
			"esp",
			"ebp",
			"esi",
			"edi",
			"r8d",
			"r9d",
			"r10d",
			"r11d",
			"r12d",
			"r13d",
			"r14d",
			"r15d"
		};

		return list[code];
	}

	char* cpu::x86_64_get_gpr16_name(uint8_t code)
	{
		static char *list[] = {
			"ax",
			"cx",
			"dx",
			"bx",
			"sp",
			"bp",
			"si",
			"di",
			"r8w",
			"r9w",
			"r10w",
			"r11w",
			"r12w",
			"r13w",
			"r14w",
			"r15w"
		};

		return list[code];
	}

	char* cpu::x86_64_get_gpr8_name(uint8_t code, uint8_t rex)
	{
		static char *list[] = {
			"al",
			"cl",
			"dl",
			"bl",
			"ah",
			"ch",
			"dh",
			"bh"
		};

		static char *list_rex[] = {
			"al",
			"cl",
			"dl",
			"bl",
			"spl",
			"bpl",
			"sil",
			"dil",
			"r8b",
			"r9b",
			"r10b",
			"r11b",
			"r12b",
			"r13b",
			"r14b",
			"r15b"
		};

		return ((rex & 0xF0) == 0x40) ? list_rex[code]: list[code];
	}

	char* cpu::x86_64_get_rflags_name(uint8_t code)
	{
		static char *list[] = {
			"cf",
			"[reserved bit 1]",
			"pf",
			"[reserved bit 3]",
			"af",
			"[reserved bit 5]",
			"zf",
			"sf",
			"[reserved bit 8]",
			"[reserved bit 9]",
			"df",
			"of",
			"[reserved bit 12]",
			"[reserved bit 13]",
			"[reserved bit 14]",
			"[reserved bit 15]",
			"[reserved bit 16]",
			"[reserved bit 17]",
			"[reserved bit 18]",
			"[reserved bit 19]",
			"[reserved bit 20]",
			"[reserved bit 21]",
			"[reserved bit 22]",
			"[reserved bit 23]",
			"[reserved bit 24]",
			"[reserved bit 25]",
			"[reserved bit 26]",
			"[reserved bit 27]",
			"[reserved bit 28]",
			"[reserved bit 29]",
			"[reserved bit 30]",
			"[reserved bit 31]"
		};

		return list[code];
	}
#endif


#ifdef X86_64_FAKE_SYSCALLS
	/*
		memory:
		-------------- end --------------
		video buffer
		stack
		program dinamic memory
		program static memory
		-------------- start ------------
	*/

	struct __attribute__ ((__packed__)) process_elf64_header {
		uint8_t ident[16];
		uint16_t type;
		uint16_t machine;
		uint32_t version;
		uint64_t entry;
		uint64_t phoff;
		uint64_t shoff;
		uint32_t flags;
		uint16_t ehsize;
		uint16_t phentsize;
		uint16_t phnum;
		uint16_t shentsize;
		uint16_t shnnum;
		uint16_t shstrndx;
	};

	struct __attribute__ ((__packed__)) process_elf64_program_header {
		uint32_t type;
		uint32_t flags;
		uint64_t offset;
		uint64_t vaddr;
		uint64_t paddr;
		uint64_t filesz;
		uint64_t memsz;
		uint64_t align;
	};

	/********************************************************************/

	static uint32_t x86_64_video_buffer_addr;
	static uint32_t x86_64_video_buffer_width;
	static uint32_t x86_64_video_buffer_height;
	static uint32_t x86_64_video_buffer_cursor_x;
	static uint32_t x86_64_video_buffer_cursor_y;
	static uint64_t x86_64_virtual_memory_heap_end;

	struct x86_64_fake_syscall_openned_files {
		char name[50]; // file name
		FILE *handler;
	};

	#define SYSCALL_STDIN     0
	#define SYSCALL_STDOUT    1
	#define SYSCALL_MAX_OPENNED_FILES    100

	x86_64_fake_syscall_openned_files openned_files[SYSCALL_MAX_OPENNED_FILES];

	/********************************************************************/

	#define PROCESS_ELF64_PROGRAM_HEADER_TYPE_LOAD     0x01

	void cpu::processor_t::read_elf64_header(uint32_t boot_size)
	{
		process_elf64_header *elf_header;
		process_elf64_program_header *program_header;
		uint32_t i;
		uint64_t last_addr;

		elf_header = (process_elf64_header*)this->memory->get_vector();
		program_header = (process_elf64_program_header*)((uint8_t*)this->memory->get_vector() + elf_header->phoff);
		x86_64_virtual_memory_heap_end = 0;

//		SANITY_ASSERT(elf_header->phnum == 1 && program_header->type == PROCESS_ELF64_PROGRAM_HEADER_TYPE_LOAD);

		last_addr = program_header->vaddr + program_header->memsz - 1;
		if (last_addr > x86_64_virtual_memory_heap_end) {
			x86_64_virtual_memory_heap_end = last_addr;
		}

		memset((uint8_t*)this->memory->get_vector()+program_header->filesz, 0, this->memory->get_size() - (program_header->filesz));

/*static char msg[200];
sprintf(msg, "heap end is 0x%llX\n", x86_64_virtual_memory_heap_end);
video_print_string(msg);*/
		/* don't know why, but linux apparently does this */
		x86_64_virtual_memory_heap_end |= 0x0FFF;
	}

	void cpu::processor_t::video_initialize(uint32_t width, uint32_t height)
	{
		uint32_t i;

		this->io_outd(IO_PORTS_WRITE_VIDEO_WIDTH, width);
		this->io_outd(IO_PORTS_WRITE_VIDEO_HEIGHT, height);
		this->io_outb(IO_PORTS_WRITE_VIDEO_CMD, IO_VIDEO_CMD_INIT);

		x86_64_video_buffer_addr = this->memory->get_size() - (width*height + 10);
		x86_64_video_buffer_width = width;
		x86_64_video_buffer_height = height;
		x86_64_video_buffer_cursor_x = 0;
		x86_64_video_buffer_cursor_y = 0;

		this->io_outd(IO_PORTS_WRITE_VIDEO_SET_BUFFER, x86_64_video_buffer_addr);

		for (i=0; i<width*height; i++)
			this->memory->write1(x86_64_video_buffer_addr+i, ' ');

		this->io_outb(IO_PORTS_WRITE_VIDEO_CMD, IO_VIDEO_CMD_REFRESH);
	}

	void cpu::processor_t::video_remove_line_from_buffer()
	{
		uint32_t i, j;
		volatile uint8_t *video_buffer = (volatile uint8_t*)this->memory->get_vector() + x86_64_video_buffer_addr;

		for (j=0; j<x86_64_video_buffer_width; j++) {
			for (i=0; i<x86_64_video_buffer_height-1; i++) {
				video_buffer[i*x86_64_video_buffer_width + j] = video_buffer[(i+1)*x86_64_video_buffer_width + j];
			}
		}

		i = (x86_64_video_buffer_height - 1) * x86_64_video_buffer_width;
		for (j=0; j<x86_64_video_buffer_width; j++) {
			video_buffer[i + j] = ' ';
		}
	}

	void cpu::processor_t::video_print_string(char *s)
	{
		char *b;
		uint32_t i, z;
		volatile uint8_t *video_buffer = (volatile uint8_t*)this->memory->get_vector() + x86_64_video_buffer_addr;

		for (b=s; *b; b++) {
			if (x86_64_video_buffer_cursor_y == x86_64_video_buffer_height) {
				video_remove_line_from_buffer();
				x86_64_video_buffer_cursor_y--;
			}

			z = x86_64_video_buffer_cursor_x + x86_64_video_buffer_cursor_y*x86_64_video_buffer_width;

			if (*b == 10) { // newline
				for (i=x86_64_video_buffer_cursor_x; i<x86_64_video_buffer_width; i++)
					video_buffer[z++] = ' ';
				x86_64_video_buffer_cursor_x = 0;
				x86_64_video_buffer_cursor_y++;
			}
			else if (*b == 13) { // return to line start
				z = x86_64_video_buffer_cursor_y*x86_64_video_buffer_width;
				for (i=0; i<x86_64_video_buffer_width; i++)
					video_buffer[z++] = ' ';
				x86_64_video_buffer_cursor_x = 0;
			}
			else {
				video_buffer[z++] = *b;

				x86_64_video_buffer_cursor_x++;
				if (x86_64_video_buffer_cursor_x == x86_64_video_buffer_width) {
					x86_64_video_buffer_cursor_x = 0;
					x86_64_video_buffer_cursor_y++;
				}
			}
		}

		this->io_outb(IO_PORTS_WRITE_VIDEO_CMD, IO_VIDEO_CMD_REFRESH);
		this->io_outd(IO_PORTS_WRITE_VIDEO_CURSOR_X, x86_64_video_buffer_cursor_x);
		this->io_outd(IO_PORTS_WRITE_VIDEO_CURSOR_Y, x86_64_video_buffer_cursor_y);
		this->io_outd(IO_PORTS_WRITE_VIDEO_CMD, IO_VIDEO_CMD_SET_CURSOR_POS);
	}

	void cpu::processor_t::process_fake_syscall()
	{
/*{static char msg[200];
sprintf(msg, "syscall %llu occured rip=0x"PRINTF_INT64X_PREFIX"  rdi=%lli   rsi=%lli   rdx=%lli\n", this->read_gen_reg_64(BX_64BIT_REG_RAX), this->get_pc(), this->read_gen_reg_64(BX_64BIT_REG_RDI), this->read_gen_reg_64(BX_64BIT_REG_RSI), this->read_gen_reg_64(BX_64BIT_REG_RDX));
video_print_string(msg);}*///exit(1);
		switch (this->read_gen_reg_64(BX_64BIT_REG_RAX)) {
			case 0: { // read
				uint64_t file;
				uint64_t len;
				char *buffer;
				uint64_t n_read;

				buffer = (char*)this->memory->get_vector() + this->read_gen_reg_64(BX_64BIT_REG_RSI);
				file = this->read_gen_reg_64(BX_64BIT_REG_RDI);
				len = this->read_gen_reg_64(BX_64BIT_REG_RDX);

				if (file == SYSCALL_STDIN) {
					host_sim_keyboard_read_line(buffer, len);
					n_read = strlen(buffer);
					video_print_string(buffer);
/*{static char msg[200];
sprintf(msg, "%llu chars typed\n", n_read);
video_print_string(msg);}*/
				}
				else if (file < 3)
					n_read = 0;
				else {
					FILE *fp;

					fp = openned_files[file-3].handler;
					if (!fp)
						n_read = 0;
					else
						n_read = fread(buffer, 1, len, fp);
				}

				this->write_gen_reg_64(BX_64BIT_REG_RAX, n_read);
			}
			break;

			case 1: { // write
				uint64_t file;
				uint64_t len;
				char *buffer;
				uint64_t n_write;

				buffer = (char*)this->memory->get_vector() + this->read_gen_reg_64(BX_64BIT_REG_RSI);
				file = this->read_gen_reg_64(BX_64BIT_REG_RDI);
				len = this->read_gen_reg_64(BX_64BIT_REG_RDX);

				if (file == SYSCALL_STDOUT) {
					static char msg[500];

					SANITY_ASSERT(len <= 499);
					SANITY_ASSERT(this->read_gen_reg_64(BX_64BIT_REG_RSI) < this->memory->get_size());
					memcpy(msg, buffer, len);
					msg[len] = 0;
					video_print_string(msg);
					n_write = len;
				}
				else if (file < 3)
					n_write = 0;
				else {
					FILE *fp;

					fp = openned_files[file-3].handler;
					if (!fp)
						n_write = 0;
					else
						n_write = fwrite(buffer, 1, len, fp);
				}

				this->write_gen_reg_64(BX_64BIT_REG_RAX, n_write);
			}
			break;

			case 2: { // open
				char *fname;
				uint32_t i;

				for (i=0; i<SYSCALL_MAX_OPENNED_FILES; i++) {
					if (openned_files[i].handler == NULL) {
						fname = (char*)this->memory->get_vector() + this->read_gen_reg_64(BX_64BIT_REG_RDI);
						strcpy(openned_files[i].name, fname);
						openned_files[i].handler = fopen(fname, "r+b");
						if (!openned_files[i].handler) {
							this->write_gen_reg_64(BX_64BIT_REG_RAX, -1);
						}
						else {
							this->write_gen_reg_64(BX_64BIT_REG_RAX, i+3);
						}
/*{static char msg[200];
sprintf(msg, "file id is %llu\n", this->read_gen_reg_64(BX_64BIT_REG_RAX));
video_print_string(msg);}*/
						i = SYSCALL_MAX_OPENNED_FILES + 1; // go out of loop
					}
				}

				if (i == SYSCALL_MAX_OPENNED_FILES)
					this->write_gen_reg_64(BX_64BIT_REG_RAX, -1);
			}
			break;

			case 3: { // close
				uint64_t file;
				int64_t r;
				FILE *fp;

				file = this->read_gen_reg_64(BX_64BIT_REG_RDI);
				if (file >= 3) {
					if (openned_files[file-3].handler != NULL) {
						r = fclose(openned_files[file-3].handler);
						openned_files[file-3].handler = NULL;
						if (r != 0)
							r = -1;
					}
					else
						r = 1;
				}
				else
					r = -1;

				this->write_gen_reg_64(BX_64BIT_REG_RAX, r);
			}
			break;

			case 12: { // brk
				uint64_t new_brk;
				int64_t incr;

/*{static char msg[200];
sprintf(msg, "syscall %llu occured rip=0x"PRINTF_INT64X_PREFIX"  rdi=%lli\n", this->read_gen_reg_64(BX_64BIT_REG_RAX), this->get_pc(), this->read_gen_reg_64(BX_64BIT_REG_RDI));
video_print_string(msg);}*/

				new_brk = this->read_gen_reg_64(BX_64BIT_REG_RDI);
				incr = new_brk - x86_64_virtual_memory_heap_end - 1;

				if (new_brk != 0) {
					x86_64_virtual_memory_heap_end += incr;
				}

				this->write_gen_reg_64(BX_64BIT_REG_RAX, x86_64_virtual_memory_heap_end+1);
/*{static char msg[200];
sprintf(msg, "brk response is 0x%llX (%llu)\n", this->read_gen_reg_64(BX_64BIT_REG_RAX), this->read_gen_reg_64(BX_64BIT_REG_RAX));
video_print_string(msg);}*/
			}
			break;

			default: {
				static char msg[200];
				sprintf(msg, "invalid syscall "PRINTF_UINT64_PREFIX" occured rip=0x"PRINTF_INT64X_PREFIX"\n", this->read_gen_reg_64(BX_64BIT_REG_RAX), this->get_pc());
				video_print_string(msg);
				SANITY_ASSERT(0);
			}
		}
	}
#endif // fake syscalls

void cpu::processor_t::initialize_arch(uint32_t boot_size)
{
	uint32_t i;

	this->set_number_dep_vectors(5);

	this->setup_dep_vector(CPU_X8664_DEP_VECTOR_GPR_BITS_0_7, BX_GENERAL_REGISTERS);
	this->setup_dep_vector(CPU_X8664_DEP_VECTOR_GPR_BITS_8_15, BX_GENERAL_REGISTERS);
	this->setup_dep_vector(CPU_X8664_DEP_VECTOR_GPR_BITS_16_63, BX_GENERAL_REGISTERS);
	this->setup_dep_vector(CPU_X8664_DEP_VECTOR_RFLAGS, 32);
	this->setup_dep_vector(CPU_X8664_DEP_VECTOR_MSR, CPU_X8664_NUMBER_MSR);

	this->write_model_specific_reg(CPU_X8664_MSR_NUMBER_PHYSICAL_CPU, advancedss_get_number_phy_cpu());
	this->write_model_specific_reg(CPU_X8664_MSR_NUMBER_VIRTUAL_CPU_PER_PHY, advancedss_get_number_virtual_cpu_per_phy_cpu());
	this->write_model_specific_reg(CPU_X8664_MSR_CPU_ID, this->get_id());
	this->write_model_specific_reg(CPU_X8664_MSR_PHYSICAL_MEMORY_SIZE, this->memory->get_size());

	for (i=0; i<BX_GENERAL_REGISTERS; i++) {
		this->write_gen_reg_64(i, 0);
	}
	
	BX_WRITE_64BIT_REG(BX_NIL_REGISTER, 0);

	this->write_gen_reg_64(BX_64BIT_REG_RAX, boot_size);

	this->setEFlags(0);
	this->cpl = CPU_X8664_CPL_KERNEL;
	this->interrupt_enabled = 0;

	this->msr_virtual_memory_mask = (x86_64_virtual_memory_cfg*)&(this->model_specific_regs[CPU_X8664_MSR_VIRTUAL_MEMORY_CFG]);
	this->msr_virtual_memory_mask->mask.paging_enable = 0;
	
	this->flush_internal_caches();
	
	this->x86_64_statistic_vm_cache_access = 0;
	this->x86_64_statistic_vm_cache_hit = 0;
	this->x86_64_statistic_vm_cache_flush = 0;
	this->x86_64_statistic_cycle_kernel = 0;
	this->x86_64_statistic_cycle_user = 0;

	#ifdef SANITY_CHECK
		SANITY_ASSERT(sizeof(x86_64_virtual_memory_cfg) == 8);
		SANITY_ASSERT(sizeof(x86_64_virtual_memory_pg_el) == 8);
		SANITY_ASSERT(sizeof(x86_84_int_vector_el) == 8);
	#endif

	#ifdef X86_64_COMPARE_RESULTS
		fp_x86_64_reg_data = fopen("reg_trace.dat", "rb");
		if (!fp_x86_64_reg_data) {
			LOG_PRINTF("error loading reg_trace.dat\n");
			exit(1);
		}
	#endif

	#ifdef X86_64_FAKE_SYSCALLS
		this->video_initialize(64, 40);
		this->read_elf64_header(boot_size);
		this->write_gen_reg_64(BX_64BIT_REG_RSP, this->memory->get_size() - x86_64_video_buffer_width*x86_64_video_buffer_height - 100);

		for (i=0; i<SYSCALL_MAX_OPENNED_FILES; i++) {
			openned_files[i].handler = NULL;
		}
	#endif
}

void cpu::processor_t::bochs_exec(volatile bochs_exec_status_t *status)
{
	uint64_t next_rip;

	// status is already initialized with default values

	this->speculative_rsp = 0;
	this->prev_rip = this->get_pc();
	next_rip = this->get_pc() + status->bochs_inst->ilen();
	this->bochs_exec_status = status;

	if (!setjmp(this->longjmp_env)) {
		this->set_pc( next_rip );

		BX_CPU_CALL_METHOD((status->bochs_inst->execute), (status->bochs_inst)); // might iterate repeat instruction
	}
	else { // instruction exception
		this->set_pc(this->prev_rip);

		if (this->speculative_rsp) {
			RSP = this->prev_rsp;
		}
	}
}

void cpu::processor_t::exception(exception_code_t vector, Bit16u error_code, unsigned unused)
{
	#ifdef X86_64_FAKE_SYSCALLS
		static char msg[200];
		sprintf(msg, "exception %u occured rip=0x"PRINTF_INT64X_PREFIX"\n", (uint32_t)vector, this->get_pc());
		video_print_string(msg);
		SANITY_ASSERT(0);
	#endif

	this->bochs_exec_status->has_exception = 1;
	this->bochs_exec_status->exception_code = vector;
	longjmp(this->longjmp_env, 1);
}

void cpu::processor_t::flush_internal_caches()
{
	uint32_t i;
	this->decoder->x86_64_flush_inst_cache();
	for (i=0; i<CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_SIZE; i++) {
		this->x86_64_vm_cache[i].busy = 0;
		this->x86_64_vmi_cache[i].busy = 0;
	}
	this->x86_64_statistic_vm_cache_flush++;
}

#define VIRTUAL_MEMORY_IS_ENABLED \
	(this->cpl == CPU_X8664_CPL_USER && this->msr_virtual_memory_mask->mask.paging_enable)

void cpu::processor_t::get_physical_memory_address(x86_64_virtual_memory_data *r, x86_64_virtual_memory_pg_el_cache *from_cache)
{
	if (VIRTUAL_MEMORY_IS_ENABLED) {
		x86_64_virtual_memory_pg_el level_1;
		x86_64_virtual_memory_pg_el level_2;
		x86_64_virtual_memory_pg_el level_3;
		x86_64_virtual_memory_pg_el level_4;
		uint32_t base_addr, index, cache_index;
		x86_64_virtual_memory_pg_el_cache *cache;
			
		cache_index = CPU_X86_64_VIRTUAL_MEMORY_CACHE_TABLE_GET_INDEX(r->vaddr >> 12);
		cache = (from_cache == NULL) ? this->x86_64_vm_cache : from_cache;
		cache += cache_index;
		this->x86_64_statistic_vm_cache_access++;
		if (cache->busy == 1 && cache->vaddr == (r->vaddr >> 12)) {
			this->x86_64_statistic_vm_cache_hit++;
			// cache-hit! yes!
			r->page_el_addr = cache->el.page_el_addr;
			index = r->vaddr & 0x0FFF;
			r->paddr = (cache->el.page.mask.addr << 12) + index;
			r->present = 1;
			r->read = 1;
			r->write = cache->el.page.mask.rw;
			r->execute = !cache->el.page.mask.nx;

			//memcpy(&r->page, &level_4, sizeof(x86_64_virtual_memory_pg_el));
			r->page.data = cache->el.page.data;

			return;
		}
		
		cache->busy = 0;
		
		base_addr = this->msr_virtual_memory_mask->mask.base_addr << 12;
		index = ((r->vaddr >> 39) & 0x01FF) << 3;
		level_1.data = this->memory_read_8_bytes(base_addr + index);
		if (level_1.mask.p == 0) {
			r->present = 0;
			r->read = 1;
			r->write = level_1.mask.rw;
			r->execute = !level_1.mask.nx;
//if (!this->decoder->get_mis_speculation()) {SANITY_ASSERT(0);}
			return;
		}

		base_addr = level_1.mask.addr << 12;
		index = ((r->vaddr >> 30) & 0x01FF) << 3;
		level_2.data = this->memory_read_8_bytes(base_addr + index);
		if (level_2.mask.p == 0) {
			r->present = 0;
			r->read = 1;
			r->write = level_2.mask.rw;
			r->execute = !level_2.mask.nx;
//if (!this->decoder->get_mis_speculation()) {SANITY_ASSERT(0);}
			return;
		}

		base_addr = level_2.mask.addr << 12;
		index = ((r->vaddr >> 21) & 0x01FF) << 3;
		level_3.data = this->memory_read_8_bytes(base_addr + index);
		if (level_3.mask.p == 0) {
			r->present = 0;
			r->read = 1;
			r->write = level_3.mask.rw;
			r->execute = !level_3.mask.nx;
//if (!this->decoder->get_mis_speculation()) {SANITY_ASSERT(0);}
			return;
		}

		base_addr = level_3.mask.addr << 12;
		index = ((r->vaddr >> 12) & 0x01FF) << 3;
		level_4.data = this->memory_read_8_bytes(base_addr + index);
		if (level_4.mask.p == 0) {
			r->present = 0;
			r->read = 1;
			r->write = level_4.mask.rw;
			r->execute = !level_4.mask.nx;
//LOG_PRINTF("vddress=(0x"PRINTF_INT64X_PREFIX")\n", r->vaddr);
//if (!this->decoder->get_mis_speculation()) {SANITY_ASSERT(0);}
			return;
		}

		r->page_el_addr = base_addr + index;
		index = r->vaddr & 0x0FFF;
		r->paddr = (level_4.mask.addr << 12) + index;
		r->present = 1;
		r->read = 1;
		r->write = level_4.mask.rw;
		r->execute = !level_4.mask.nx;

		//memcpy(&r->page, &level_4, sizeof(x86_64_virtual_memory_pg_el));
		r->page.data = level_4.data;
		
		// put entry in cache
		memcpy(&(cache->el), r, sizeof(x86_64_virtual_memory_data));
		//vm_cache[cache_index].el.data = level_4.data;
		cache->vaddr = r->vaddr >> 12;
		cache->busy = 1;

	//	LOG_PRINTF("virtual address (0x"PRINTF_INT64X_PREFIX") taranslated to %u\n", r->vaddr, r->paddr);
		//SANITY_ASSERT(0);
	}
	else {
		r->paddr = r->vaddr;
		r->present = 1;
		r->read = 1;
		r->write = 1;
		r->execute = 1;
	}
}

void cpu::processor_t::get_idt_el(uint8_t c, x86_84_int_vector_el *el)
{
	uint32_t addr;

	addr = this->model_specific_regs[CPU_X8664_MSR_INTERRUPT_VECTOR_ADDR];
//LOG_PRINTF("idt base %u\n", addr);
//SANITY_ASSERT(0);
	addr += (uint32_t)c << 3; // 8 bytes per element

	el->data = this->memory_read_8_bytes(addr);
}

void cpu::processor_t::process_exception(exception_code_t c, target_addr_t inst_addr, uint8_t inst_length)
{
	#ifdef X86_64_FAKE_SYSCALLS
		static char msg[200];
		sprintf(msg, "exception %u occured rip=0x"PRINTF_INT64X_PREFIX"\n", (uint32_t)c, this->get_pc());
		video_print_string(msg);
		SANITY_ASSERT(0);
	#else
		uint8_t code;
		code = c & 0x1F;
		if (this->cpl == CPU_X8664_CPL_USER && this->interrupt_enabled) {
			x86_84_int_vector_el el;
			
			this->flush_internal_caches();

			this->get_idt_el(code, &el);

			if (code == BX_GP_EXCEPTION || code == BX_PF_EXCEPTION) {
				this->model_specific_regs[CPU_X8664_MSR_VIRTUAL_MEMORY_PAGE_FAULT_ADDR] = c & 0xFFFFFFFFFFFFF000LL;
			}
	//LOG_PRINTF("exception %u handler 0x%X\n", code, el.mask.handler_addr << 2);
	//SANITY_ASSERT(0);
			// handler address is aligned to 4 bytes
			this->go_to_kernel_mode(el.mask.handler_addr << 2);
		}
		else {
			LOG_PRINTF("exception %u occured when inside kernel mode or interrupts disabled! rip=0x"PRINTF_INT64X_PREFIX"\n", (uint32_t)code, this->get_pc());
			SANITY_ASSERT(0);
		}
	#endif
}

void cpu::processor_t::process_trap(trap_code_t c, target_addr_t inst_addr, uint8_t inst_length)
{
	this->set_pc(inst_addr + inst_length);

	#ifdef X86_64_FAKE_SYSCALLS
		if (c == 0xFFFF) { // syscall instruction
			this->flush_internal_caches();
			this->process_fake_syscall();
		}
		else { // int instruction
			static char msg[200];
			sprintf(msg, "int 0x%X occured rip=0x"PRINTF_INT64X_PREFIX"\n", c, this->get_pc());
			video_print_string(msg);
			SANITY_ASSERT(0);
		}
	#else
		if (this->cpl == CPU_X8664_CPL_USER && this->interrupt_enabled) {
			this->flush_internal_caches();
			if (c < 256) { // int instruction
				x86_84_int_vector_el el;

				this->get_idt_el(c, &el);

				if (!el.mask.permit_software) {
					this->get_idt_el(BX_GP_EXCEPTION, &el);
				}

				// handler address is aligned to 4 bytes
				this->go_to_kernel_mode(el.mask.handler_addr << 2);
			}
			else if (c == 0xFFFF) { // syscall instruction
				uint64_t temp_rsp;

				#ifdef CPU_CACHE_LOCAL_STATISTIC
					cache_memory_struct->event_contex_switch(this);
				#endif

				this->cpl = CPU_X8664_CPL_KERNEL;

				// load kernel stack
				temp_rsp = RSP;
				RSP = this->model_specific_regs[CPU_X8664_MSR_KERNEL_STACK_PTR];

				RCX = this->get_pc();
				R11 = this->read_eflags();

				// push user's stack pointer
				RSP -= 8;
				this->memory_write_8_bytes(RSP, temp_rsp);

				// redirect pc
				this->set_pc(this->model_specific_regs[CPU_X8664_MSR_SYSCALL_HANDLER_ADDR]);
			}
		}
	#endif
}

uint8_t cpu::processor_t::process_external_interruption(uint8_t c)
{
	#ifdef X86_64_FAKE_SYSCALLS
		return 1;
	#else
		uint8_t ret_val;
		uint8_t code;

		code = c + 32; // the first 32 elements are exceptions

		if (this->cpl == CPU_X8664_CPL_USER && this->interrupt_enabled) {
			x86_84_int_vector_el el;
			this->flush_internal_caches();
	//LOG_PRINTF("external interruption %u\n", code);
	//SANITY_ASSERT(0);
			this->get_idt_el(code, &el);

			// handler address is aligned to 4 bytes
			this->go_to_kernel_mode(el.mask.handler_addr << 2);

			ret_val = 1;
		}
		else {
			ret_val = 0;
		}

		return ret_val;
	#endif
}

void cpu::processor_t::process_instruction_that_flushes_pipeline()
{
	this->flush_internal_caches();
	#ifdef CPU_CACHE_LOCAL_STATISTIC
		cache_memory_struct->event_contex_switch(this);
	#endif
}

void cpu::processor_t::go_to_kernel_mode(uint64_t start_pc)
{
	uint64_t temp_rsp;

	#ifdef CPU_CACHE_LOCAL_STATISTIC
		cache_memory_struct->event_contex_switch(this);
	#endif

	this->cpl = CPU_X8664_CPL_KERNEL;

	// load kernel stack
	temp_rsp = RSP;
	RSP = this->model_specific_regs[CPU_X8664_MSR_KERNEL_STACK_PTR];

	// push rflags
	RSP -= 8;
	this->memory_write_8_bytes(RSP, this->read_eflags());

	// push user's stack pointer
	RSP -= 8;
	this->memory_write_8_bytes(RSP, temp_rsp);

	// push user's rip
	RSP -= 8;
	this->memory_write_8_bytes(RSP, this->get_pc());

	// redirect pc
	this->set_pc(start_pc);
}

uint32_t cpu::processor_t::memory_read_inst_byte(target_addr_t vaddr, uint8_t& has_exception, exception_code_t& exc)
{
	x86_64_virtual_memory_data vm;
//if (this->cpl == 3){LOG_PRINTF("fetching...\n");}
	vm.vaddr = vaddr;
	this->get_physical_memory_address(&vm, this->x86_64_vmi_cache);
//if (this->cpl == 3){LOG_PRINTF("fetching ended...\n");}
	if (vm.present == 0) {
		has_exception = 1;
		exc = BX_PF_EXCEPTION;
		exc |= vaddr & 0xFFFFFFFFFFFFF000LL;
		return 0;
	}

	if (vm.execute == 0) {
		has_exception = 1;
		exc = BX_GP_EXCEPTION;
		exc |= vaddr & 0xFFFFFFFFFFFFF000LL;
		return 0;
	}

	has_exception = 0;

	return vm.paddr;
}

uint8_t cpu::processor_t::bochs_mem_read_1_bytes(uint64_t vaddr)
{
	uint8_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read(vaddr, 1, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	data = this->memory_read_1_bytes(vm->first_page.paddr);

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("memory read 1 byte from vaddress 0x"PRINTF_INT64X_PREFIX"  paddress=0x%X   data=0x%X decimal=%i\n", vaddr, vm->first_page.paddr, (uint32_t)data, (int32_t)((int8_t)data));
	#endif

	return data;
}

uint16_t cpu::processor_t::bochs_mem_read_2_bytes(uint64_t vaddr)
{
	uint16_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read(vaddr, 2, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		data = this->memory_read_2_bytes(vm->first_page.paddr);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 2 bytes from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   data=0x%X decimal=%ih\n", vaddr, vm->first_page.paddr, (uint32_t)data, data);
		#endif
	}
	else {
		uint16_t tmp;
		tmp = this->memory_read_1_bytes(vm->first_page.paddr);
		data = tmp;
		tmp = this->memory_read_1_bytes(vm->second_page.paddr);
		data |= tmp << 8;
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 2 bytes boundary from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr1=0x%X  paddr2=0x%X   data=0x%X decimal=%ih\n", vaddr, vm->first_page.paddr, vm->second_page.paddr, (uint32_t)data, data);
		#endif
	}

	return data;
}

#define READ_FROM_BOUNDARY(DATA, VM)   {     \
		uint8_t tmp; \
		uint64_t i; \
		uint32_t z, j; \
		\
		z = 0; \
		DATA = 0; \
		\
		j = 0; \
		for (i=vaddr; i<VM->second_page.vaddr; i++) { \
			tmp = this->memory_read_1_bytes(VM->first_page.paddr+j); \
			DATA |= tmp << (z << 3); \
			z++; \
			j++; \
		} \
		\
		j = 0; \
		for (i=VM->second_page.vaddr; i<=VM->last_vaddr_second_page; i++) { \
			tmp = this->memory_read_1_bytes(VM->second_page.paddr+j); \
			DATA |= tmp << (z << 3); \
			z++; \
			j++; \
		} \
	}

uint32_t cpu::processor_t::bochs_mem_read_4_bytes(uint64_t vaddr)
{
	uint32_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read(vaddr, 4, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

//if (vaddr > 100000) {LOG_PRINTF("read ....... pc 0x"PRINTF_INT64X_PREFIX"\n",this->get_pc());SANITY_ASSERT(0);}
	if (!vm->cross_boundary) {
		data = this->memory_read_4_bytes(vm->first_page.paddr);

		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 4 bytes from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   data=0x%X decimal=%i\n", vaddr, vm->first_page.paddr, (uint32_t)data, data);
		#endif
	}
	else {
//		READ_FROM_BOUNDARY(data, vm)
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_read_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_read_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 4-len);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 4 bytes boundary from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   paddr2=0x%X   data=0x%X decimal=%i\n", vaddr, vm->first_page.paddr, vm->second_page.paddr, (uint32_t)data, data);
		#endif
	}

	return data;
}

uint64_t cpu::processor_t::bochs_mem_read_8_bytes(uint64_t vaddr)
{
	uint64_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read(vaddr, 8, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		data = this->memory_read_8_bytes(vm->first_page.paddr);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 8 bytes from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   data="PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", vaddr, vm->first_page.paddr, data, data);
		#endif
	}
	else {
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_read_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_read_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 8-len);
//		READ_FROM_BOUNDARY(data, vm)
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 8 bytes boundary from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X   data="PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", vaddr, vm->first_page.paddr, vm->second_page.paddr, data, data);
		#endif
	}

//if (this->get_pc()==0x402698){LOG_PRINTF("/*pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX" cross=%u*/\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr, vm->cross_boundary);}
//if (this->get_pc()==0x46be){LOG_PRINTF("pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr);}
//if (this->get_pc()==0x46da){LOG_PRINTF("pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr);}
//if (this->get_pc()==0x46ea){LOG_PRINTF("pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr);}

	return data;
}

void cpu::processor_t::bochs_mem_read_PackedXmmRegister_aligned(uint64_t vaddr, BxPackedXmmRegister *data)
{
	if (vaddr & 15) {
		exception(BX_GP_EXCEPTION, 0, 0);
	}
	this->bochs_mem_read_PackedXmmRegister(vaddr, data);
}

void cpu::processor_t::bochs_mem_read_PackedXmmRegister(uint64_t vaddr, BxPackedXmmRegister *data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read(vaddr, 16, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		this->memory_read_n_bytes(vm->first_page.paddr, data, 16);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 8 bytes from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   data="PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", vaddr, vm->first_page.paddr, data, data);
		#endif
	}
	else {
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_read_n_bytes(vm->first_page.paddr, data, len);
		this->memory_read_n_bytes(vm->second_page.paddr, ((uint8_t*)(data))+len, 16-len);
//		READ_FROM_BOUNDARY(data, vm)
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 8 bytes boundary from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X   data="PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", vaddr, vm->first_page.paddr, vm->second_page.paddr, data, data);
		#endif
	}
}

void cpu::processor_t::bochs_mem_write_1_bytes(uint64_t vaddr, uint8_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_write(vaddr, 1, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

//if (this->get_pc()==0x32ce){LOG_PRINTF("write 8 bits  0x%X    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", data, (uint64_t)vm->paddr, (uint64_t)vm->paddr);}
	#ifdef DEBUG_CPU
		DEBUG_PRINTF("memory writting 1 byte 0x%X (decimal=%i) into vaddress 0x"PRINTF_INT64X_PREFIX"   paddr=0x%X\n", (uint32_t)data, (int32_t)((int8_t)data), vaddr, vm->first_page.paddr);
	#endif
	this->memory_write_1_bytes(vm->first_page.paddr, data);
}

void cpu::processor_t::bochs_mem_write_2_bytes(uint64_t vaddr, uint16_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_write(vaddr, 2, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 2 bytes 0x%X (decimal=%ih) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X\n", (uint32_t)data, data, vaddr, vm->first_page.paddr);
		#endif
		this->memory_write_2_bytes(vm->first_page.paddr, data);
	}
	else {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 2 bytes boundary 0x%X (decimal=%ih) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X\n", (uint32_t)data, data, vaddr, vm->first_page.paddr, vm->second_page.paddr);
		#endif
		this->memory_write_1_bytes(vm->first_page.paddr, data & 0x00FF);
		this->memory_write_1_bytes(vm->second_page.paddr, data >> 8);
	}
}

#define WRITE_TO_BOUNDARY(DATA, VM)   {     \
		uint8_t tmp; \
		uint64_t i; \
		uint32_t z, j; \
		\
		z = 0; \
		\
		j = 0; \
		for (i=VM->vaddr; i<VM->second_page.vaddr; i++) { \
			tmp = (DATA >> (z << 3)) & 0x00FF; \
			this->memory_write_1_bytes(VM->first_page.paddr+j, tmp); \
			z++; \
			j++; \
		} \
		\
		j = 0; \
		for (i=VM->second_page.vaddr; i<=VM->last_vaddr_second_page; i++) { \
			tmp = (DATA >> (z << 3)) & 0x00FF; \
			this->memory_write_1_bytes(VM->second_page.paddr+j, tmp); \
			z++; \
			j++; \
		} \
	}

void cpu::processor_t::bochs_mem_write_4_bytes(uint64_t vaddr, uint32_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_write(vaddr, 4, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 4 bytes 0x%X (decimal=%i) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X\n", (uint32_t)data, data, vaddr, vm->first_page.paddr);
		#endif
		this->memory_write_4_bytes(vm->first_page.paddr, data);
	}
	else {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting boundary 4 bytes 0x%X (decimal=%i) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddra2=0x%X\n", (uint32_t)data, data, vaddr, vm->first_page.paddr, vm->second_page.paddr);
		#endif
		//WRITE_TO_BOUNDARY(uint32_t, data, vm)
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_write_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_write_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 4-len);
	}
}

void cpu::processor_t::bochs_mem_write_8_bytes(uint64_t vaddr, uint64_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_write(vaddr, 8, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 8 bytes 0x"PRINTF_INT64X_PREFIX" (decimal="PRINTF_INT64_PREFIX") into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X\n", data, data, vaddr, vm->first_page.paddr);
		#endif
		this->memory_write_8_bytes(vm->first_page.paddr, data);
	}
	else {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting boundary 8 bytes 0x"PRINTF_INT64X_PREFIX" (decimal="PRINTF_INT64_PREFIX") into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X\n", data, data, vaddr, vm->first_page.paddr, vm->second_page.paddr);
		#endif
		//WRITE_TO_BOUNDARY(uint64_t, data, vm)
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_write_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_write_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 8-len);
	}
}

void cpu::processor_t::bochs_mem_write_PackedXmmRegister_aligned(uint64_t vaddr, BxPackedXmmRegister *data)
{
	if (vaddr & 15) {
		exception(BX_GP_EXCEPTION, 0, 0);
	}
	this->bochs_mem_write_PackedXmmRegister(vaddr, data);
}

void cpu::processor_t::bochs_mem_write_PackedXmmRegister(uint64_t vaddr, BxPackedXmmRegister *data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_write(vaddr, 16, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 16 bytes to vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X\n", vaddr, vm->first_page.paddr);
		#endif
		this->memory_write_n_bytes(vm->first_page.paddr, data, 16);
	}
	else {
		uint8_t len;
		#ifdef DEBUG_CPU
//			DEBUG_PRINTF("memory writting boundary 8 bytes 0x"PRINTF_INT64X_PREFIX" (decimal="PRINTF_INT64_PREFIX") into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X\n", data, data, vaddr, vm->first_page.paddr, vm->second_page.paddr);
		#endif
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_write_n_bytes(vm->first_page.paddr, data, len);
		this->memory_write_n_bytes(vm->second_page.paddr, ((uint8_t*)(data))+len, 16-len);
	}
}

uint8_t cpu::processor_t::bochs_mem_RMW_read_1_bytes(uint64_t vaddr)
{
	uint8_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read_write(vaddr, 1, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	data = this->memory_read_1_bytes(vm->first_page.paddr);

	#ifdef DEBUG_CPU
		DEBUG_PRINTF("memory read 1 byte from vaddress 0x"PRINTF_INT64X_PREFIX"  paddress=0x%X   data=0x%X decimal=%i\n", vaddr, vm->first_page.paddr, (uint32_t)data, (int32_t)((int8_t)data));
	#endif

	return data;
}

uint16_t cpu::processor_t::bochs_mem_RMW_read_2_bytes(uint64_t vaddr)
{
	uint16_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read_write(vaddr, 2, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		data = this->memory_read_2_bytes(vm->first_page.paddr);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 2 bytes from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   data=0x%X decimal=%ih\n", vaddr, vm->first_page.paddr, (uint32_t)data, data);
		#endif
	}
	else {
		uint16_t tmp;
		tmp = this->memory_read_1_bytes(vm->first_page.paddr);
		data = tmp;
		tmp = this->memory_read_1_bytes(vm->second_page.paddr);
		data |= tmp << 8;
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 2 bytes boundary from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr1=0x%X  paddr2=0x%X   data=0x%X decimal=%ih\n", vaddr, vm->first_page.paddr, vm->second_page.paddr, (uint32_t)data, data);
		#endif
	}

	return data;
}

uint32_t cpu::processor_t::bochs_mem_RMW_read_4_bytes(uint64_t vaddr)
{
	uint32_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read_write(vaddr, 4, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

//if (vaddr > 100000) {LOG_PRINTF("read ....... pc 0x"PRINTF_INT64X_PREFIX"\n",this->get_pc());SANITY_ASSERT(0);}
	if (!vm->cross_boundary) {
		data = this->memory_read_4_bytes(vm->first_page.paddr);

		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 4 bytes from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   data=0x%X decimal=%i\n", vaddr, vm->first_page.paddr, (uint32_t)data, data);
		#endif
	}
	else {
		//READ_FROM_BOUNDARY(uint32_t, data, vm)
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_read_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_read_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 4-len);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 4 bytes boundary from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   paddr2=0x%X   data=0x%X decimal=%i\n", vaddr, vm->first_page.paddr, vm->second_page.paddr, (uint32_t)data, data);
		#endif
	}

	return data;
}

uint64_t cpu::processor_t::bochs_mem_RMW_read_8_bytes(uint64_t vaddr)
{
	uint64_t data;
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;
	uint8_t has_exception;
	exception_code_t exc;

	this->mem_check_exc_read_write(vaddr, 8, has_exception, exc);

	if (has_exception) {
		this->exception(exc, 0, 0);
	}

	if (!vm->cross_boundary) {
		data = this->memory_read_8_bytes(vm->first_page.paddr);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 8 bytes from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X   data="PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", vaddr, vm->first_page.paddr, data, data);
		#endif
	}
	else {
//		READ_FROM_BOUNDARY(uint64_t, data, vm)
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_read_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_read_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 8-len);
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory read 8 bytes boundary from vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X   data="PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", vaddr, vm->first_page.paddr, vm->second_page.paddr, data, data);
		#endif
	}

//if (this->get_pc()==0x402698){LOG_PRINTF("/*pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX" cross=%u*/\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr, vm->cross_boundary);}
//if (this->get_pc()==0x46be){LOG_PRINTF("pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr);}
//if (this->get_pc()==0x46da){LOG_PRINTF("pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr);}
//if (this->get_pc()==0x46ea){LOG_PRINTF("pc 0x%llX read 64 bits  0x%llX    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", this->get_pc(), data, (uint64_t)vm->vaddr, (uint64_t)vm->vaddr);}

	return data;
}

void cpu::processor_t::bochs_mem_RMW_write_1_bytes(uint8_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;

//if (this->get_pc()==0x32ce){LOG_PRINTF("write 8 bits  0x%X    addr=0x"PRINTF_INT64X_PREFIX" decimal="PRINTF_INT64_PREFIX"\n", data, (uint64_t)vm->paddr, (uint64_t)vm->paddr);}
	#ifdef DEBUG_CPU
		DEBUG_PRINTF("memory writting 1 byte 0x%X (decimal=%i) into vaddress 0x"PRINTF_INT64X_PREFIX"   paddr=0x%X\n", (uint32_t)data, (int32_t)((int8_t)data), vm->vaddr, vm->first_page.paddr);
	#endif
	this->memory_write_1_bytes(vm->first_page.paddr, data);
}

void cpu::processor_t::bochs_mem_RMW_write_2_bytes(uint16_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;

	if (!vm->cross_boundary) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 2 bytes 0x%X (decimal=%ih) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X\n", (uint32_t)data, data, vm->vaddr, vm->first_page.paddr);
		#endif
		this->memory_write_2_bytes(vm->first_page.paddr, data);
	}
	else {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 2 bytes boundary 0x%X (decimal=%ih) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X\n", (uint32_t)data, data, vm->vaddr, vm->first_page.paddr, vm->second_page.paddr);
		#endif
		this->memory_write_1_bytes(vm->first_page.paddr, data & 0x00FF);
		this->memory_write_1_bytes(vm->second_page.paddr, data >> 8);
	}
}

void cpu::processor_t::bochs_mem_RMW_write_4_bytes(uint32_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;

	if (!vm->cross_boundary) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 4 bytes 0x%X (decimal=%i) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X\n", (uint32_t)data, data, vm->vaddr, vm->first_page.paddr);
		#endif
		this->memory_write_4_bytes(vm->first_page.paddr, data);
	}
	else {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting boundary 4 bytes 0x%X (decimal=%i) into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddra2=0x%X\n", (uint32_t)data, data, vm->vaddr, vm->first_page.paddr, vm->second_page.paddr);
		#endif
//		WRITE_TO_BOUNDARY(uint32_t, data, vm)
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_write_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_write_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 4-len);
	}
}

void cpu::processor_t::bochs_mem_RMW_write_8_bytes(uint64_t data)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;

	if (!vm->cross_boundary) {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting 8 bytes 0x"PRINTF_INT64X_PREFIX" (decimal="PRINTF_INT64_PREFIX") into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X\n", data, data, vm->vaddr, vm->first_page.paddr);
		#endif
		this->memory_write_8_bytes(vm->first_page.paddr, data);
	}
	else {
		#ifdef DEBUG_CPU
			DEBUG_PRINTF("memory writting boundary 8 bytes 0x"PRINTF_INT64X_PREFIX" (decimal="PRINTF_INT64_PREFIX") into vaddress 0x"PRINTF_INT64X_PREFIX"  paddr=0x%X  paddr2=0x%X\n", data, data, vm->vaddr, vm->first_page.paddr, vm->second_page.paddr);
		#endif
//		WRITE_TO_BOUNDARY(uint64_t, data, vm)
		uint8_t len;
		len = vm->second_page.vaddr - vm->vaddr;
		this->memory_write_n_bytes(vm->first_page.paddr, &data, len);
		this->memory_write_n_bytes(vm->second_page.paddr, ((uint8_t*)(&data))+len, 8-len);
	}
}

void cpu::processor_t::pipeline_before_decode_inst()
{
	this->virtual_memory_translate.exception_read_check = 0;
	this->virtual_memory_translate.exception_write_check = 0;
	if (this->cpl == CPU_X8664_CPL_KERNEL) {
		io::timer->set_counter(0);
	}
}

void cpu::processor_t::pipeline_after_decode_inst()
{
}

void cpu::processor_t::pipeline_before_fetch_inst()
{
	if (this->cpl == CPU_X8664_CPL_KERNEL)
		this->x86_64_statistic_cycle_kernel++;
	else
		this->x86_64_statistic_cycle_user++;
}

void cpu::processor_t::pipeline_after_fetch_inst()
{
}

void cpu::processor_t::mem_check_exc_read(uint64_t vaddr, uint8_t length, uint8_t& has_exception, exception_code_t& exc)
{
	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;

	vm->exception_read_check = 0;

	vm->vaddr = vaddr;
	vm->length = length;

	vm->first_page.vaddr = vaddr;
	this->get_physical_memory_address(&vm->first_page);

	if (VIRTUAL_MEMORY_IS_ENABLED == 0 || (vaddr & 0xFFFFFFFFFFFFF000LL) == ((vaddr+length-1) & 0xFFFFFFFFFFFFF000LL))
		vm->cross_boundary = 0;
	else {
		vm->last_vaddr_second_page = vaddr + length - 1;
		vm->second_page.vaddr = vm->last_vaddr_second_page & 0xFFFFFFFFFFFFF000LL;
		this->get_physical_memory_address(&vm->second_page);

		vm->cross_boundary = 1;
	}

	if (vm->first_page.present == 0) {
		has_exception = 1;
		exc = BX_PF_EXCEPTION;
		exc |= vm->first_page.vaddr & 0xFFFFFFFFFFFFF000LL;
		return;
	}
	else if (vm->cross_boundary && vm->second_page.present == 0) {
		has_exception = 1;
		exc = BX_PF_EXCEPTION;
		exc |= vm->second_page.vaddr & 0xFFFFFFFFFFFFF000LL;
		return;
	}

	has_exception = 0;

	vm->exception_read_check = 1;
}

void cpu::processor_t::mem_check_exc_read_write(uint64_t vaddr, uint8_t length, uint8_t& has_exception, exception_code_t& exc)
{
	// copy of mem_check_exc_write, but sets the read flag too

	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;

	vm->exception_write_check = 0;
	vm->exception_read_check = 0;

	vm->vaddr = vaddr;
	vm->length = length;

	vm->first_page.vaddr = vaddr;
	this->get_physical_memory_address(&vm->first_page);

	if (VIRTUAL_MEMORY_IS_ENABLED == 0 || (vaddr & 0xFFFFFFFFFFFFF000LL) == ((vaddr+length-1) & 0xFFFFFFFFFFFFF000LL))
		vm->cross_boundary = 0;
	else {
		vm->last_vaddr_second_page = vaddr + length - 1;
		vm->second_page.vaddr = vm->last_vaddr_second_page & 0xFFFFFFFFFFFFF000LL;
		this->get_physical_memory_address(&vm->second_page);

		vm->cross_boundary = 1;
	}

	if (vm->first_page.present == 0) {
		has_exception = 1;
		exc = BX_PF_EXCEPTION;
		exc |= vm->first_page.vaddr & 0xFFFFFFFFFFFFF000LL;
		return;
	}
	else if (vm->first_page.write == 0) {
		has_exception = 1;
		exc = BX_GP_EXCEPTION;
		exc |= vm->first_page.vaddr & 0xFFFFFFFFFFFFF000LL;
		return;
	}
	else if (vm->cross_boundary) {
		if (vm->second_page.present == 0) {
			has_exception = 1;
			exc = BX_PF_EXCEPTION;
			exc |= vm->second_page.vaddr & 0xFFFFFFFFFFFFF000LL;
			return;
		}
		else if (vm->second_page.write == 0) {
			has_exception = 1;
			exc = BX_GP_EXCEPTION;
			exc |= vm->second_page.vaddr & 0xFFFFFFFFFFFFF000LL;
			return;
		}
	}

	has_exception = 0;

	vm->exception_write_check = 1;
	vm->exception_read_check = 1;
}

void cpu::processor_t::mem_check_exc_write(uint64_t vaddr, uint8_t length, uint8_t& has_exception, exception_code_t& exc)
{
	// all that is here must be in mem_check_exc_read_write

	x86_64_virtual_memory_data2 *vm = &this->virtual_memory_translate;

	vm->exception_write_check = 0;

	vm->vaddr = vaddr;
	vm->length = length;

	vm->first_page.vaddr = vaddr;
	this->get_physical_memory_address(&vm->first_page);

	if (VIRTUAL_MEMORY_IS_ENABLED == 0 || (vaddr & 0xFFFFFFFFFFFFF000LL) == ((vaddr+length-1) & 0xFFFFFFFFFFFFF000LL))
		vm->cross_boundary = 0;
	else {
		vm->last_vaddr_second_page = vaddr + length - 1;
		vm->second_page.vaddr = vm->last_vaddr_second_page & 0xFFFFFFFFFFFFF000LL;
		this->get_physical_memory_address(&vm->second_page);

		vm->cross_boundary = 1;
	}

	if (vm->first_page.present == 0) {
		has_exception = 1;
		exc = BX_PF_EXCEPTION;
		exc |= vm->first_page.vaddr & 0xFFFFFFFFFFFFF000LL;
		return;
	}
	else if (vm->first_page.write == 0) {
		has_exception = 1;
		exc = BX_GP_EXCEPTION;
		exc |= vm->first_page.vaddr & 0xFFFFFFFFFFFFF000LL;
		return;
	}
	else if (vm->cross_boundary) {
		if (vm->second_page.present == 0) {
			has_exception = 1;
			exc = BX_PF_EXCEPTION;
			exc |= vm->second_page.vaddr & 0xFFFFFFFFFFFFF000LL;
			return;
		}
		else if (vm->second_page.write == 0) {
			has_exception = 1;
			exc = BX_GP_EXCEPTION;
			exc |= vm->second_page.vaddr & 0xFFFFFFFFFFFFF000LL;
			return;
		}
	}

	has_exception = 0;

	vm->exception_write_check = 1;
}

void cpu::processor_t::print_registers2(char *s)
{
//#define KDSKLDKL

#ifdef X86_64_COMPARE_RESULTS
	struct instruction_cycle {
		uint64_t regs[18];
	};

	static instruction_cycle stc;
	static int must_read = 1;
	static int line = 0;
	static int error = 0;
	int i;

	if (must_read) {
		line++;

		if (fread(&stc, sizeof(instruction_cycle), 1, fp_x86_64_reg_data) != 1) {
			LOG_PRINTF("reg_trace.dat finished!\n");
			exit(1);
		}

		must_read = 0;
	}

	if (this->get_pc() == stc.regs[16]) {
		for (i=0; i<=10; i++) {
			if (this->gen_regs[i] != stc.regs[i]) {
				LOG_PRINTF("inconsistencia na linha %i\n", line);
				exit(1);
			}
		}
		for (i=12; i<=15; i++) {
			if (this->gen_regs[i] != stc.regs[i]) {
				LOG_PRINTF("inconsistencia na linha %i\n", line);
				exit(1);
			}
		}

		must_read = 1;
		error = 0;
	}
	else if (error > 50) {
		LOG_PRINTF("perdi o rumo na linha %i\n", line);
		exit(1);
	}
	else
		error = 0;
#endif

#ifdef KDSKLDKL
	uint32_t i;
	static int64_t cycle_=0, cycle_2=0;
	uint32_t stepi = 1;

	if (strcmp(s, "CF")) {
		if (cycle_==0) {
			LOG_PRINTF("<?php\n");
		}
		if ((cycle_ % stepi) == 0) {
	LOG_PRINTF("/* cycle %lli func = %s */ \n", cycle_, s);
		for (i=0; i<BX_GENERAL_REGISTERS; i++) {
			LOG_PRINTF("$reg[%u][%u] = \"0x%llX\";\n", cycle_2, i, this->read_gen_reg_64(i));
		}
		LOG_PRINTF("$reg[%u][16] = \"0x%llX\";\n", cycle_2, this->read_eflags());

		LOG_PRINTF("$reg[%u][17] = \"0x%llX\";\n", cycle_2, this->get_pc());

		LOG_PRINTF("$reg[%u][18] = \"%s\";\n", cycle_2, s);
		cycle_2++;
	//	LOG_PRINTF("<regend>\n");
	LOG_PRINTF("/*******************************/ \n");

	//if (cycle_2 == 10000) { exit(0); }
	}

		cycle_++;
	}
#endif
}

void cpu::processor_t::print_registers()
{
	char state[300];
	uint32_t i;
	rs_instruction_t *s;

	for (i=0; i<BX_GENERAL_REGISTERS; i++) {
		sprintf(state, "deps: ");
		strcat(state, "0-7: ");
		s = *this->get_dep_vector_handler(CPU_X8664_DEP_VECTOR_GPR_BITS_0_7, i);
		if ((s != NULL)) {
			#ifdef DEBUG_CPU
				strcat(state, s->get_unmounted_inst());
			#else
				strcat(state, "NOT READY");
			#endif
		}
		else
			strcat(state, "READY");
		strcat(state, "      8-15: ");
		s = *this->get_dep_vector_handler(CPU_X8664_DEP_VECTOR_GPR_BITS_8_15, i);
		if ((s != NULL)) {
			#ifdef DEBUG_CPU
				strcat(state, s->get_unmounted_inst());
			#else
				strcat(state, "NOT READY");
			#endif
		}
		else
			strcat(state, "READY");
		strcat(state, "      16-63: ");
		s = *this->get_dep_vector_handler(CPU_X8664_DEP_VECTOR_GPR_BITS_16_63, i);
		if ((s != NULL)) {
			#ifdef DEBUG_CPU
				strcat(state, s->get_unmounted_inst());
			#else
				strcat(state, "NOT READY");
			#endif
		}
		else
			strcat(state, "READY");

		//sDEBUG_PRINTF(state, "s=%u", CONVERT64TOPRINT(get_gen_reg_dep(i)->get_id()));
		LOG_PRINTF("gpr[%u] = "PRINTF_INT64_PREFIX" (0x"PRINTF_INT64X_PREFIX") %s           ", i, this->read_gen_reg_64(i), this->read_gen_reg_64(i), state);
		LOG_PRINTF("\n");
	}
	LOG_PRINTF("PC="PRINTF_INT64_PREFIX"(0x"PRINTF_INT64X_PREFIX") - SPEC_PC="PRINTF_INT64_PREFIX" (0x"PRINTF_INT64X_PREFIX")\n", this->get_pc(), this->get_pc(), this->spec_pc, this->spec_pc);
	LOG_PRINTF("CF=%u    PF=%u    AF=%u    ZF=%u    SF=%u    DF=%u    OF=%u\n\n",
		(uint32_t)this->get_CF(),
		(uint32_t)this->get_PF(),
		(uint32_t)this->get_AF(),
		(uint32_t)this->get_ZF(),
		(uint32_t)this->get_SF(),
		(uint32_t)this->get_DF(),
		(uint32_t)this->get_OF());

	LOG_PRINTF("CPL=%u\n", this->cpl);
	LOG_PRINTF("paging enabled=%u\n", this->msr_virtual_memory_mask->mask.paging_enable);
	LOG_PRINTF("interruption enabled=%u\n", this->interrupt_enabled);
}


