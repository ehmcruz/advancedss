#include "keyboard.h"
#include "vars.h"
#include "lib.h"
#include "process.h"

#define IO_PORTS_READ_KEYBOARD_STATUS        0x3000
#define IO_PORTS_READ_KEYBOARD_DATA          0x3001

void keyboard_initialize()
{

}

void keyboard_interrupt()
{
	uint8_t typed;
	thread_t *t = NULL;
	video_data_t *video;
	uint8_t video_id;
	process_t *p;
	
	interrupt_happened();
	
	typed = inportb(IO_PORTS_READ_KEYBOARD_DATA);
//	printk("key %c pressed\n",typed);

//		video_print_string(&process_focus_process->video_buffer, "key typed\n");
	//	video_refresh(&process_focus_process->video_buffer);

	if (typed < 128) {
		video = video_get_focus_window();
		if (video != NULL) {
			video_id = video_get_window_id(video);
			p = process_blocked_window_keyboard[video_id];
			t = process_warn_keyboard_typed(p, typed);
		}
	}
	else if (typed == SIM_KEYBOARD_SCAN_CODE_PAGE_UP) {
		uint8_t window;
		window = video_get_focus_window_id() + 1;
		if (window <= CONFIG_KERNEL_NUMBER_OF_WINDOWS)
			video_set_focus_window(window);
	}
	else if (typed == SIM_KEYBOARD_SCAN_CODE_PAGE_DOWN) {
		uint8_t window;
		window = video_get_focus_window_id() - 1;
		if (window <= CONFIG_KERNEL_NUMBER_OF_WINDOWS)
			video_set_focus_window(window);
	}
	
	if (t == NULL)	
		process_scheduler_schedule();
	else {
		process_blocked_window_keyboard[video_id] = NULL;
		process_schedule_thread(p, t);
	}
	
/*	video_print_string(&process_focus_process->video_buffer, "kb scheduling ");
	video_print_string(&process_focus_process->video_buffer, t->owner->fname);
	video_print_string(&process_focus_process->video_buffer, "\n");
	video_refresh(&process_focus_process->video_buffer);*/
	
	// else, discard
}
