#include <stdio.h>
#include <time.h>
#include "../host.h"

static uint32_t w_ = 0;
static uint32_t h_ = 0;
static uint32_t n_ = 0;

static clock_t start_time;

void* host_sim_input_thread_handler(void *v);

void host_sim_initialize()
{
	start_time = clock();
}

double host_sim_get_running_time()
{
	return (double)(clock() - start_time) / (double)CLOCKS_PER_SEC;
}

uint16_t host_sim_input_get_input_if_there_is()
{
}

void* host_sim_input_thread_handler(void *v)
{
}

void host_sim_video_start()
{
}

void host_sim_video_set_cursor_pos(uint32_t x, uint32_t y)
{
}

void host_sim_video_set_width(uint32_t w)
{
	w_ = w;
	n_ = w_ * h_;
}

void host_sim_video_set_height(uint32_t h)
{
	h_ = h;
	n_ = w_ * h_;
}

void host_sim_video_refresh(uint8_t *buffer)
{
	uint32_t i, j, k;

	printf("START OF VIDEO DATA\n\n");

	k = 0;
	for (i=0; i<h_; i++) {
		for (j=0; j<w_; j++) {
			if (buffer[k] >= 33 && buffer[k] <= 126)
				printf("%c", buffer[k]);
			else
				printf(" ");
			k++;
		}
		printf("\n");
	}

	printf("\n\nEND OF VIDEO DATA\n\n");
}
