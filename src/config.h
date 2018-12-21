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

#ifndef _ADVANCEDSS_CONFIGURE_HEADER_
#define _ADVANCEDSS_CONFIGURE_HEADER_

#include <stdio.h>
#include <assert.h>

//#define DEBUG_CPU

//#define CPU_CACHE_LOCAL_STATISTIC 10000

//#define CPU_CHECK_EXECUTION_TRACE

#define SANITY_CHECK

#define ADVANCEDSS_CPU_CYCLE_SYNCRONOUS

//#define CPU_DETECT_PIPELINE_DEADLOCK_CYCLES      20

/************************/

// x86-64 config

//#define X86_64_FAKE_SYSCALLS

#ifdef X86_64_FAKE_SYSCALLS
	#define CONFIG_DO_NOT_WAIT_FOR_KEYBOARD_INPUT
#endif

/************************/

#define CONFIG_LOG_TO_FILE "log_.txt"

#ifdef CONFIG_LOG_TO_FILE
	extern FILE *output_debug_fp_;
	#define LOG_PRINTF(...) fprintf(output_debug_fp_, __VA_ARGS__);
	#define CONFIG_LOG_TO_FILE_CLOSE fclose(output_debug_fp_);
#else
	#define LOG_PRINTF(ARGS...) printf(ARGS);
	#define CONFIG_LOG_TO_FILE_CLOSE
#endif

#ifdef SANITY_CHECK
	#define SANITY_ASSERT(V)      if (!(V)) { \
			double time; \
			volatile int *aaa; \
			time = host_sim_get_running_time(); \
			LOG_PRINTF("Simulation stopped at cycle " PRINTF_UINT64_PREFIX " due to error (time = %f seconds... %f cycles/sec)\n", advancedss_get_system_cycle(), time, (double)advancedss_get_system_cycle() / time); \
			LOG_PRINTF("File %s at line %u assertion failed!\n%s\n", __FILE__, __LINE__, #V); \
			advancedss_print_status(); \
			CONFIG_LOG_TO_FILE_CLOSE \
			aaa = (volatile int*)0xFFFFFFFFFFFFLLU; \
			while (1) { \
				*aaa = 0; \
				aaa++; \
			} \
		}
//#define SANITY_ASSERT(V) assert(V);
#else
	#define SANITY_ASSERT(V)
#endif

#ifdef DEBUG_CPU
	#define CONVERT64TOPRINT(D)        (uint32_t)D
	#ifdef CONFIG_LOG_TO_FILE
//		#define DEBUG_PRINTF(ARGS...) { if (inst_aa_a >=1320376) fprintf(output_debug_fp_, ARGS);}
		#define DEBUG_PRINTF(ARGS...) fprintf(output_debug_fp_, ARGS)
	#else
		#define DEBUG_PRINTF(ARGS...) printf(ARGS)
	#endif
#else
	#define DEBUG_PRINTF(...)
#endif

#define PRINTF_INT64_PREFIX "%lli"
#define PRINTF_UINT64_PREFIX "%llu"
#define PRINTF_INT64X_PREFIX "%llX"

#define CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
#define CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN

/* if your compiler support variable length arrays
   gcc does, others might not support (like microsoft visual c++)

   not being used
*/
//#define CONFIG_COMPILER_SUPPORT_VLA

/* some compilers have problems with <stdint.h>
   - gcc linux didn't have any problems
   - mingw didn't recognize uint16_t, althought it's specified in it's <stdint.h>,
     so it's possibly a  bug
*/

// gcc  linux:

#include <stdint.h>

// mingw:

/*typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;*/

// microsoft compiler:

/*typedef signed __int8     int8_t;
typedef signed __int16    int16_t;
typedef signed __int32    int32_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef signed __int64       int64_t;
typedef unsigned __int64     uint64_t;*/

typedef uint32_t phy_addr_t;

//extern uint64_t inst_aa_a;

/***********************************************************/

#define GET_DATA8_FROM_VECTOR(V)      *((uint8_t*)V)

#ifdef CONFIG_HOST_SYSTEM_LITTLE_ENDIAN
	#ifdef CONFIG_TARGET_SYSTEM_LITTLE_ENDIAN
		#define GET_DATA16_FROM_VECTOR(V)      *((uint16_t*)V)
		#define GET_DATA32_FROM_VECTOR(V)      *((uint32_t*)V)
		#define GET_DATA64_FROM_VECTOR(V)      *((uint64_t*)V)
	#else
		#error Only little endian hosts and targets are already supported!
	#endif
#else
	#error Only little endian hosts and targets are already supported!
#endif

#ifndef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
	#error Asyncronous simulation still not supported!
#endif

// defined in main.cpp
void advancedss_print_status();
#ifdef ADVANCEDSS_CPU_CYCLE_SYNCRONOUS
	uint64_t advancedss_get_system_cycle();
#endif

// defined by host code
double host_sim_get_running_time();

#endif
