#include "page.h"
#include <cstdio>

#define PAGE_HDR \
	"\033[2m" \
	"-----------|-------------------------------------------------------------------------------------------------------|----------------------------------\n" \
	"  Address  |  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F  |                Text\n" \
	"-----------|-------------------------------------------------------------------------------------------------------|----------------------------------\n"

void Page::print_hdr(void) const {
	printf(PAGE_HDR);
}

void Page::print_adr(long long offset) const {
	printf("\033[2m  %07llx  |  \033[0m", offset);
}

void Page::print_hex(PBYTE line, int len) const {
	static const int q1 = 0x08;
	static const int q2 = 0x10;
	static const int q3 = 0x18;
	for (int i =  0; i < q1; ++i) { printf("%02x ", line[i]); }
	for (int i = q1; i < q2; ++i) { printf(" %02x", line[i]); }
	printf("   ");
	for (int i = q2; i < q3; ++i)  { printf("%02x ", line[i]); }
	for (int i = q3; i < len; ++i) { printf(" %02x", line[i]); }
	printf("\033[2m  |  \033[0m");
}

void Page::print_str(PBYTE line, int len) const {
	for (int i = 0x00; i < len; ++i) {
		char c = line[i];
		if (0x20 <= c && c <= 0x7E) {
			printf("%c", c);
		} else if (c == 0) {
			printf("\033[31;2m.\033[0m");
		} else {
			printf("\033[31;1m.\033[0m");
		}
	}
	printf("\n");
}

#define LINE_WIDTH 0x20
void Page::print_buf(PBYTE buffer, DWORD len, DWORD nbytes) const {
	PBYTE end = buffer + len;
	PBYTE pos = buffer;
	printf("buffer length: %d\n", len);
	printf("read bytes: %d\n", nbytes);
	
	print_hdr();
	while(pos < end) {
		print_adr(pos - buffer);
		print_hex(pos, LINE_WIDTH);
		print_str(pos, LINE_WIDTH);
		pos += LINE_WIDTH;
	}
}
