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

void print_commands(void) {
	printf("\n");
	printf("Q:    quit                  UP:     last sector         PGUP:     rew 10000 sectors\n");
	printf("0:    show sector 0         DOWN:   next sector         PGDOWN:   fwd 10000 sectors\n");
	printf("HOME: goto first sector     LEFT:   fwd 100 sectors     S+PGUP:   rew 1000000 sectors\n");
	printf("END:  goto last sector      RIGHT:  rew 100 sectors     S+PGDOWN: fwd 1000000 sectors\n");
}

void DiskExplorer::run(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	_ui.clear_screen();
	_device.print_geometry();

	// save fat32 sector0 data
	_device.read();
	memcpy(&sector0, _device.buffer(), _device.geometry().BytesPerSector);
	set_print();

	
	KeyCode key = TERMUI_KEY_UNDEFINED;
	while ((key = _ui.read()) != TERMUI_KEY_Q) {
		switch(key) {
		case TERMUI_KEY_0:
			_page.set((PBYTE)(&sector0), LEN, 0);
			_page.print(true);
			break;
		case TERMUI_KEY_ARROW_UP: advance_sectors(-2 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_ARROW_DOWN: read_set_print(); break;
		case TERMUI_KEY_ARROW_LEFT: advance_sectors(99 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_ARROW_RIGHT: advance_sectors(-101 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_HOME: advance_sectors(-_device.offset()); read_set_print(); break;
		case TERMUI_KEY_END: advance_sectors(_device.capacity()); read_set_print(); break;
		case TERMUI_KEY_PGUP: advance_sectors(-10001 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_PGDOWN: advance_sectors(9999 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_SHIFT_PGUP: advance_sectors(-1000001 * (long) LEN); read_set_print(); break;
		case TERMUI_KEY_SHIFT_PGDOWN: advance_sectors(999999 * (long) LEN); read_set_print(); break;
		default:
			set_print();
			break;
		}
	}
}

void DiskExplorer::set_print(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	
	_page.set(_device.buffer(), LEN, _device.offset() - LEN);
	_page.print(true);
	print_commands();
}

void DiskExplorer::read_set_print(void) {
	_device.read();
	set_print();
}

void DiskExplorer::advance_sectors(LONGLONG offset) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	LONGLONG mod = _device.capacity() % LEN;
	LONGLONG max_off = _device.capacity() - mod - LEN;
	wprintf(L"MOD %lld\b", mod);

	if (_device.offset() + offset < 0) {
		offset = -_device.offset();
	} else if (_device.offset() + offset > max_off) {
		offset = max_off - _device.offset();
	} else if (offset % LEN != 0) {
		offset -= offset % LEN;
		wprintf(L"must offset by sector size!!");
	}
	_device.seek(offset);
}

