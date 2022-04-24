#include "utility.h"
#include <cstdio>

#define FORMAT_SIZE 64
static char sts_buffer[FORMAT_SIZE];
static wchar_t sts_wbuffer[FORMAT_SIZE];

const char* size_to_string(size_t size, bool append_unit) {
	const char    unit[ ] = {' ', 'B'};
	const char  suffix[ ] = {' ', 'k', 'M', 'G', 'T'};
	const size_t isize[2] = { size,  size };
	      double fsize[2] = { -1.0,  -1.0 };
	      int    index[2] = {    0,     0 };
	const size_t    it[2] = { 1000, 0x400 };
	const double    ft[2] = { (double) 1000, (double) 0x400 };

	for (int i = 0; i < 2; ++i) {
		if (isize[i] >= it[i]) {
			fsize[i] = (double) isize[i] / ft[i];
			++index[i];
			while (fsize[i] >= ft[i] - 0.05) {
				fsize[i] /= ft[i];
				++index[i];
			}
		}
	}

	if (index[0] == 0) {
		if (index[1] == 0) {
			snprintf(sts_buffer, FORMAT_SIZE, "%zu %c%c (%zu %c%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			snprintf(sts_buffer, FORMAT_SIZE, "%zu %c%c (%.1f %ci%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	} else {
		if (index[1] == 0) {
			snprintf(sts_buffer, FORMAT_SIZE, "%.1f %c%c (%zu %c%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			snprintf(sts_buffer, FORMAT_SIZE, "%.1f %c%c (%.1f %ci%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	}

	return sts_buffer;
}

const wchar_t* size_to_wstring(size_t size, bool append_unit) {
	const wchar_t   unit[ ] = {L' ', L'B'};
	const wchar_t suffix[ ] = {L' ', L'k', L'M', L'G', L'T'};
	const size_t   isize[2] = { size,  size };
	      double   fsize[2] = { -1.0,  -1.0 };
	      int      index[2] = {    0,     0 };
	const size_t      it[2] = { 1000, 0x400 };
	const double      ft[2] = { (double) 1000, (double) 0x400 };

	for (int i = 0; i < 2; ++i) {
		if (isize[i] >= it[i]) {
			fsize[i] = (double) isize[i] / ft[i];
			++index[i];
			while (fsize[i] >= ft[i] - 0.05) {
				fsize[i] /= ft[i];
				++index[i];
			}
		}
	}

	if (index[0] == 0) {
		if (index[1] == 0) {
			_snwprintf_s(sts_wbuffer, FORMAT_SIZE, FORMAT_SIZE, L"%zu %c%c (%zu %c%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			_snwprintf_s(sts_wbuffer, FORMAT_SIZE, FORMAT_SIZE, L"%zu %c%c (%.1f %ci%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	} else {
		if (index[1] == 0) {
			_snwprintf_s(sts_wbuffer, FORMAT_SIZE, FORMAT_SIZE, L"%.1f %c%c (%zu %c%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			_snwprintf_s(sts_wbuffer, FORMAT_SIZE, FORMAT_SIZE, L"%.1f %c%c (%.1f %ci%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	}

	return sts_wbuffer;
}

static char color_char_buffer[FORMAT_SIZE];
const char * colorize_char(char c, char ctl) {
	if      (0x20 <= c && c <= 0x7E) { snprintf(color_char_buffer, FORMAT_SIZE, "%c", c); }
	else if (             c == 0   ) { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;2m%c\033[0m", ctl); }
	else                             { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%c\033[0m", ctl); }
	return color_char_buffer;
}
const char * colorize_char(unsigned char c, int width) {
	if      (0x20 <= c && c <= 0x7E) { snprintf(color_char_buffer, FORMAT_SIZE, "\033[1m%*c\033[0m"    , width,   c); }
	else if (             c == 0   ) { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;2m%0*X\033[0m", width,   c); }
	else if (             c == '\t') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m" , width, "T"); }
	else if (             c == '\n') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m" , width, "N"); }
	else if (             c == '\r') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m" , width, "R"); }
	else                             { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31m%0*X\033[0m"  , width,   c); }
	return color_char_buffer;
}
const char * colorize_char(char c, const char * ctl, int width) {
	if      (0x20 <= c && c <= 0x7E) { snprintf(color_char_buffer, FORMAT_SIZE, "%*c", width, c); }
	else if (             c == 0   ) { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;2m%*s\033[0m", width, ctl); }
	else if (             c == '\t') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m", width, "T"); }
	else if (             c == '\n') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m", width, "N"); }
	else if (             c == '\r') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m", width, "R"); }
	else                             { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m", width, ctl); }
	return color_char_buffer;
}
// const char * colorize_char(char c, const char * ctl, int width, const char * spacer) {
// 	if      (0x20 <= c && c <= 0x7E) { snprintf(color_char_buffer, FORMAT_SIZE,                  "%*c%s", width,   c, spacer); }
// 	else if (             c == 0   ) { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;2m%*s\033[0m%s", width, ctl, spacer); }
// 	else if (             c == '\t') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m%s", width, "T", spacer); }
// 	else if (             c == '\n') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m%s", width, "N", spacer); }
// 	else if (             c == '\r') { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m%s", width, "R", spacer); }
// 	else                             { snprintf(color_char_buffer, FORMAT_SIZE, "\033[31;1m%*s\033[0m%s", width, ctl, spacer); }
// 	return color_char_buffer;
// }