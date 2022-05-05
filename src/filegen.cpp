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
		std::cout << "filegen [File name] [File size] [Num files] [rnd|seq] <dry>\n\n";
		std::cout << "        File size in bytes\n";
		std::cout << "        rnd: file with random characters\n";
		std::cout << "        seq: SeqFile\n";
		std::cout << "        dry: optional parameter for dry run (the file will not be written)\n";
		return 0;
	}
	bool seq_file = strcmp(argv[4], "seq") == 0;
	bool rnd_file = strcmp(argv[4], "rnd") == 0;
	bool dry_run  = argc >= 6;
	
	try {
		size_t fsize = std::stoull(argv[2]);
		int max_files = std::stoi(argv[3]);

		std::filesystem::path path(argv[1]);
		static char fextn[16] = "";
		static char fname[32] = "";

		if (path.has_extension()) {
			snprintf(fextn, SeqFile::RF_EXTN_LEN + 1, "%s", path.extension().string().substr(1,3).c_str());
		}
		snprintf(fname, SeqFile::RF_NAME_LEN + 1, "%s", path.stem().string().c_str());
		size_t len = strlen(fname);
		char * pos = fname + len;
		size_t rem = 1 + SeqFile::RF_NAME_LEN - len;

		for (int i = 0; i < max_files; ++i) {
			if (max_files > 1) {
				snprintf(pos, rem, "%d", i+1);
			}
			
			std::cout << "Creating file: " << fname << '.' << fextn << " - " << size_to_string(fsize, true) << "\n";
			SeqFile outfile(fname, fextn, fsize, rnd_file);
			
			char msg[4][256];
			auto checked = SeqFile::check_file(outfile, msg);
			if (seq_file && !checked.valid) {
				printf("%s\n%s\n%s\n%s\n", msg[0], msg[1], msg[2], msg[3]);
				throw std::runtime_error("invalid seqfile");
			}

			if (!dry_run) { outfile.write(); }

		}
	} catch (std::exception & e) {
		std::cout << "Error: " << e.what() << std::endl;
	}

	return 0;
}

