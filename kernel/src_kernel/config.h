#ifndef _KERNEL_HEADER_CONFIG_H_
#define _KERNEL_HEADER_CONFIG_H_

#define CONFIG_KERNEL_STACK_SIZE                  16000
#define CONFIG_KERNEL_MAX_PROCESS                 5
#define CONFIG_KERNEL_PROCESS_OPEN_QUEUE_SIZE     20
#define CONFIG_KERNEL_MAX_THREADS_PER_PROCESS     1
#define CONFIG_KERNEL_MAX_CPUS                    2

#define CONFIG_KERNEL_NUMBER_GPR                  16

#define CONFIG_KERNEL_VIDEO_XSIZE                 64  /* must be multiple of 8 */
#define CONFIG_KERNEL_VIDEO_YSIZE                 40

#define CONFIG_KERNEL_MSR_NUMBER_PHY_CPUS         0
#define CONFIG_KERNEL_MSR_NUMBER_VIRTUAL_CPUS_PER_PHY_CPU 1
#define CONFIG_KERNEL_MSR_CPUID                   2
#define CONFIG_KERNEL_MSR_KERNEL_STACK            3
#define CONFIG_KERNEL_MSR_MEM_SIZE                4
#define CONFIG_KERNEL_MSR_VIRTUAL_MEMORY_CFG              5
#define CONFIG_KERNEL_MSR_VIRTUAL_MEMORY_PAGE_FAULT_ADDR  6
#define CONFIG_KERNEL_MSR_INTERRUPT_VECTOR_ADDR           7
#define CONFIG_KERNEL_MSR_SYSCALL_HANDLER_ADDR            8

#define CONFIG_KERNEL_GPR_CODE_RAX           0
#define CONFIG_KERNEL_GPR_CODE_RCX           1
#define CONFIG_KERNEL_GPR_CODE_RBX           3
#define CONFIG_KERNEL_GPR_CODE_RSP           4
#define CONFIG_KERNEL_GPR_CODE_RBP           5

#define CONFIG_KERNEL_IDT_SIZE     256

#define CONFIG_KERNEL_VIRTUAL_MEMORY_ENTRIES_PER_TB     512

#define CONFIG_KERNEL_PAGE_SIZE                   4096

#define CONFIG_KERNEL_MAX_FILE_NAME_LENGTH        50
#define CONFIG_KERNEL_MAX_HD_OPERATIONS           1000
#define CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE     20480

#define CONFIG_KERNEL_PROCESS_BIN_FILE_MAX_SIZE   1000000

#define CONFIG_KERNEL_MAX_OPEN_FILES_PER_PROCESS     100

#define CONFIG_KERNEL_PROCESS_QUANTUM        10
#define CONFIG_KERNEL_PROCESS_KEYBOARD_BUFFER_SIZE        100

#define CONFIG_KERNEL_CODE_STDIN         0
#define CONFIG_KERNEL_CODE_STDOUT        1

#define SIM_KEYBOARD_SCAN_CODE_NEWLINE          10
#define SIM_KEYBOARD_SCAN_CODE_BACKSPACE        8
#define SIM_KEYBOARD_SCAN_CODE_F0               128
#define SIM_KEYBOARD_SCAN_CODE_F12              (128+12)
#define SIM_KEYBOARD_SCAN_CODE_PAGE_UP          141
#define SIM_KEYBOARD_SCAN_CODE_PAGE_DOWN        142

#define CONFIG_KERNEL_NUMBER_OF_WINDOWS         1

#define CONFIG_KERNEL_NUMBER_SYSCALL_BUFFER     5
#define CONFIG_KERNEL_SIZE_SYSCALL_BUFFER       1000000

#define CONFIG_KERNEL_THREAD_DEFAULT_STACK_SIZE       (4096 * 20)

#endif
