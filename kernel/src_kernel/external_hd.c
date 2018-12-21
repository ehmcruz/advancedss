#include "external_hd.h"
#include "printk.h"

struct external_hd_operation {
	external_hd_file_descriptor *fd;
	uint32_t len;
	uint64_t intermediate;
	void *buffer;
	uint8_t op;
	void (*callback)(uint64_t, void*);
	void *requester_data;
};

static uint32_t list_start, list_end, list_available;
static uint8_t i_am_idle;

typedef struct external_hd_operation external_hd_operation;

static external_hd_operation operations[CONFIG_KERNEL_MAX_HD_OPERATIONS];

#define EXTERNAL_HD_OPERATION_READ       0
#define EXTERNAL_HD_OPERATION_WRITE      1
#define EXTERNAL_HD_OPERATION_SEEK       2
#define EXTERNAL_HD_OPERATION_OPEN       3
#define EXTERNAL_HD_OPERATION_CLOSE      4

#define IO_EXTERNAL_HD_CMD_FNAME_START       0
#define IO_EXTERNAL_HD_CMD_FNAME_END         1
#define IO_EXTERNAL_HD_CMD_OPEN              2
#define IO_EXTERNAL_HD_CMD_CLOSE             3
#define IO_EXTERNAL_HD_CMD_READ_SECTOR       4
#define IO_EXTERNAL_HD_CMD_WRITE_SECTOR      5
#define IO_EXTERNAL_HD_CMD_SEEK              6
#define IO_EXTERNAL_HD_CMD_GET_FILESIZE      9
#define IO_EXTERNAL_HD_CMD_WRITE_FILE_BUFFER 10
#define IO_EXTERNAL_HD_CMD_GET_FILEPOS       13

#define IO_PORTS_READ_EXTERNAL_HD_STATUS     0x2000
#define IO_PORTS_READ_EXTERNAL_HD_DATA       0x2001
#define IO_PORTS_READ_EXTERNAL_HD_FBYTES     0x2002
#define IO_PORTS_READ_EXTERNAL_HD_FHANDLER   0x2003

#define IO_PORTS_WRITE_EXTERNAL_HD_CMD       0x2000
#define IO_PORTS_WRITE_EXTERNAL_HD_DATA      0x2001
#define IO_PORTS_WRITE_EXTERNAL_HD_FBYTES    0x2002
#define IO_PORTS_WRITE_EXTERNAL_HD_FHANDLER  0x2003

#define IO_EXTERNAL_HD_STATE_IDLE            0
#define IO_EXTERNAL_HD_STATE_WAIT_FNAME      1
#define IO_EXTERNAL_HD_STATE_WAIT_SEEK       2
#define IO_EXTERNAL_HD_STATE_READING_EXT_HD  3
#define IO_EXTERNAL_HD_STATE_WRITTING_EXT_HD 4
#define IO_EXTERNAL_HD_STATE_WAIT_READ       5
#define IO_EXTERNAL_HD_STATE_WAIT_WBUFFER    9

#define GET_STATE   (external_hd_read_status() & 0x3F)

/****************/

void external_hd_process_write_file(external_hd_file_descriptor *fd, uint32_t len, void *buffer);
uint8_t external_hd_process_open_file(external_hd_file_descriptor *fd);
uint8_t external_hd_read_status();
void external_hd_next_request();
void external_hd_set_file_handler(external_hd_file_descriptor *fd);
void external_hd_request_processed(uint64_t r);
void external_hd_request_job(external_hd_file_descriptor *fd, uint8_t op, uint32_t len, void *buffer, void (*callback)(uint64_t, void*), void *requester_data);
void external_hd_process_read_file_after_interrupt(external_hd_file_descriptor *fd, void *buffer);
void external_hd_process_write_file_after_interrupt(external_hd_file_descriptor *fd);

/****************/

void initialize_external_hd()
{
	list_start = 0;
	list_end = 0;
	list_available = CONFIG_KERNEL_MAX_HD_OPERATIONS;
	i_am_idle = 1;
}

void external_hd_request_open_file(external_hd_file_descriptor *fd, char *fname, void (*callback)(uint64_t, void*), void *requester_data)
{
	strcpy(fd->fname, fname);
	external_hd_request_job(fd, EXTERNAL_HD_OPERATION_OPEN, 0, NULL, callback, requester_data);
}

void external_hd_request_read_file(external_hd_file_descriptor *fd, void *buffer, uint32_t len, void (*callback)(uint64_t, void*), void *requester_data)
{
	uint8_t *b;
	uint32_t i, ac;
	
	// we have to break the read into various reads of CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE
	
	b = buffer;
	ac = 0;
	
	for (i=CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE; i<len; i+=CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE) {
		external_hd_request_job(fd, EXTERNAL_HD_OPERATION_READ, CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE, b, NULL, NULL);
		b += CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE;
		ac += CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE;
		
	}
	external_hd_request_job(fd, EXTERNAL_HD_OPERATION_READ, len - ac, b, callback, requester_data);
}

void external_hd_request_write_file(external_hd_file_descriptor *fd, void *buffer, uint32_t len, void (*callback)(uint64_t, void*), void *requester_data)
{
	uint8_t *b;
	uint32_t i, ac;
	
	// we have to break the read into various reads of CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE
	
	b = buffer;
	ac = 0;
	
	for (i=CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE; i<len; i+=CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE) {
		external_hd_request_job(fd, EXTERNAL_HD_OPERATION_WRITE, CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE, b, NULL, NULL);
		b += CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE;
		ac += CONFIG_KERNEL_EXTERNAL_HD_SECTOR_SIZE;
		
	}
	external_hd_request_job(fd, EXTERNAL_HD_OPERATION_WRITE, len - ac, b, callback, requester_data);
}

void external_hd_request_close_file(external_hd_file_descriptor *fd, void (*callback)(uint64_t, void*), void *requester_data)
{
	external_hd_request_job(fd, EXTERNAL_HD_OPERATION_CLOSE, 0, NULL, callback, requester_data);
}

// add a job to queue

void external_hd_request_job(external_hd_file_descriptor *fd, uint8_t op, uint32_t len, void *buffer, void (*callback)(uint64_t, void*), void *requester_data)
{
	KERNEL_ASSERT(list_available > 0);
	//printk("requested %u handler=%llu\n", list_end, callback);
	list_available--;
	operations[list_end].fd = fd;
	operations[list_end].len = len;
	operations[list_end].buffer = buffer;
	operations[list_end].callback = callback;
	operations[list_end].op = op;
	operations[list_end].requester_data = requester_data;
	operations[list_end].intermediate = 0;
	list_end++;
	if (list_end == CONFIG_KERNEL_MAX_HD_OPERATIONS)
		list_end = 0;

	if (i_am_idle)
		external_hd_next_request();
}

uint8_t external_hd_read_status()
{
	return inportb(IO_PORTS_READ_EXTERNAL_HD_STATUS);
}

// called when an interrupt happens

void external_hd_interrupt()
{
	interrupt_happened();
	printk("external hd interrupt!\n");
	if (operations[list_start].op == EXTERNAL_HD_OPERATION_READ)
		external_hd_process_read_file_after_interrupt(operations[list_start].fd, operations[list_start].buffer);
	else if (operations[list_start].op == EXTERNAL_HD_OPERATION_WRITE)
		external_hd_process_write_file_after_interrupt(operations[list_start].fd);
	else {
		printk("interrupt: unknown external hd operation");
		panic();
	}
	process_scheduler_schedule();
}

void external_hd_set_file_handler(external_hd_file_descriptor *fd)
{
	outportd(IO_PORTS_WRITE_EXTERNAL_HD_FHANDLER, fd->id);
	outportd(IO_PORTS_WRITE_EXTERNAL_HD_FHANDLER, fd->id >> 32);
}

// called when a request ended

void external_hd_request_processed(uint64_t r)
{//printk("request processed %u handler=%llu\n", list_start, operations[list_start].callback);
	uint8_t must_add;
	uint64_t to_add;
	
	if (operations[list_start].callback != NULL) {
		(*operations[list_start].callback)(r + operations[list_start].intermediate, operations[list_start].requester_data);
		must_add = 0;
	}
	else {
		must_add = 1;
		to_add = operations[list_start].intermediate;
	}
	list_available++;
	list_start++;

	printk("external hd: request processed (remainning %u)\n", CONFIG_KERNEL_MAX_HD_OPERATIONS - list_available);	

	if (list_start == CONFIG_KERNEL_MAX_HD_OPERATIONS)
		list_start = 0;
	if (must_add)
		operations[list_start].intermediate = r + to_add;
	i_am_idle = 1;
	KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_IDLE);
	if (list_available < CONFIG_KERNEL_MAX_HD_OPERATIONS)
		external_hd_next_request();
}

// process the next request of queue

void external_hd_next_request()
{
	i_am_idle = 0;

	KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_IDLE);
	
	switch (operations[list_start].op) {
		case EXTERNAL_HD_OPERATION_READ: {
			external_hd_set_file_handler(operations[list_start].fd);
			outportd(IO_PORTS_WRITE_EXTERNAL_HD_FBYTES, operations[list_start].len);
//printk("lllllllllllllllll file bytes is %u\n", inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES));
			outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_READ_SECTOR);
		}
		break;

		case EXTERNAL_HD_OPERATION_WRITE: {
			external_hd_process_write_file(operations[list_start].fd, operations[list_start].len, operations[list_start].buffer);
		}
		break;

		case EXTERNAL_HD_OPERATION_SEEK: {
			uint32_t tmp;
			external_hd_set_file_handler(operations[list_start].fd);
			outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_SEEK);
			KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_WAIT_SEEK);
			outportd(IO_PORTS_WRITE_EXTERNAL_HD_DATA, operations[list_start].len);
			KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_IDLE);
			outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_GET_FILEPOS);
			tmp = inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES);
			operations[list_start].fd->pos= tmp;
			external_hd_request_processed(tmp == operations[list_start].len);
		}
		break;

		case EXTERNAL_HD_OPERATION_OPEN: {
			uint8_t ret;
			ret = external_hd_process_open_file(operations[list_start].fd);
			external_hd_request_processed(ret);
		}
		break;

		case EXTERNAL_HD_OPERATION_CLOSE: {
			external_hd_set_file_handler(operations[list_start].fd);
			outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_CLOSE);
			external_hd_request_processed(1);
		}
		break;

		default:
			printk("unknown external hd operation");
			panic();
	}
}

uint8_t external_hd_process_open_file(external_hd_file_descriptor *fd)
{
	char *f;
	uint64_t tmp;

	// send file name to controller
	f = fd->fname;
	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_FNAME_START);
	while (*f) {
		outportb(IO_PORTS_WRITE_EXTERNAL_HD_DATA, *f);
		f++;
	}
	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_FNAME_END);

	KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_IDLE);

	// open the file
	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_OPEN);

	// now, save the handler
	tmp = inportd(IO_PORTS_READ_EXTERNAL_HD_FHANDLER);
	fd->id = tmp;
	tmp = inportd(IO_PORTS_READ_EXTERNAL_HD_FHANDLER);
	fd->id |= tmp << 32;

	if (fd->id == NULL)
		return 0;

	// now, check the file size
	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_GET_FILESIZE);
	fd->fsize = inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES);

	// set position to start of the file
	fd->pos = 0;

	return 1;
}

void external_hd_process_write_file(external_hd_file_descriptor *fd, uint32_t len, void *buffer)
{
	uint32_t i;
	uint8_t *b;

	b = (uint8_t*)buffer;
	external_hd_set_file_handler(fd);

	outportd(IO_PORTS_WRITE_EXTERNAL_HD_FBYTES, len);
	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_WRITE_FILE_BUFFER);
	KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_WAIT_WBUFFER);
	for (i=0; i<len; i++) {
		outportb(IO_PORTS_WRITE_EXTERNAL_HD_DATA, b[i]);
	}
	
	KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_IDLE);
	
	//outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_GET_FILEPOS);
	//fd->pos = inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES);

	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_WRITE_SECTOR);
}

void external_hd_process_read_file_after_interrupt(external_hd_file_descriptor *fd, void *buffer)
{
	uint32_t i, read;
	uint8_t *b;
	uint8_t state;
//printk("i was reading the hd\n");
	b = (uint8_t*)buffer;

	read = inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES);
	
	if (read > 0) {
		KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_WAIT_READ);

	//printk("must read %u bytes\n", read);
		for (i=0; i<read; i++) {
			b[i] = inportb(IO_PORTS_READ_EXTERNAL_HD_DATA);
		}
	}

	state = GET_STATE;
	if(state != IO_EXTERNAL_HD_STATE_IDLE) {
		printk("external_hd::read_ai  state is 0x%X  bytes_read=%u", state, read);
		panic();
	}

	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_GET_FILEPOS);
	fd->pos = inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES);

	external_hd_request_processed(read);
}

void external_hd_process_write_file_after_interrupt(external_hd_file_descriptor *fd)
{
	uint32_t written;

	KERNEL_ASSERT(GET_STATE == IO_EXTERNAL_HD_STATE_IDLE);

	written = inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES);

	outportb(IO_PORTS_WRITE_EXTERNAL_HD_CMD, IO_EXTERNAL_HD_CMD_GET_FILEPOS);
	fd->pos = inportd(IO_PORTS_READ_EXTERNAL_HD_FBYTES);

	external_hd_request_processed(written);
}
