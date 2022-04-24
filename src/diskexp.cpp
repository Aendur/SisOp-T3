#include "disk_explorer.h"

int main(int argc, WCHAR ** argv) {
	if (argc < 2) {
		printf("Missing args - ");
		Device::check_drive(0);
		printf("usage:\n\tdiskexp <drive_letter>\n");
		return 0;
	}

	WCHAR drive = argv[1][0];
	if (Device::check_drive(drive)) {
		DiskExplorer de(drive);
		de.run();
	}

	return 0;
}
