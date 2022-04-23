#include "utility.h"
#include <cstdio>

const char* size_to_string(char * output, size_t size, bool append_unit) {
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
			snprintf(output, STS_MAX_FORMAT_SIZE, "%zu %c%c (%zu %c%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			snprintf(output, STS_MAX_FORMAT_SIZE, "%zu %c%c (%.1f %ci%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	} else {
		if (index[1] == 0) {
			snprintf(output, STS_MAX_FORMAT_SIZE, "%.1f %c%c (%zu %c%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			snprintf(output, STS_MAX_FORMAT_SIZE, "%.1f %c%c (%.1f %ci%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	}

	return output;
}

const wchar_t* size_to_wstring(wchar_t * output, size_t size, bool append_unit) {
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
			_snwprintf_s(output, STS_MAX_FORMAT_SIZE, STS_MAX_FORMAT_SIZE, L"%zu %c%c (%zu %c%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			_snwprintf_s(output, STS_MAX_FORMAT_SIZE, STS_MAX_FORMAT_SIZE, L"%zu %c%c (%.1f %ci%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	} else {
		if (index[1] == 0) {
			_snwprintf_s(output, STS_MAX_FORMAT_SIZE, STS_MAX_FORMAT_SIZE, L"%.1f %c%c (%zu %c%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			_snwprintf_s(output, STS_MAX_FORMAT_SIZE, STS_MAX_FORMAT_SIZE, L"%.1f %c%c (%.1f %ci%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	}

	return output;
}

