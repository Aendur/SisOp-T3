#include "seqfile.h"
#include "rndfile.h"
#include "utility.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>



int main (int argc, char ** argv) {
	srand((unsigned int) time(NULL));
	if (argc < 5) {
		std::cout << "a.out [File name] [File size] [Num files] [rnd|seq] <dry>\n\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	bool seq_file = strcmp(argv[4], "seq") == 0;
	bool rnd_file = strcmp(argv[4], "rnd") == 0;
	bool dry_run  = argc >= 6;
	
	try {
		size_t fsize = std::stoull(argv[2]);
		int max_files = std::stoi(argv[3]);

		static char fext[16] = "";
		static char fname[1024];
		std::filesystem::path path(argv[1]);
		if (path.has_extension()) {
			snprintf(fext, 16, "%s", path.extension().string().substr(1,5).c_str());
		}

		snprintf(fname, 1024, "%s", path.stem().string().c_str());
		char * pos = fname + strlen(fname);
	
		for (int i = 0; i < max_files; ++i) {
			snprintf(pos, 512, "%d", i);
			
			std::cout << "Creating file: " << fname << '.' << fext << " - " << size_to_string(fsize, true) << "\n";
			if (seq_file) {
				SeqFile outfile(fname, fext, fsize);
				if (!dry_run) { outfile.write(); }
			} else {
				RndFile outfile(fname, fext, fsize);
				if (!dry_run) { outfile.write(); }
			}
		}
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

