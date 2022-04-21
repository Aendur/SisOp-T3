#if defined(FILEGEN)
#include "random_file.h"
#include "utility.h"

#include <stdexcept>
#include <iostream>
#include <string>

int main (int argc, char ** argv) {
	if (argc < 3) {
		std::cout << "a.out file_name file_size\n\n";
		std::cout << "      file_name max 25 characters\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	try {
		size_t fsize = std::stoull(argv[2]);
		char size_str[STS_MAX_FORMAT_SIZE];
		std::cout << "Creating file: " << argv[1] << " - " << size_to_string(size_str, fsize, true) << "\n";
		RandomFile rf(argv[1], fsize);
		rf.write();
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

#elif defined(DISKEXP)

#include "disk_explorer.h"

int main(int, char **) {
	return 0;
}

#endif

