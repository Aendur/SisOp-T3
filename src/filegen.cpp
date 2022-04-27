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
	char * result = str + strlen(str);
	while(result >= str) {
		if (*result == '.') {
			printf("found dot @ %p\n", result);
			return result;
		}
		--result;
	}
	return NULL;
}


static char extension_buffer[4];
const char * get_filename(char * name, int current, int N) {
	static const int BUFSIZE = 256;
	static char base_name_buffer[BUFSIZE];

	if (strlen(name) < 3) {
		snprintf(extension_buffer, 4, "");
		return name;
	} 
	
	static char * ext = find_dot(name);
	if (ext != NULL) {
		*ext = 0;
		snprintf(extension_buffer, 4, "%s", ext + 1);
		if (N == 1) {
			snprintf(base_name_buffer, BUFSIZE, "%s", name);
		} else {
			snprintf(base_name_buffer, BUFSIZE, "%s%d", name, current + 1);
		}
		return base_name_buffer;
	} else {
		snprintf(extension_buffer, 4, "");
		if (N == 1) {
			snprintf(base_name_buffer, BUFSIZE, "%s", name);
		} else {
			snprintf(base_name_buffer, BUFSIZE, "%s%d", name, current + 1);
		}
		return base_name_buffer;
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
			std::cout << "Creating file: " << fname << '.' << extension_buffer << " - " << size_to_string(fsize, true) << "\n";
			if (!dry_run) {
				RandomFile rf(fname, extension_buffer, fsize);
				rf.write();
			}
		}
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

