#include "random_file.h"

#include <stdexcept>
#include <iostream>

int main (int argc, char ** argv) {
	if (argc < 3) {
		std::cout << "a.out file_name file_size\n\n";
		std::cout << "      file_name max 25 characters\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	try {
		size_t fsize = std::stoull(argv[2]);
		std::cout << "Creating file: " << argv[1] << " (" << fsize << ") bytes\n";
		RandomFile rf(argv[1], fsize);
		rf.write();
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

