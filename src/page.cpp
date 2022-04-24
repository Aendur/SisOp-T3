#include "page.h"
#include "term_ui.h"
#include "utility.h"
#include <cstdio>
#include <cstring>

#define PAGE_BAR \
	"\033[2m" \
	"+------------+-------------------------------------------------------------------------------------------------------+------------------------------------+"\
	"\033[0m"
#define PAGE_HDR \
	"\033[2m" \
	"|  Address   |  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F  |  0123456789ABCDEF0123456789ABCDEF  |" \
	"\033[0m"
//	"|  Address   |  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F   10 11 12 13 14 15 16 17  18 19 1A 1B 1C 1D 1E 1F  |                Text                |"
#define PAGE_FTR \
	"\033[2m" \
	"|  %-10llu B %-50s Sector: %-77lld  |" \
	"\033[0m"
#define PAGE_ADR \
	"\033[2m|  %08llX  |  \033[0m"

#define PAGE_SEP0 \
	" "

#define PAGE_SEP1 \
	"  "

#define PAGE_SEP2 \
	"\033[2m |  \033[0m"

#define PAGE_LMARGIN \
	"  \033[40G"

#define PAGE_RMARGIN \
	"  \033[2m|\033[0m"

#define PAGE_LE \
	"\033[1B"


#define PAGE_SAVE L"\033[1;1H"
#define PAGE_LOAD L"\033[1;1H"

void Page::print_hdr(void) const {
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
	printf(PAGE_LMARGIN PAGE_HDR PAGE_LE);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
}

void Page::print_ftr(void) const {
	char buf[STS_MAX_FORMAT_SIZE];
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
	printf(PAGE_LMARGIN PAGE_FTR PAGE_LE, _offset, size_to_string(buf, _offset, true), _offset / _length);
	printf(PAGE_LMARGIN PAGE_BAR PAGE_LE);
	printf("\n\n");
}

//// Data line BEGIN
void Page::print_adr(ULONGLONG offset) const {
	printf(PAGE_LMARGIN PAGE_ADR, offset);
}

static char color_char_buffer[32];
const char * colorize_char(char c, char ctl) {
	if      (0x20 <= c && c <= 0x7E) { snprintf(color_char_buffer, 32, "%c", c); }
	else if (             c == 0   ) { snprintf(color_char_buffer, 32, "\033[31;2m%c\033[0m", ctl); }
	else                             { snprintf(color_char_buffer, 32, "\033[31;1m%c\033[0m", ctl); }
	return color_char_buffer;
}
const char * colorize_char(char c, const char * ctl, int width) {
	if      (0x20 <= c && c <= 0x7E) { snprintf(color_char_buffer, 32, "%*c", width, c); }
	else if (             c == 0   ) { snprintf(color_char_buffer, 32, "\033[31;2m%*s\033[0m", width, ctl); }
	else if (             c == '\t') { snprintf(color_char_buffer, 32, "\033[31;1m%*s\033[0m" , width, "\\t"); }
	else if (             c == '\n') { snprintf(color_char_buffer, 32, "\033[31;1m%*s\033[0m" , width, "\\n"); }
	else if (             c == '\r') { snprintf(color_char_buffer, 32, "\033[31;1m%*s\033[0m" , width, "\\r"); }
	else                             { snprintf(color_char_buffer, 32, "\033[31;1m%*s\033[0m", width, ctl); }
	return color_char_buffer;
}
const char * colorize_char(unsigned char c, int width) {
	if      (0x20 <= c && c <= 0x7E) { snprintf(color_char_buffer, 32, "\033[1m%*c\033[0m"    , width,     c); }
	else if (             c == 0   ) { snprintf(color_char_buffer, 32, "\033[31;2m%0*X\033[0m", width,     c); }
	else if (             c == '\t') { snprintf(color_char_buffer, 32, "\033[31;1m%*s\033[0m" , width, "\\t"); }
	else if (             c == '\n') { snprintf(color_char_buffer, 32, "\033[31;1m%*s\033[0m" , width, "\\n"); }
	else if (             c == '\r') { snprintf(color_char_buffer, 32, "\033[31;1m%*s\033[0m" , width, "\\r"); }
	else                             { snprintf(color_char_buffer, 32, "\033[31m%0*X\033[0m"  , width,     c); }
	return color_char_buffer;
}

void Page::print_hex_block(PBYTE line, int i0, int i1) const {
	for (int i =  i0; i < i1; ++i) {
		switch(_mode) {
		case Mode::CHR:
			printf("%s ", colorize_char(line[i], 2));
			break;
		case Mode::CHX:
			printf("%s ", colorize_char(line[i], "..", 2));
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
void Page::print(bool reset) const {
	PBYTE end = _buffer + _length;
	PBYTE pos = _buffer;
	// printf("buffer length: %d\n", len);
	// printf("read bytes: %d\n", nbytes);
	
	if (reset) {
		_ui->write(PAGE_LOAD);
		//_ui->load_position();
	} else {
		_ui->write(PAGE_SAVE);
		//_ui->save_position();
	}
	print_hdr();
	while(pos < end) {
		print_adr(_offset + pos - _buffer);
		print_hex(pos, LINE_WIDTH);
		print_str(pos, LINE_WIDTH);
		pos += LINE_WIDTH;
	}
	print_ftr();
}

