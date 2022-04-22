#include "disk_explorer.h"


DiskExplorer::DiskExplorer(void) {
	_ui.init();
	_device.open_drive('G');
}


void DiskExplorer::run(void) {
	_device.print_geometry();
}


