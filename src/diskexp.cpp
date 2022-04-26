#include "disk_explorer.h"

#include <stdexcept>

int main(int argc, WCHAR ** argv) {
	if (argc < 2) {
		printf("Missing args - ");
		Device::check_drive(0);
		printf("usage:\n\tdiskexp <drive_letter>\n");
		return 0;
	}

	WCHAR drive = argv[1][0];
	if (Device::check_drive(drive)) {
		try {
			DiskExplorer de(drive);
			de.run();
		} catch (std::exception & e) {
			printf("\n\n\n\n\n%s\n\n\n\n\n", e.what());
		}
	}

	return 0;
}
