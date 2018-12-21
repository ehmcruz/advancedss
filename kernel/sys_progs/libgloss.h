#ifndef _KERNEL_SYSPROGS_HEADER_LIBRARY_H_
#define _KERNEL_SYSPROGS_HEADER_LIBRARY_H_

typedef signed char int8_t;
typedef unsigned char   uint8_t;
typedef short  int16_t;
typedef unsigned short  uint16_t;
typedef int  int32_t;
typedef unsigned   uint32_t;
typedef long long  int64_t;
typedef unsigned long long   uint64_t;

#define NULL 0

int read (int file, void *ptr, int len);
int write (int file, const void *ptr, int len);
int open (char *fname, int flags, int mode);
void* sbrk (int incr);

#endif
