#include "utility.h"
#include <cstdio>

#define FORMAT_SIZE 128
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

#define NEGATIVE "\033[7m"
#define EMPTYSTR ""
#define ATTRSTR "%s"
#define HEXSTR "%02X"
#define CHRSTR "%*c"
#define CTLSTR "%*s"
#define LMAR "%*s"
#define RMAR "%*s"
#define LPAD "%*s"
#define RPAD "%*s"
#define CLRSTR "\033[0m"
#define RED_0 "\033[31m"
#define RED_1 "\033[31;1m"
#define RED_2 "\033[31;2m"
#define WHITE_0 "\033[37;0m"
#define WHITE_1 "\033[37;1m"
#define WHITE_2 "\033[37;2m"

const char * colorize_byte(const ColorizeOptions & opts) {
	static char color_char_buffer[FORMAT_SIZE];
	const char * attr_negative = opts.negative ? NEGATIVE : EMPTYSTR;
	const char * attr_color_ctl = opts.chr_hex ? WHITE_0 : (opts.byte == 0 ? RED_2 : RED_0);
	const char * attr_color_chr = WHITE_1; // : WHITE_0;

	if (0x20 <= opts.byte && opts.byte <= 0x7E) {
		if (opts.chr_hex) {
			snprintf(color_char_buffer, FORMAT_SIZE,
				LMAR ATTRSTR ATTRSTR LPAD HEXSTR RPAD CLRSTR RMAR,
				opts.margin_left, EMPTYSTR,
				attr_color_chr,
				attr_negative,
				opts.padding_left, EMPTYSTR,
				opts.byte,
				opts.padding_right, EMPTYSTR,
				opts.margin_right, EMPTYSTR
			);
		} else {
			snprintf(color_char_buffer, FORMAT_SIZE,
				LMAR ATTRSTR ATTRSTR LPAD CHRSTR RPAD CLRSTR RMAR,
				opts.margin_left, EMPTYSTR,
				attr_color_chr,
				attr_negative,
				opts.padding_left, EMPTYSTR,
				opts.width, opts.byte,
				opts.padding_right, EMPTYSTR,
				opts.margin_right, EMPTYSTR
			);
		}
	} else {
		#pragma message ("warning: FIX THIS")
		if (opts.ctl_str == NULL) {
			snprintf(color_char_buffer, FORMAT_SIZE,
				LMAR ATTRSTR ATTRSTR LPAD HEXSTR RPAD CLRSTR RMAR,
				opts.margin_left, EMPTYSTR,
				attr_color_ctl,
				attr_negative,
				opts.padding_left, EMPTYSTR,
				opts.byte,
				opts.padding_right, EMPTYSTR,
				opts.margin_right, EMPTYSTR
			);
		} else {
			snprintf(color_char_buffer, FORMAT_SIZE,
				LMAR ATTRSTR ATTRSTR LPAD CTLSTR RPAD CLRSTR RMAR,
				opts.margin_left, EMPTYSTR,
				attr_color_ctl,
				attr_negative,
				opts.padding_left, EMPTYSTR,
				opts.width, opts.ctl_str,
				opts.padding_right, EMPTYSTR,
				opts.margin_right, EMPTYSTR
			);
		}
	}
	return color_char_buffer;
}
