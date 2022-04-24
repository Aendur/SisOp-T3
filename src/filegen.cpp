#include "random_file.h"
#include "utility.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

int get_len(int x) {
	int len = 1;
	while (x > 9) {
		x /= 10;
		++len;
	}
	return len;
}

char * find_dot(char * str) {
	printf("search dot @ %p\n", str);
	++str;
	while(*str != '\0') {
		if (*str == '.') {
			printf("found dot @ %p\n", str);
			return str;
		}
		++str;
	}
	return NULL;
}

const char * get_filename(char * name, int current, int N) {
	if (N == 1) return name;
	if (strlen(name) < 2) return name;
	
	static const int BUFSIZE = 64;
	static char file_name_buffer[BUFSIZE];
	static char * ext = find_dot(name);
	if (ext != NULL) {
		*ext = 0;
		int L = get_len(N);
		snprintf(file_name_buffer, BUFSIZE, "%s%0*d.%s", name, L, current + 1, ext + 1);
		return file_name_buffer;
	} else {
		int L = get_len(N);
		snprintf(file_name_buffer, BUFSIZE, "%s%0*d", name, L, current + 1);
		return file_name_buffer;
	}

}

int main (int argc, char ** argv) {
	srand((unsigned int) time(NULL));
	if (argc < 4) {
		std::cout << "a.out file_name file_size num_files [dry_run]\n\n";
		std::cout << "      file_name max 25 characters\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	bool dry_run = argc == 5;
	try {
		size_t fsize = std::stoull(argv[2]);
		int max_files = std::stoi(argv[3]);
		for (int i = 0; i < max_files; ++i) {
			const char * fname = get_filename(argv[1], i, max_files);
			std::cout << "Creating file: " << fname << " - " << size_to_string(fsize, true) << "\n";
			if (!dry_run) {
				RandomFile rf(fname, fsize);
				rf.write();
			}
		}
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

