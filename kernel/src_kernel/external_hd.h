#ifndef _KERNEL_HEADER_EXTERNALHD_H_
#define _KERNEL_HEADER_EXTERNALHD_H_

#include "config.h"
#include "lib.h"

struct external_hd_file_descriptor {
	uint64_t id;
	uint32_t pos;
	uint32_t fsize;
	char fname[CONFIG_KERNEL_MAX_FILE_NAME_LENGTH];
	uint8_t avl; // extesrnal hd won't look this, so it can be used by process as it wants
};

typedef struct external_hd_file_descriptor external_hd_file_descriptor;

void initialize_external_hd();
void external_hd_request_open_file(external_hd_file_descriptor *fd, char *fname, void (*callback)(uint64_t, void*), void *requester_data);
void external_hd_request_read_file(external_hd_file_descriptor *fd, void *buffer, uint32_t len, void (*callback)(uint64_t, void*), void *requester_data);
void external_hd_request_write_file(external_hd_file_descriptor *fd, void *buffer, uint32_t len, void (*callback)(uint64_t, void*), void *requester_data);
void external_hd_request_close_file(external_hd_file_descriptor *fd, void (*callback)(uint64_t, void*), void *requester_data);

#endif
