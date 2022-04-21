#include "utility.h"
#include <cstdio>

const char* size_to_string(char * output, size_t size, bool append_unit) {
	const char     unit[] = {' ', 'B'};
	const char   suffix[] = {' ', 'k', 'M', 'G', 'T'};
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

//cl /Wall /W4 /EHsc /std:c++17 /Iheader /DUNITTEST source/utility.cpp
#ifdef UNITTEST
#include <cstring>
int main () {
	char buffer[STS_MAX_FORMAT_SIZE];
	size_t inc = 1;
	for (size_t T = 900; T <= 900000000000; T*=1000) {
		for (size_t siz = T; siz <= T+(200 * inc); siz += inc) {
			size_t len = strlen(size_to_string(buffer, siz, true));
			printf("%zu %zu %s\n", len, siz, buffer);
		}
		inc *= 1000;
	}
}
#endif
