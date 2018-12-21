#ifndef _KERNEL_HEADER_VIDEO_H_
#define _KERNEL_HEADER_VIDEO_H_

#include "config.h"
#include "lib.h"

struct video_data_t
{
	uint8_t buffer[CONFIG_KERNEL_VIDEO_XSIZE * CONFIG_KERNEL_VIDEO_YSIZE];
	uint16_t cursor_x;
	uint16_t cursor_y;
};

typedef struct video_data_t video_data_t;

void video_init();
void video_clear_buffer(video_data_t *video);
void video_refresh(video_data_t *video);
void video_print_string(video_data_t *video, char *s);
void video_set_cursor_x_pos(video_data_t *video, uint16_t x);
video_data_t* video_get_frame_buffer(uint8_t n);
void video_set_focus_window(uint8_t n);
void video_set_focus_window_kernel();
video_data_t* video_get_focus_window();
uint8_t video_get_window_id(video_data_t *video);
uint8_t video_get_focus_window_id();

#endif
