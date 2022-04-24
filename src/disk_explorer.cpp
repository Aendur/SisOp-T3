#include "disk_explorer.h"
#include <stdexcept>
#include <vector>
#include <string>

DiskExplorer::DiskExplorer(void) {
	_ui.init();
	_device.open_drive('G');
	
	if (_device.geometry().BytesPerSector != 512) {
		throw std::runtime_error("mismatch assumed bytes per sector = 512");
	}
}

void DiskExplorer::print_commands(void) {
	printf("\033[1;1H\n\n");
	printf("-- NAV --                 \n");
	printf("SPACE  : show current sec \n");
	printf("0      : show sector 0    \n");
	printf("1      : goto FirstDataSec\n");
	printf("UP     : rew %d sector%c  \n", _adv_N, _adv_N == 1 ? ' ' : 's');
	printf("DOWN   : fwd %d sector%c  \n", _adv_N, _adv_N == 1 ? ' ' : 's');
	printf("L/R    : set N=%-10d\n", _adv_N);
	printf("PGUP   : goto last cluster\n");
	printf("PGDOWN : goto next cluster\n");
	printf("HOME   : goto first sector\n");
	printf("END    : goto last sector \n");
	printf("-- DISP --                \n");
	printf("TAB : toggle display mode \n");
	printf("D   : show drive info     \n");
	printf("F   : show fat32 info     \n");
	printf("Q   : quit                \n");
}

void DiskExplorer::run(void) {
	static const DWORD LEN = _device.geometry().BytesPerSector;

	// save fat32 _sector0 data
	read_setpage();
	memcpy(&_sector0, _device.buffer(), _device.geometry().BytesPerSector);
	_page.init(_device.geometry().BytesPerSector, cluster_size());

	_ui.clear_screen();
	print_commands();
	_page.print();

	
	KeyCode key = TERMUI_KEY_UNDEFINED;
	while ((key = _ui.read()) != TERMUI_KEY_Q) {
		switch(key) {
		case TERMUI_KEY_TAB        : _page.toggle_mode()                             ;                 break;
		case TERMUI_KEY_0          : _page.set((PBYTE)(&_sector0), 0)                ;                 break;
		case TERMUI_KEY_1          : goto_sector(fds_offset())                       ; read_setpage(); break;
		case TERMUI_KEY_D          : printf("\033[0J"); _device.print_geometry()     ;                 break;
		case TERMUI_KEY_F          : show_fat32_info()                               ;                 break;
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

void DiskExplorer::show_fat32_info(void) {
	printf("\033[0J");
	printf("BPB_FATSz16:     %u\n", _sector0.BPB_FATSz16());
	printf("BPB_FATSz32:     %u\n", _sector0.BPB_FATSz32());
	printf("BPB_NumFATs:     %u\n", _sector0.BPB_NumFATs());
	printf("BPB_RsvdSecCnt:  %u\n", _sector0.BPB_RsvdSecCnt());
	printf("Bytes/sector:    %u\n", _sector0.BPB_BytsPerSec());
	printf("Sectors/cluster: %u\n", _sector0.BPB_SecPerClus());
	printf("Cluster size   : %lu\n", cluster_size());
	printf("\n");
	printf("FirstDataSector: %lu\n" , first_data_sector());
	printf("FDS offset     : %llu\n", fds_offset());
	printf("BPB_RootClus   : %d\n"  , _sector0.BPB_RootClus());

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
