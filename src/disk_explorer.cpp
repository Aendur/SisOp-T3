#include "disk_explorer.h"
#include <stdexcept>

DiskExplorer::DiskExplorer(void) {
	_ui.init();
	_page.init(&_ui);
	_device.open_drive('G');
	
	if (_device.geometry().BytesPerSector != 512) {
		throw std::runtime_error("mismatch assumed bytes per sector = 512");
	}

}

void DiskExplorer::run(void) {
	const DWORD LEN = _device.geometry().BytesPerSector;
	_ui.clear_screen();

	_device.print_geometry();
	
	_device.read();
	_page.set(_device.buffer(), LEN, _device.offset() - LEN);
	_page.print(false);
	

	while (_ui.read() != TERMUI_KEY_Q) {
		_device.read();
		_page.set(_device.buffer(), LEN, _device.offset() - LEN);
		_page.print(true);
	}
}


