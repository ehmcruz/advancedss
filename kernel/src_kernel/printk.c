#include "printk.h"
#include "video.h"
#include "vars.h"

struct vargs_t {
	uint64_t *gpr_init;
	uint16_t pos; // witch param we are now
};

typedef struct vargs_t vargs_t;

enum printk_state { PRINTK_NORMAL, PRINTK_SUB, PRINTK_SUB2 };
enum printk_type {
	PRINTK_TYPE_INT32,
	PRINTK_TYPE_UINT32,
	PRINTK_TYPE_UINT32X,
	PRINTK_TYPE_INT64,
	PRINTK_TYPE_UINT64,
	PRINTK_TYPE_UINT64X,
	PRINTK_TYPE_STRING,
	PRINTK_TYPE_CHAR,
	PRINTK_TYPE_INVALID
};

#define vargs_init(VARGS_STC)  \
	__asm__ __volatile__( \
		"lea -0xA0(%%rbp), %0\n" \
		: "=a"(VARGS_STC.gpr_init) \
		); \
		\
		VARGS_STC.pos = 0;

#define vargs_get(VARGS_STC)  \
	VARGS_STC.gpr_init[VARGS_STC.pos++]

static enum printk_type get_type_token(char *str)
{
	enum printk_type token;

	if (!strcmp(str, "i"))
		token = PRINTK_TYPE_INT32;
	else if (!strcmp(str, "lli"))
		token = PRINTK_TYPE_INT64;
	else if (!strcmp(str, "u"))
		token = PRINTK_TYPE_UINT32;
	else if (!strcmp(str, "llu"))
		token = PRINTK_TYPE_UINT64;
	else if (!strcmp(str, "s"))
		token = PRINTK_TYPE_STRING;
	else if (!strcmp(str, "c"))
		token = PRINTK_TYPE_CHAR;
	else if (!strcmp(str, "X"))
		token = PRINTK_TYPE_UINT32X;
	else if (!strcmp(str, "llX"))
		token = PRINTK_TYPE_UINT64X;
	else
		token = PRINTK_TYPE_INVALID;

	return token;
}

#define PRINT_HEX(N, DEST, TYPE, LENGTH)        {   \
		uint8_t high, low; \
		int8_t i, k; \
		\
		LENGTH = 0; \
		k = 0; \
		\
		for (i=sizeof(TYPE)-1; i>=0; i--) { \
			high = N >> (i << 3); \
			low = high & 0x0F; \
			high >>= 4; \
			\
			if (high < 10) \
				DEST[LENGTH++] = high + 48; \
			else \
				DEST[LENGTH++] = high + (65 - 10); \
			\
			if (low < 10) \
				DEST[LENGTH++] = low + 48; \
			else \
				DEST[LENGTH++] = low + (65 - 10); \
		} \
		DEST[LENGTH] = 0; \
	}

static uint8_t print_uint32x(char *buffer, uint32_t n)
{
	uint8_t length;
	PRINT_HEX(n, buffer, uint32_t, length)
	return length;
}

static uint8_t print_uint64x(char *buffer, uint64_t n)
{
	uint8_t length;
	PRINT_HEX(n, buffer, uint64_t, length)
	return length;
}

#define PRINT_UNSIGNED(N, DEST, TYPE, LENGTH)        {   \
		TYPE tmp = N; \
		int8_t i = 0, j = 0, t, len; \
		do { \
			DEST[i++] = (tmp % 10) + 48; \
			tmp /= 10; \
		} while (tmp != 0); \
		LENGTH = i; \
		len = i >> 1; \
		DEST[i--] = 0;  \
		for (j=0; j<len; j++) { \
			t = DEST[j]; \
			DEST[j] = DEST[i]; \
			DEST[i--] = t; \
		} \
	}

static uint8_t print_uint32(char *buffer, uint32_t n)
{
	uint8_t length;
	PRINT_UNSIGNED(n, buffer, uint32_t, length)
	return length;
}

static uint8_t print_int32(char *buffer, int32_t n)
{
	uint8_t length, neg;
	neg = 0;
	if (n < 0) {
		buffer[0] = '-';
		buffer++;
		n = -n;
		neg++;
	}
	PRINT_UNSIGNED(n, buffer, uint32_t, length)
	return length+neg;
}

static uint8_t print_uint64(char *buffer, uint64_t n)
{
	uint8_t length;
	PRINT_UNSIGNED(n, buffer, uint64_t, length)
	return length;
}

static uint8_t print_int64(char *buffer, int64_t n)
{
	uint8_t length, neg;
	neg = 0;
	if (n < 0) {
		buffer[0] = '-';
		buffer++;
		n = -n;
		neg++;
	}
	PRINT_UNSIGNED(n, buffer, uint64_t, length)
	return length+neg;
}

void printk_(video_data_t *video_buffer, char *format, ...)
{
	vargs_t vargs;
	char *s, *out, *t;
	enum printk_state state;
	static char output_string[500];
	static char type[10];
	enum printk_type type_token;
	int must_go_out;

	vargs_init(vargs);
	
	out = output_string;
	state = PRINTK_NORMAL;
	must_go_out = 0;
	
	for (s=format; *s; s++) {
		switch (state) {
			case PRINTK_NORMAL:
				if (*s != '%') {
					*out = *s;
					out++;
				}
				else {
					state = PRINTK_SUB;
				}
			break;

			case PRINTK_SUB:
				if (*s != '%') {
					t = type;
					*t = *s;
					t++;
					state = PRINTK_SUB2;
					
					if (s[1] == 0) {
						must_go_out = 1;
						goto post_process;
					}
				}
				else {
					state = PRINTK_NORMAL;
					*out = *s;
					out++;
				}
			break;

			case PRINTK_SUB2: {
				uint8_t len = 0;
				
				if ((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z')) {
					*t = *s;
					t++;
					
					if (s[1] == 0) {
						must_go_out = 1;
						goto post_process;
					}
				}
				else {
					post_process:
					
					*t = 0;
					type_token = get_type_token(type);
					switch (type_token) {
						case PRINTK_TYPE_UINT32: {
							uint32_t data;
							data = vargs_get(vargs);
							len = print_uint32(out, data);
						}
						break;
						
						case PRINTK_TYPE_INT32: {
							int32_t data;
							data = vargs_get(vargs);
							len = print_int32(out, data);
						}
						break;

						case PRINTK_TYPE_INT64: {
							int64_t data;
							data = vargs_get(vargs);
							len = print_int64(out, data);
						}
						break;
						
						case PRINTK_TYPE_UINT32X: {
							uint32_t data;
							data = vargs_get(vargs);
							len = print_uint32x(out, data);
						}
						break;

						case PRINTK_TYPE_UINT64: {
							uint64_t data;
							data = vargs_get(vargs);
							len = print_uint64(out, data);
						}
						break;
						
						case PRINTK_TYPE_UINT64X: {
							uint64_t data;
							data = vargs_get(vargs);
							len = print_uint64x(out, data);
						}
						break;

						case PRINTK_TYPE_STRING: {
							char *s;
							s = (char*)vargs_get(vargs);
							len = 0;
							while (*s) {
								out[len++] = *s;
								s++;
							}
						}
						break;
						
						case PRINTK_TYPE_CHAR: {
							char c;
							c = vargs_get(vargs);
							*out = c;
							len = 1;
						}
						break;
					}
					state = PRINTK_NORMAL;
					out += len;
					
					if (must_go_out == 0) {
						*out = *s;
						out++;
					}
				}
			}
			break;
		}
	}

	*out = 0;

	video_print_string(video_buffer, output_string);
}
