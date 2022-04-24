#include "page.h"
#include "device.h"
#include "utility.h"
#include <cstdio>
#include <cstring>

#define PAGE_BAR \
	"\033[2m" \
	"+---+------------+-------------------------------------------------------------------------------------------------------+------------------------------------+"\
	"\033[0m"
#define PAGE_HDR \
	"\033[2m" \
	"|   |   Offset   |  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F  |                Text                |" \
	"\033[0m"

//	"|   |   Offset   |  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F  |  0123456789ABCDEF0123456789ABCDEF  |"

#define PAGE_FTR \
	"\033[2m" \
	"|   | %-10llu |  %-50s Cluster: %-15llu Sector: %-15llu  |                                    |" \
	"\033[0m"

#define PAGE_ADR \
	"\033[2m| %X |  %08llX  |  \033[0m"

#define PAGE_SEP0 \
	" "

#define PAGE_SEP1 \
	"  "

#define PAGE_SEP2 \
	"\033[2m |  \033[0m"

#define PAGE_LMARGIN \
	"  \033[36G"

#define PAGE_RMARGIN \
	"  \033[2m|\033[0m"

#define PAGE_LE \
	"\033[1B"


#define PAGE_SAVE "\033[1;1H"
#define PAGE_LOAD "\033[1;1H"

void Page::print_hdr(void) const {
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
	printf(PAGE_LMARGIN PAGE_HDR PAGE_LE);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
}

void Page::print_ftr(void) const {
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
	printf(PAGE_LMARGIN PAGE_FTR PAGE_LE,
		_offset,
		size_to_string(_offset, true),
		_offset /_clustr_length,
		_offset /_sector_length
	);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
	printf("\n\n");
}

//// Data line BEGIN
void Page::print_adr(int nline, ULONGLONG offset) const {
	printf(PAGE_LMARGIN PAGE_ADR, nline, offset);
}

void Page::print_hex_block(PBYTE line, int i0, int i1) const {
	for (int i =  i0; i < i1; ++i) {
		switch(_mode) {
		case Mode::CHR:
			printf("%s ", colorize_char(line[i], 2));
			break;
		case Mode::CHX:
			printf("%s ", colorize_char(line[i], " .", 2));
			break;
		case Mode::HEX:
		default:
			printf("%02X ", line[i]);
			break;
		}
	}
}
void Page::print_hex(PBYTE line, int len) const {
	static const int q1 = 0x08;
	static const int q2 = 0x10;
	static const int q3 = 0x18;
	print_hex_block(line,  0,  q1); printf(PAGE_SEP0);
	print_hex_block(line, q1,  q2); printf(PAGE_SEP1);
	print_hex_block(line, q2,  q3); printf(PAGE_SEP0);
	print_hex_block(line, q3, len); printf(PAGE_SEP2);
}

void Page::print_str(PBYTE line, int len) const {
	for (int i = 0x00; i < len; ++i) {
		char c = line[i];
		printf("%s", colorize_char(c, '.'));
	}
	printf(PAGE_RMARGIN PAGE_LE);
}
//// Data line END


#define LINE_WIDTH 0x20
void Page::print(void) const {
	PBYTE end = _buffer + _sector_length;
	PBYTE pos = _buffer;

	printf(PAGE_LOAD);

	print_hdr();
	int nline = 0;
	while(pos < end) {
		print_adr(nline++, _offset + pos - _buffer);
		print_hex(pos, LINE_WIDTH);
		print_str(pos, LINE_WIDTH);
		pos += LINE_WIDTH;
	}
	print_ftr();
}

