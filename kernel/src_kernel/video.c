#include "video.h"
#include "printk.h"

#define IO_PORTS_WRITE_VIDEO_SET_BUFFER      0x1000
#define IO_PORTS_WRITE_VIDEO_CMD             0x1001
#define IO_PORTS_WRITE_VIDEO_WIDTH           0x1002
#define IO_PORTS_WRITE_VIDEO_HEIGHT          0x1003
#define IO_PORTS_WRITE_VIDEO_CURSOR_X        0x1004
#define IO_PORTS_WRITE_VIDEO_CURSOR_Y        0x1005

#define IO_VIDEO_CMD_REFRESH          0x00
#define IO_VIDEO_CMD_INIT             0x01
#define IO_VIDEO_CMD_SET_CURSOR_POS   0x03

static video_data_t video_frame_buffer_windows[ CONFIG_KERNEL_NUMBER_OF_WINDOWS + 1 ]; // 1 for kernel
static video_data_t *video_focused_window = NULL;

void video_init()
{
	uint8_t i;
	
	outportd(IO_PORTS_WRITE_VIDEO_WIDTH, CONFIG_KERNEL_VIDEO_XSIZE);
	outportd(IO_PORTS_WRITE_VIDEO_HEIGHT, CONFIG_KERNEL_VIDEO_YSIZE);
	outportb(IO_PORTS_WRITE_VIDEO_CMD, IO_VIDEO_CMD_INIT);	
	
	for (i=0; i<CONFIG_KERNEL_NUMBER_OF_WINDOWS+1; i++) {
		video_clear_buffer(video_frame_buffer_windows+i);
	}
	
	video_focused_window = video_frame_buffer_windows;
}

video_data_t* video_get_frame_buffer(uint8_t n)
{
	return video_frame_buffer_windows + n;
}

void video_set_focus_window(uint8_t n)
{
	video_focused_window = video_frame_buffer_windows + n;
	video_refresh(video_focused_window);
}

void video_set_focus_window_kernel()
{
	video_focused_window = video_frame_buffer_windows;
	video_refresh(video_focused_window);
}

video_data_t* video_get_focus_window()
{
	return video_focused_window;
}

uint8_t video_get_focus_window_id()
{
	return video_get_window_id(video_focused_window);
}

uint8_t video_get_window_id(video_data_t *video)
{
	uint8_t i;
	for (i=0; i<CONFIG_KERNEL_NUMBER_OF_WINDOWS+1; i++) {
		if ((video_frame_buffer_windows+i) == video)
			return i;
	}
	
	KERNEL_ASSERT(0);
}

void video_set_cursor_pos(video_data_t *video)
{
	outportd(IO_PORTS_WRITE_VIDEO_CURSOR_X, video->cursor_x);
	outportd(IO_PORTS_WRITE_VIDEO_CURSOR_Y, video->cursor_y);
	outportb(IO_PORTS_WRITE_VIDEO_CMD, IO_VIDEO_CMD_SET_CURSOR_POS);
}

void video_clear_buffer(video_data_t *video)
{
	uint16_t i, j, z;

	z = 0;
	for (i=0; i<CONFIG_KERNEL_VIDEO_YSIZE; i++) {
		for (j=0; j<CONFIG_KERNEL_VIDEO_XSIZE; j++) {
			video->buffer[z++] = ' ';
		}
	}

	video->cursor_x = 0;
	video->cursor_y = 0;
}

void video_refresh(video_data_t *video)
{
	outportd(IO_PORTS_WRITE_VIDEO_SET_BUFFER, (uint32_t)video->buffer);
	outportb(IO_PORTS_WRITE_VIDEO_CMD, IO_VIDEO_CMD_REFRESH);
	video_set_cursor_pos(video);
}

static void video_remove_line_from_video_buffer(video_data_t *video)
{
	uint16_t i, j, line_start;
	uint8_t *video_buffer2 = video->buffer;
	uint64_t *video_buffer = (uint64_t*)video->buffer;

	for (j=0; j<(CONFIG_KERNEL_VIDEO_XSIZE >> 3); j++) {
		for (i=0; i<CONFIG_KERNEL_VIDEO_YSIZE-1; i++) {
			line_start = i * (CONFIG_KERNEL_VIDEO_XSIZE >> 3);
			video_buffer[ line_start + j ] = video_buffer[ line_start + (CONFIG_KERNEL_VIDEO_XSIZE >> 3) + j ];
		}
	}

	line_start = (CONFIG_KERNEL_VIDEO_YSIZE - 1) * CONFIG_KERNEL_VIDEO_XSIZE;
	for (j=0; j<CONFIG_KERNEL_VIDEO_XSIZE; j++) {
		video_buffer2[ line_start + j ] = ' ';
	}
}

void video_print_string(video_data_t *video, char *s)
{
	char *b;
	uint32_t i, z;
	uint8_t *video_buffer = video->buffer;

	for (b=s; *b; b++) {	
		if (video->cursor_y == CONFIG_KERNEL_VIDEO_YSIZE) {
			video_remove_line_from_video_buffer(video);
			video->cursor_y--;
		}

		z = video->cursor_x + video->cursor_y*CONFIG_KERNEL_VIDEO_XSIZE;

		if (*b == 10) { // newline
			for (i=video->cursor_x; i<CONFIG_KERNEL_VIDEO_XSIZE; i++)
				video_buffer[z++] = ' ';
			video->cursor_x = 0;
			video->cursor_y++;
		}
		else if (*b == 13) { // return to line start
			z = video->cursor_y*CONFIG_KERNEL_VIDEO_XSIZE;
			for (i=0; i<CONFIG_KERNEL_VIDEO_XSIZE; i++)
				video_buffer[z++] = ' ';
			video->cursor_x = 0;
		}
		else {
			video_buffer[z++] = *b;

			video->cursor_x++;
			if (video->cursor_x == CONFIG_KERNEL_VIDEO_XSIZE) {
				video->cursor_x = 0;
				video->cursor_y++;
			}
		}
	}
	
	if (video == video_focused_window)
		video_refresh(video);
}

void video_set_cursor_x_pos(video_data_t *video, uint16_t x)
{
	uint16_t i, j;
	
	j = x;
	for (i=x+video->cursor_y*CONFIG_KERNEL_VIDEO_XSIZE; j<video->cursor_x; i++) {
		video->buffer[i] = ' ';
		j++;
	}
	video->cursor_x = x;
}
