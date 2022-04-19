#include "random_file.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

#define FORMAT_SIZE 32
const char* size_to_string(char * output, size_t size, bool append_unit) {
	
	static const char     unit[] = {' ', 'B'};
	static const char   suffix[] = {' ', 'k', 'M', 'G', 'T'};
	static const size_t isize[2] = { size,  size };
	             double fsize[2] = { -1.0,  -1.0 };
	             int    index[2] = {    0,     0 };
	static const size_t    it[2] = { 1000, 0x400 };
	static const double    ft[2] = { (double) 1000, (double) 0x400 };

	for (int i = 0; i < 2; ++i) {
		if (isize[i] >= it[i]) {
			fsize[i] = isize[i] / ft[i];
			++index[i];
			while (fsize[i] >= ft[i] - 0.05) {
				fsize[i] /= ft[i];
				++index[i];
			}
		}
	}

	if (index[0] == 0) {
		if (index[1] == 0) {
			snprintf(output, FORMAT_SIZE, "%lu %c%c (%lu %c%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			snprintf(output, FORMAT_SIZE, "%lu %c%c (%.1f %ci%c)",
				isize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	} else {
		if (index[1] == 0) {
			snprintf(output, FORMAT_SIZE, "%.1f %c%c (%lu %c%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				isize[1], suffix[index[1]], unit[append_unit]
			);
		} else {
			snprintf(output, FORMAT_SIZE, "%.1f %c%c (%.1f %ci%c)",
				fsize[0], suffix[index[0]], unit[append_unit],
				fsize[1], suffix[index[1]], unit[append_unit]
			);
		}
	}

	return output;
}

int main (int argc, char ** argv) {
	if (argc < 3) {
		std::cout << "a.out file_name file_size\n\n";
		std::cout << "      file_name max 25 characters\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	try {
		size_t fsize = std::stoull(argv[2]);
		char size_str[FORMAT_SIZE];
		std::cout << "Creating file: " << argv[1] << " - " << size_to_string(size_str, fsize, true) << "\n";
		RandomFile rf(argv[1], fsize);
		rf.write();
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

