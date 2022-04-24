#include "disk_explorer.h"
#include "entry.h"
#include "utility.h"
#include <stdexcept>
#include <vector>
#include <string>

DiskExplorer::DiskExplorer(WCHAR drive) {
	_ui.init();
	_device.open_drive(drive);
	_input.init(&_ui);
	
	if (_device.geometry().BytesPerSector != 512) {
		throw std::runtime_error("mismatch assumed bytes per sector = 512");
	}

	// save fat32 _sector0 data
	read_setpage();
	memcpy(&_sector0, _device.buffer(), _device.geometry().BytesPerSector);
	_page.init(_device.geometry().BytesPerSector, cluster_size());
	_editor.init(&_ui, &_page);
}

void DiskExplorer::print_commands(void) const {
	printf("\033[1;1H\n");
	printf("-- NAV --                 \n");
	printf("SPACE : show current sec  \n");
	printf("0     : show sector 0     \n");
	printf("1     : goto FirstDataSec \n");
	printf("g     : goto sector %-7lld\n", _sector_bookmark);
	printf("G     : goto sector (type)\n");
	printf("U_ARR : rewind %d sector%c\n", _adv_N, _adv_N == 1 ? ' ' : 's');
	printf("D_ARR : forwrd %d sector%c\n", _adv_N, _adv_N == 1 ? ' ' : 's');
	printf("LR_ARR: set N=%-10d \n", _adv_N);
	printf("PGUP  : goto prev cluster \n");
	printf("PGDOWN: goto next cluster \n");
	printf("HOME  : goto first sector \n");
	printf("END   : goto last sector  \n");
	printf("-- DISP --                \n");
	printf("TAB : toggle sector disp. \n");
	printf("^TAB: toggle entry disp.  \n");
	printf("d/D : show drive info     \n");
	printf("q/Q : quit                \n");

	if (_show_drive_info) {
		printf("\n-- GEOMETRY --\n");
		show_geom_info();
		printf("\n-- FAT32 --\n");
		show_fat32_info();
	} else {
		printf("\033[0J");
	}


}

void DiskExplorer::run(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;

	_ui.clear_screen();
	print_commands();
	_page.print();

	KeyCode key = TERMUI_KEY_UNDEFINED;
	while ((key = _ui.read()) != TERMUI_KEY_q && key != TERMUI_KEY_Q) {
		switch(key) {
		case TERMUI_KEY_TAB        : _page.toggle_mode()                             ;                 break;
		case TERMUI_KEY_STAB       : _page.toggle_extended()                         ;                 break;
		case TERMUI_KEY_0          : _page.set((PBYTE)(&_sector0), 0)                ;                 break;
		case TERMUI_KEY_1          : goto_sector(fds_offset())                       ; read_setpage(); break;
		case TERMUI_KEY_d          :
		case TERMUI_KEY_D          : _show_drive_info = !_show_drive_info            ;                 break;
		//case TERMUI_KEY_e          :
		//case TERMUI_KEY_E          : _page.toggle_extended()                         ;                 break;
		case TERMUI_KEY_g          : goto_sector(_sector_bookmark * (long) LEN)      ; read_setpage(); break;
		case TERMUI_KEY_G          : input_and_go()                                  ;                 break;
		//case TERMUI_KEY_G          : printf("\033[48;1HINPUT FIELD WIP");            ;                 break;
		case TERMUI_KEY_ARROW_UP   : advance_sectors(-(_adv_N+1) * (long) LEN)       ; read_setpage(); break;
		case TERMUI_KEY_ARROW_DOWN : advance_sectors( (_adv_N-1) * (long) LEN)       ; read_setpage(); break;
		case TERMUI_KEY_ARROW_RIGHT: _adv_N = _adv_N < 100000 ? _adv_N * 10 : 1000000;                 break;
		case TERMUI_KEY_ARROW_LEFT : _adv_N = _adv_N > 10     ? _adv_N / 10 : 1      ;                 break;
		case TERMUI_KEY_PGUP       : advance_sectors(-(LONGLONG) cluster_size()-LEN) ; read_setpage(); break;
		case TERMUI_KEY_PGDOWN     : advance_sectors( (LONGLONG) cluster_size()-LEN) ; read_setpage(); break;
		case TERMUI_KEY_HOME       : goto_sector(         0)                         ; read_setpage(); break;
		case TERMUI_KEY_END        : goto_sector(-(long)LEN)                         ; read_setpage(); break;
		case TERMUI_KEY_SPACE      : setpage()                                       ;                 break;
		default                    : setpage()                                       ;                 break;
		}
		print_commands();
		_page.print();
	}
}

void DiskExplorer::setpage(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	_page.set(_device.buffer(), _device.offset() - LEN);
}

void DiskExplorer::read_setpage(void) {
	_device.read();
	setpage();
}

void DiskExplorer::advance_sectors(LONGLONG offset) {
	static const DWORD LEN = _device.geometry().BytesPerSector;
	LONGLONG mod = _device.capacity() % LEN;
	LONGLONG max_off = _device.capacity() - mod - LEN;

	if (_device.offset() + offset < 0) {
		offset = -_device.offset();
	} else if (_device.offset() + offset > max_off) {
		offset = max_off - _device.offset();
	} else if (offset % LEN != 0) {
		offset -= offset % LEN;
		wprintf(L"must offset by sector size!!");
	}
	_device.seek(offset, true);
}

void DiskExplorer::goto_sector(LONGLONG offset) {
	if (offset < 0) { offset = _device.capacity() + offset; }

	static const DWORD LEN = _device.geometry().BytesPerSector;
	LONGLONG mod = _device.capacity() % LEN;
	LONGLONG max_off = _device.capacity() - LEN - mod;

	if (offset < 0) {
		offset = 0;
	} else if (offset > max_off) {
		offset = max_off;
	} else {
		offset -= offset % LEN;
	}
	_device.seek(offset, false);
}

void DiskExplorer::input_and_go(void) {
	if (_input.get(&_sector_bookmark)) {
		goto_sector(_sector_bookmark * _device.geometry().BytesPerSector);
		read_setpage();
	}
}

void DiskExplorer::show_geom_info(void) const {
	wprintf(L"MediaType         %d\n",   _device.geometry().MediaType);
	wprintf(L"Cylinders (Quad)  %lld\n", _device.geometry().Cylinders.QuadPart);
	wprintf(L"Cylinders (High)  %d\n",   _device.geometry().Cylinders.HighPart);
	wprintf(L"Cylinders (Low)   %d\n",   _device.geometry().Cylinders.LowPart);
	wprintf(L"TracksPerCylinder %d\n",   _device.geometry().TracksPerCylinder);
	wprintf(L"SectorsPerTrack   %d\n",   _device.geometry().SectorsPerTrack);
	wprintf(L"BytesPerSector    %d\n",   _device.geometry().BytesPerSector);
	wprintf(L"Total capacity    %lld B\n", _device.capacity());
	wprintf(L"      %s\n"    , size_to_wstring(_device.capacity(), true));
	//fwprintf(_out, L"NBytes            %d\n", _geom_nbytes);
}

void DiskExplorer::show_fat32_info(void) const {
	printf("BPB_FATSz16    : %u\n", _sector0.BPB_FATSz16());
	printf("BPB_FATSz32    : %u\n", _sector0.BPB_FATSz32());
	printf("BPB_NumFATs    : %u\n", _sector0.BPB_NumFATs());
	printf("BPB_RsvdSecCnt : %u\n", _sector0.BPB_RsvdSecCnt());
	printf("Bytes/sector   : %u\n", _sector0.BPB_BytsPerSec());
	printf("Sectors/cluster: %u\n", _sector0.BPB_SecPerClus());
	printf("BPB_RootClus   : %d\n", _sector0.BPB_RootClus());
	printf("\n");
	printf("Cluster size   : %lu\n", cluster_size());
	printf("FirstDataSector: %lu\n" , first_data_sector());
	printf("FDS offset     : %llu\n", fds_offset());
	// size_t FirstSectorofCluster = ((N - 2) * _sector0.BPB_SecPerClus()) + FirstDataSector
	// size_t FirstSectorofCluster = ((N - 2) * _sector0.BPB_SecPerClus()) + FirstDataSector
	// SEEK
	//FDS_offset
}


ULONG DiskExplorer::cluster_size(void) const {
	return _sector0.BPB_BytsPerSec() * _sector0.BPB_SecPerClus();
}
LONG DiskExplorer::first_data_sector(void) const {
	return _sector0.BPB_RsvdSecCnt() + _sector0.BPB_FATSz32() * _sector0.BPB_NumFATs();
}
LONGLONG DiskExplorer::fds_offset(void) const {
	return first_data_sector() * _sector0.BPB_BytsPerSec();
}
