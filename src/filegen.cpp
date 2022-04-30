#include "seqfile.h"
#include "utility.h"

#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <string>
#include <filesystem>


int main (int argc, char ** argv) {
	if (argc < 5) {
		std::cout << "a.out [File name] [File size] [Num files] [rnd|seq] <dry>\n\n";
		std::cout << "      file_size in bytes\n";
		return 0;
	}
	//bool seq_file = strcmp(argv[4], "seq") == 0;
	bool rnd_file = strcmp(argv[4], "rnd") == 0;
	bool dry_run  = argc >= 6;
	
	try {
		size_t fsize = std::stoull(argv[2]);
		int max_files = std::stoi(argv[3]);

		static char fext[16] = "";
		static char fname[1024];
		std::filesystem::path path = std::filesystem::absolute(argv[1]);
		if (path.has_extension()) {
			snprintf(fext, 16, "%s", path.extension().string().substr(1,5).c_str());
		}

		
		char * pos;
		if (path.has_parent_path()) {
			snprintf(fname, 1024, "%s\\", path.parent_path().string().c_str());
		} else {
			snprintf(fname, 1024, ".\\");
		}
		
		pos = fname + strlen(fname);
		snprintf(pos, 512, "%s", path.stem().string().c_str());
		pos = fname + strlen(fname);

		for (int i = 0; i < max_files; ++i) {
			if (max_files > 1) {
				snprintf(pos, 512, "%d", i+1);
			}
			
			std::cout << "Creating file: " << fname << '.' << fext << " - " << size_to_string(fsize, true) << "\n";
			SeqFile outfile(fname, fext, fsize, rnd_file);
			if (!dry_run) { outfile.write(); }

		}
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

