#ifndef _KERNEL_HEADER_PRINTK_H_
#define _KERNEL_HEADER_PRINTK_H_

#include "config.h"
#include "lib.h"
#include "video.h"

void printk_(video_data_t *video_buffer, char *format, ...);
#define printk(ARGS...) printk_(video_get_frame_buffer(0), ARGS)

#endif
